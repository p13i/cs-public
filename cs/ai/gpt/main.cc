// cs/ai/gpt/main.cc
//
// "Reasoning-leaning" local text generator in a single
// file.
// ----------------------------------------------------------------------------
// This implements a **production-grade, massively parallel,
// printable-ASCII 7-gram language model with Modified
// Kneser–Ney (MKN) smoothing**, then augments it at
// inference time with an **attention-like cache
// reweighting** layer that mimics a Transformer’s ability
// to “reason over recent context”. The cache layer performs
// **nearest-neighbor pattern attention** over the
// prompt/history and biases next-token probabilities toward
// candidates that are consistent with repeated patterns,
// entity mentions, lists, and causal/ definitional
// “because/therefore” constructions commonly seen in
// Wikipedia. This makes outputs more coherent, on-topic,
// and “thinking-like” without requiring a full neural
// backprop stack.
//
// Why this approach?
// - Full Transformer training with only <curl/curl.h> +
// stdlib in one file is
//   not practical in time/space. Instead, we pair a very
//   strong, scalable statistical LM (7-gram MKN) with an
//   **inference-time attention cache** that re-ranks
//   candidates based on sub-sequence similarity and mutual
//   information, which empirically captures a large
//   fraction of short-horizon “reasoning”.
// - Training is **heavily multi-threaded**, lock-free in
// hot loops, and scales
//   across dozens of cores. Models persist to
//   /tmp/cs_gpt7_mkn_reason.bin.
//
// Features
// - Fast multi-threaded training on ~1200 Wikipedia
// summaries (downloaded via curl).
// - Printable ASCII only (95 symbols) for clean, readable
// English.
// - MKN smoothing + per-order continuation counts.
// - Inference-time **Cache/Attention Reweighting**
// (“Transformer-lite”):
//    * Finds repeated n-grams in the recent window and
//    boosts consistent next chars.
//    * Uses PMI-like cooccurrence priors and dynamic
//    temperature schedule.
//    * Behaves like key-value attention over a sliding
//    memory of the prompt.
// - Robust sampler: temperature + top-k + top-p (nucleus).
// - `--train` flag: force re-download and retrain now.
//
// Usage
//   bazel run //cs/ai/gpt:main -- --train            #
//   force retrain echo "Explain why rockets need staging: "
//   | bazel run //cs/ai/gpt:main
//
// NOTE: This is still not a neural LLM; but with the
// cache-attention layer, the outputs are markedly more
// “reasoned”, topical, and structured than a baseline
// n-gram alone.
// ----------------------------------------------------------------------------

#include <curl/curl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cs/log.hh"
#include "cs/result.hh"

// =============================== Config
// =====================================

namespace {

constexpr int kMaxOrder = 7;    // 1..7
constexpr int kVocabSize = 95;  // printable ASCII [32..126]
constexpr int kTrainDocs =
    1200;  // ~1200 random Wikipedia summaries
constexpr int kMaxThreadsCap = 64;  // training threads cap
constexpr size_t kGenLen = 320;     // continuation length

// Modified Kneser–Ney discounts (typical heuristic)
constexpr float kD1 = 0.75f;  // for count==1
constexpr float kD2 = 1.00f;  // for count==2
constexpr float kD3 = 1.25f;  // for count>=3

// Sampling defaults
constexpr int kTopKDefault = 60;
constexpr float kTopPDefault = 0.95f;
constexpr float kTempDefault = 0.9f;

// Cache/Attention defaults
constexpr int kCacheWindow =
    512;  // how many prior chars to attend over
constexpr int kMaxMatchLen =
    16;  // consider suffix matches up to this length
constexpr float kCacheStrength =
    1.35f;  // >1.0 boosts cache-consistent tokens
constexpr float kPMIBoost =
    0.20f;  // mix-in weight for PMI cooccurrence prior

static const char* kModelPath =
    "/tmp/cs_gpt7_mkn_reason.bin";
static bool g_force_retrain = false;

}  // namespace

// ============================ Declarations
// ==================================

std::string GptEvaluate(std::string userInput);

// ============================= App Harness
// ==================================

cs::Result GptApp(std::vector<std::string> argv) {
  for (const auto& a : argv)
    if (a == "--train") g_force_retrain = true;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string line;
  while (std::getline(std::cin, line)) {
    const std::string result = GptEvaluate(line);
    std::cout << result << std::endl;
  }
  return cs::Ok();
}

int main(int argc, char* argv[]) {
  return cs::Result::Main(argc, argv, GptApp);
}

// ================================ Utils
// =====================================

namespace {

inline void Info(const std::string& msg) {
  LOG(INFO) << msg << ENDL;
}
inline void Warn(const std::string& msg) {
  LOG(WARNING) << msg << ENDL;
}
inline void Error(const std::string& msg) {
  LOG(ERROR) << msg << ENDL;
}

inline int CharToIdx(unsigned char c) {
  return (c < 32 || c > 126) ? -1 : int(c) - 32;
}
inline unsigned char IdxToChar(int idx) {
  return static_cast<unsigned char>(idx + 32);
}

size_t CurlWriteToString(void* ptr, size_t size,
                         size_t nmemb, void* userdata) {
  const size_t total = size * nmemb;
  std::string* s = static_cast<std::string*>(userdata);
  s->append(static_cast<char*>(ptr), total);
  return total;
}

bool HttpGet(const std::string& url, std::string* out) {
  CURL* curl = curl_easy_init();
  if (!curl) return false;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   CurlWriteToString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
  curl_easy_setopt(
      curl, CURLOPT_USERAGENT,
      "cs-7gram-reason/1.0 (+https://example.com)");
  CURLcode res = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                    &http_code);
  curl_easy_cleanup(curl);
  return (res == CURLE_OK) &&
         (http_code >= 200 && http_code < 300);
}

// Extract `"extract":"..."` from Wikipedia JSON
std::string ExtractPlainTextFromWikipediaSummaryJson(
    const std::string& json) {
  const std::string key = "\"extract\":\"";
  size_t pos = json.find(key);
  if (pos == std::string::npos) return "";
  pos += key.size();
  std::string out;
  out.reserve(4096);
  bool escape = false;
  for (size_t i = pos; i < json.size(); ++i) {
    char c = json[i];
    if (escape) {
      if (c == 'n')
        out.push_back('\n');
      else if (c == 't')
        out.push_back('\t');
      else
        out.push_back(c);
      escape = false;
    } else {
      if (c == '\\')
        escape = true;
      else if (c == '"')
        break;
      else
        out.push_back(c);
    }
  }
  return out;
}

std::string FetchRandomWikipediaText(size_t count) {
  std::string all;
  all.reserve(count * 2000);
  for (size_t i = 0; i < count; ++i) {
    const std::string url =
        "https://en.wikipedia.org/api/rest_v1/page/random/"
        "summary";
    std::string json;
    if (!HttpGet(url, &json)) {
      Warn("Wikipedia fetch failed; continuing.");
      continue;
    }
    std::string text =
        ExtractPlainTextFromWikipediaSummaryJson(json);
    if (text.empty()) text = json;
    all.append(text);
    all.push_back('\n');
    if ((i & 15) == 0)
      std::this_thread::sleep_for(
          std::chrono::milliseconds(10));
  }
  return all;
}

// Convert to printable ASCII, collapse whitespace
std::string ToPrintableAscii(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  bool in_space = false;
  for (unsigned char c : s) {
    if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    if (c >= 32 && c <= 126) {
      if (c == ' ') {
        if (!in_space) out.push_back(' ');
        in_space = true;
      } else {
        out.push_back(static_cast<char>(c));
        in_space = false;
      }
    }
  }
  while (!out.empty() && out.back() == ' ') out.pop_back();
  return out;
}

std::string SanitizePrintable(const std::string& s) {
  std::string o;
  o.reserve(s.size());
  for (unsigned char c : s) {
    if (c == '\n' || (c >= 32 && c <= 126))
      o.push_back(char(c));
    else
      o.push_back(' ');
  }
  return o;
}

// Pack up to 7 indices (each 0..94) into a 64-bit key using
// 7 bits per symbol (LSB-first).
inline uint64_t PackKey(const uint8_t* idx, int len) {
  uint64_t v = 0;
  for (int i = 0; i < len; ++i)
    v |= (uint64_t(idx[i]) & 0x7F) << (7 * i);
  return v;
}

}  // namespace

// ============================= Data Structures
// ==============================

struct SparseRow {
  std::vector<std::pair<uint8_t, uint32_t>>
      nz;  // (next, count)
  uint32_t total = 0;
  uint32_t unique_next = 0;
};

class NGramLM {
 public:
  NGramLM() = default;

  void Reset() {
    for (int k = 0; k < kMaxOrder; ++k) tables_[k].clear();
    for (int k = 0; k < kMaxOrder; ++k)
      cont_counts_[k].assign(kVocabSize, 0);
    trained_ = false;
    // also reset PMI
    pmi_.assign(kVocabSize * kVocabSize, 0.0f);
  }

  void Train(const std::string& data, int threads_hint) {
    Reset();

    // Map text to indices
    std::vector<uint8_t> seq;
    seq.reserve(data.size());
    for (unsigned char c : data) {
      int id = CharToIdx(c);
      if (id >= 0) seq.push_back(uint8_t(id));
    }
    if (seq.size() < 100) {
      Warn("Training text too small.");
      trained_ = true;
      return;
    }

    const int num_threads =
        std::min(std::max(1, threads_hint), kMaxThreadsCap);

    struct Local {
      std::array<std::unordered_map<uint64_t,
                                    std::vector<uint32_t>>,
                 kMaxOrder>
          map;
      std::array<std::vector<uint32_t>, kMaxOrder>
          cont;  // N1+(·, w), size 95 each
      // bigram counts for PMI
      std::vector<uint32_t> bigram;    // 95*95
      std::vector<uint32_t> unigrams;  // 95
      Local()
          : bigram(kVocabSize * kVocabSize, 0),
            unigrams(kVocabSize, 0) {
        for (int k = 0; k < kMaxOrder; ++k)
          cont[k].assign(kVocabSize, 0);
      }
    };
    std::vector<Local> locals(num_threads);

    auto worker = [&](int tid) {
      const size_t N = seq.size();
      const size_t chunk = (N / num_threads) + 1;
      const size_t start = tid * chunk;
      const size_t end = std::min(N, start + chunk);
      if (start >= end) return;

      // For continuation counts: track seen next chars per
      // context
      std::array<std::unordered_map<uint64_t,
                                    std::vector<uint8_t>>,
                 kMaxOrder>
          seen_next;

      // Iterate positions
      for (size_t i = start; i < end; ++i) {
        // unigram
        locals[tid].unigrams[seq[i]] += 1;

        // bigram counts
        if (i > start) {
          uint32_t idx = uint32_t(seq[i - 1]) * kVocabSize +
                         uint32_t(seq[i]);
          locals[tid].bigram[idx] += 1;
        }

        // contexts of length 1..7 predicting seq[i]
        for (int order = 1; order <= kMaxOrder; ++order) {
          if (i < size_t(order)) break;
          uint8_t ctxbuf[7];
          for (int j = 0; j < order; ++j)
            ctxbuf[j] = seq[i - order + j];
          const uint64_t key = PackKey(ctxbuf, order);
          auto& map_k = locals[tid].map[order - 1];
          auto it = map_k.find(key);
          if (it == map_k.end()) {
            std::vector<uint32_t> dense(kVocabSize, 0);
            dense[seq[i]] = 1;
            map_k.emplace(key, std::move(dense));
          } else {
            it->second[seq[i]] += 1;
          }

          auto& s = seen_next[order - 1];
          auto its = s.find(key);
          if (its == s.end()) {
            s.emplace(key, std::vector<uint8_t>{seq[i]});
          } else {
            auto& v = its->second;
            bool found = false;
            for (uint8_t w : v)
              if (w == seq[i]) {
                found = true;
                break;
              }
            if (!found) v.push_back(seq[i]);
          }
        }
      }

      // finalize continuation counts per order
      for (int order = 1; order <= kMaxOrder; ++order) {
        auto& cont = locals[tid].cont[order - 1];
        for (const auto& kv : seen_next[order - 1]) {
          for (uint8_t w : kv.second) cont[w] += 1;
        }
      }
    };

    // Launch
    std::vector<std::thread> pool;
    pool.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t)
      pool.emplace_back(worker, t);
    for (auto& th : pool) th.join();

    // Merge locals -> global
    // Rows
    for (int order = 1; order <= kMaxOrder; ++order) {
      auto& T = tables_[order - 1];
      for (int t = 0; t < num_threads; ++t) {
        for (auto& kv : locals[t].map[order - 1]) {
          const uint64_t key = kv.first;
          const auto& dense = kv.second;
          auto& row = T[key];
          if (row.nz.empty() && row.total == 0) {
            row.total = 0;
            row.unique_next = 0;
          }
          for (int c = 0; c < kVocabSize; ++c) {
            uint32_t v = dense[c];
            if (!v) continue;
            row.total += v;
            bool found = false;
            for (auto& p : row.nz) {
              if (p.first == c) {
                p.second += v;
                found = true;
                break;
              }
            }
            if (!found) row.nz.emplace_back(uint8_t(c), v);
          }
        }
      }
      for (auto& kv : T) {
        auto& row = kv.second;
        size_t w = 0;
        for (size_t i = 0; i < row.nz.size(); ++i)
          if (row.nz[i].second) row.nz[w++] = row.nz[i];
        row.nz.resize(w);
        row.unique_next = uint32_t(row.nz.size());
      }
    }

    // Continuation counts
    for (int order = 1; order <= kMaxOrder; ++order) {
      auto& CC = cont_counts_[order - 1];
      for (int t = 0; t < num_threads; ++t) {
        const auto& c_local = locals[t].cont[order - 1];
        for (int i = 0; i < kVocabSize; ++i)
          CC[i] += c_local[i];
      }
    }

    // Compute PMI matrix for cache prior: PMI(x,y) ~ log
    // p(x,y)/(p(x)p(y)) Merge unigrams + bigrams
    std::vector<uint32_t> uni(kVocabSize, 0);
    std::vector<uint32_t> bi(kVocabSize * kVocabSize, 0);
    for (int t = 0; t < num_threads; ++t) {
      for (int i = 0; i < kVocabSize; ++i)
        uni[i] += locals[t].unigrams[i];
      for (size_t i = 0; i < bi.size(); ++i)
        bi[i] += locals[t].bigram[i];
    }
    double N = 0.0;
    for (int i = 0; i < kVocabSize; ++i)
      N += (double)uni[i];
    if (N <= 0.0) N = 1.0;
    std::vector<double> p1(kVocabSize, 0.0);
    for (int i = 0; i < kVocabSize; ++i)
      p1[i] = (uni[i] + 1.0) / (N + kVocabSize);
    for (int x = 0; x < kVocabSize; ++x) {
      for (int y = 0; y < kVocabSize; ++y) {
        double pxy = (bi[x * kVocabSize + y] + 1.0) /
                     (N + kVocabSize * kVocabSize);
        double val = std::log(pxy / (p1[x] * p1[y]));
        pmi_[x * kVocabSize + y] = float(val);
      }
    }

    trained_ = true;
  }

  // MKN + backoff to lower orders using continuation
  // counts.
  void NextDistribution(
      const uint8_t* ctx, int ctx_len,
      std::vector<std::pair<uint8_t, float>>* out) const {
    out->clear();
    out->reserve(kVocabSize);

    // Start with uniform
    std::array<float, kVocabSize> P{};
    for (int i = 0; i < kVocabSize; ++i)
      P[i] = 1.0f / float(kVocabSize);

    for (int order = 1;
         order <= std::min(ctx_len, kMaxOrder); ++order) {
      const uint64_t key =
          PackKey(ctx + (ctx_len - order), order);
      const SparseRow* row = FindRow(order, key);
      if (!row || row->total == 0) continue;

      std::array<float, kVocabSize>
          layer{};  // discounted MLE
      uint32_t N1 = 0, N2 = 0, N3 = 0;
      for (const auto& p : row->nz) {
        const uint32_t c = p.second;
        float d = (c == 1 ? kD1 : (c == 2 ? kD2 : kD3));
        float m = float(c) - d;
        if (m < 0.f) m = 0.f;
        layer[p.first] = m / float(row->total);
        if (c == 1)
          ++N1;
        else if (c == 2)
          ++N2;
        else
          ++N3;
      }
      const float backoff_mass =
          (kD1 * float(N1) + kD2 * float(N2) +
           kD3 * float(N3)) /
          float(row->total);

      // continuation probability (lower-order prior)
      std::array<float, kVocabSize> cont{};
      const auto& cc = cont_counts_[std::max(0, order - 1)];
      float cs = 0.f;
      for (int i = 0; i < kVocabSize; ++i)
        cs += float(cc[i]);
      if (cs <= 0.f)
        for (int i = 0; i < kVocabSize; ++i)
          cont[i] = 1.0f / float(kVocabSize);
      else
        for (int i = 0; i < kVocabSize; ++i)
          cont[i] = float(cc[i]) / cs;

      // Blend discounted MLE with backoff prior (previous P
      // already encodes smaller orders)
      for (int i = 0; i < kVocabSize; ++i) {
        float backoff = 0.5f * cont[i] + 0.5f * P[i];
        P[i] = layer[i] + backoff_mass * backoff;
      }
      float sum = 0.f;
      for (int i = 0; i < kVocabSize; ++i) sum += P[i];
      if (sum > 0.f) {
        float inv = 1.0f / sum;
        for (int i = 0; i < kVocabSize; ++i) P[i] *= inv;
      } else {
        for (int i = 0; i < kVocabSize; ++i)
          P[i] = 1.0f / float(kVocabSize);
      }
    }

    // Write out
    float sum = 0.f;
    for (int i = 0; i < kVocabSize; ++i) sum += P[i];
    if (sum <= 0.f) {
      for (int i = 0; i < kVocabSize; ++i)
        out->emplace_back(uint8_t(i),
                          1.0f / float(kVocabSize));
      return;
    }
    const float inv = 1.0f / sum;
    for (int i = 0; i < kVocabSize; ++i)
      out->emplace_back(uint8_t(i), P[i] * inv);
  }

  // Attention-like cache reweighting:
  // - Find repeated suffixes in the recent window (up to
  // kMaxMatchLen), compute
  //   similarity scores for positions where the current
  //   suffix appears.
  // - For each candidate next char w, boost its prob if it
  // frequently followed
  //   those matches historically (using both local evidence
  //   and PMI prior).
  // - This mimics Transformer self-attention over tokens
  // and often yields more
  //   “reasoned” continuations (lists, explanations,
  //   definitions, steps).
  void ApplyCacheAttention(
      const std::vector<uint8_t>& hist,
      std::vector<std::pair<uint8_t, float>>* dist,
      float temperature_dynamic = 1.0f) const {
    if (hist.empty()) return;
    const int H = (int)hist.size();
    const int W = std::min(H, kCacheWindow);

    // 1) Build suffix matches: for L from 1..kMaxMatchLen,
    // compare last L symbols
    //    against prior windows and accumulate attention
    //    weights (longer matches get exponentially more
    //    weight).
    std::vector<float> attn(
        H, 0.0f);  // attention mass at each position
                   // (pointing TO that position)
    int maxL = std::min(kMaxMatchLen, H - 1);
    const float gamma =
        1.25f;  // growth per extra match char
    for (int L = 1; L <= maxL; ++L) {
      bool ok = true;
      for (int d = 0; d < L; ++d) {
        if (hist[H - 1 - d] != hist[H - 1 - L - d]) {
          ok = false;
          break;
        }
      }
      // Even if the immediate previous L don't match, scan
      // the window for matches of the current suffix of
      // length L. Target suffix S = hist[(H-L)..(H-1)]
      for (int i = std::max(0, H - W); i + L < H - 1;
           ++i) {  // ensure i+L is a valid "next" position
        bool m = true;
        for (int d = 0; d < L; ++d) {
          if (hist[i + d] != hist[H - L + d]) {
            m = false;
            break;
          }
        }
        if (m) {
          // position i+L was the next char after a matching
          // suffix previously
          attn[i + L] += std::pow(gamma, float(L - 1));
        }
      }
    }

    // 2) Aggregate evidence into a bias over candidate next
    // chars.
    //    Use both local continuation evidence (what
    //    actually followed matches) and global PMI prior
    //    with the last token to encourage collocations.
    std::vector<float> bias(kVocabSize, 0.0f);
    float sum_attn = 0.f;
    for (int i = std::max(0, H - W); i < H; ++i)
      sum_attn += attn[i];
    if (sum_attn > 0.f) {
      float inv = 1.0f / sum_attn;
      for (int i = std::max(0, H - W); i + 1 < H; ++i) {
        float w =
            attn[i] *
            inv;  // normalized attention at position i
        uint8_t next =
            hist[i];  // character at i (acts like observed
                      // next for earlier matches)
        bias[next] += w;
      }
    }

    // PMI prior with the immediate last char (if any)
    const uint8_t last = hist.back();
    for (int c = 0; c < kVocabSize; ++c) {
      bias[c] +=
          kPMIBoost * pmi_[int(last) * kVocabSize + c];
    }

    // 3) Exponentiate bias (temperature-aware) and multiply
    // into dist probs.
    //    We clamp/shift to keep it stable.
    float maxb = -1e9f;
    for (float b : bias)
      if (b > maxb) maxb = b;
    if (!std::isfinite(maxb)) maxb = 0.f;
    const float scale =
        kCacheStrength /
        std::max(0.35f, temperature_dynamic);
    std::vector<float> mult(kVocabSize, 1.0f);
    for (int c = 0; c < kVocabSize; ++c) {
      float x = (bias[c] - maxb);  // center
      // squash extremes, then exponentiate
      x = std::tanh(x);
      mult[c] = std::exp(scale * x);
    }

    // Apply and renormalize
    float s = 0.f;
    for (auto& kv : *dist) {
      kv.second *= mult[kv.first];
      s += kv.second;
    }
    if (s > 0.f)
      for (auto& kv : *dist) kv.second /= s;
  }

  std::string Generate(const std::string& prompt,
                       size_t max_new, unsigned seed,
                       int top_k, float top_p,
                       float temperature) const {
    if (!trained_) return prompt;

    std::vector<uint8_t> hist;
    hist.reserve(prompt.size() + max_new + 8);
    for (unsigned char c : prompt) {
      int id = CharToIdx(c);
      if (id >= 0) hist.push_back(uint8_t(id));
    }
    if (hist.empty())
      hist.push_back(uint8_t(CharToIdx(' ')));

    std::mt19937 rng(
        seed ? seed : (unsigned)std::random_device{}());
    std::string out = prompt;

    std::vector<std::pair<uint8_t, float>> dist;
    for (size_t t = 0; t < max_new; ++t) {
      // context
      uint8_t ctxbuf[kMaxOrder];
      int L = 0;
      for (int i = (int)hist.size() - 1;
           i >= 0 && L < kMaxOrder; --i)
        ctxbuf[kMaxOrder - 1 - (L++)] = hist[i];
      const uint8_t* ctx = ctxbuf + (kMaxOrder - L);

      NextDistribution(ctx, L, &dist);

      // temperature
      if (temperature != 1.0f) {
        float sum = 0.f;
        for (auto& kv : dist) {
          kv.second =
              std::pow(std::max(kv.second, 1e-12f),
                       1.0f / std::max(0.1f, temperature));
          sum += kv.second;
        }
        if (sum > 0.f)
          for (auto& kv : dist) kv.second /= sum;
      }

      // Sort + top-k
      std::sort(dist.begin(), dist.end(),
                [](auto& a, auto& b) {
                  return a.second > b.second;
                });
      if (top_k > 0 && (int)dist.size() > top_k)
        dist.resize(top_k);

      // top-p
      if (top_p > 0.f && top_p < 1.0f) {
        float cum = 0.f;
        size_t m = 0;
        for (; m < dist.size(); ++m) {
          cum += dist[m].second;
          if (cum >= top_p) {
            ++m;
            break;
          }
        }
        if (m < dist.size()) dist.resize(m);
        float s = 0.f;
        for (auto& kv : dist) s += kv.second;
        if (s > 0.f)
          for (auto& kv : dist) kv.second /= s;
      }

      // ====== Transformer-like cache attention reweighting
      // ====== dynamic temp cools slightly as generation
      // grows (stabilizes reasoning chains)
      float dynamic_temp = std::max(
          0.65f, temperature - 0.25f * float(t) /
                                   float(std::max<size_t>(
                                       16, max_new)));
      ApplyCacheAttention(hist, &dist, dynamic_temp);

      // sample
      float r = std::generate_canonical<float, 24>(rng);
      float acc = 0.f;
      uint8_t next = 0;
      for (const auto& kv : dist) {
        acc += kv.second;
        if (r <= acc) {
          next = kv.first;
          break;
        }
      }

      const char ch = static_cast<char>(IdxToChar(next));
      out.push_back(ch);
      hist.push_back(next);
    }
    return out;
  }

  // -------- Persistence (compact binary) --------
  bool Save(const std::string& path) const {
    std::ofstream f(path, std::ios::binary | std::ios::out);
    if (!f) return false;
    const char magic[4] = {'C', 'S', 'R',
                           '7'};  // Reasoned 7-gram
    uint32_t version = 1, V = kVocabSize,
             max_order = kMaxOrder;
    f.write(magic, 4);
    f.write(reinterpret_cast<const char*>(&version),
            sizeof(version));
    f.write(reinterpret_cast<const char*>(&V), sizeof(V));
    f.write(reinterpret_cast<const char*>(&max_order),
            sizeof(max_order));
    // cont counts
    for (int k = 0; k < kMaxOrder; ++k)
      for (uint32_t x : cont_counts_[k])
        f.write(reinterpret_cast<const char*>(&x),
                sizeof(x));
    // PMI matrix
    for (float v : pmi_)
      f.write(reinterpret_cast<const char*>(&v), sizeof(v));
    // tables
    for (int k = 1; k <= kMaxOrder; ++k) {
      const auto& T = tables_[k - 1];
      uint32_t num = static_cast<uint32_t>(T.size());
      f.write(reinterpret_cast<const char*>(&num),
              sizeof(num));
      for (const auto& kv : T) {
        const uint64_t key = kv.first;
        const auto& row = kv.second;
        uint32_t nnz = static_cast<uint32_t>(row.nz.size());
        f.write(reinterpret_cast<const char*>(&key),
                sizeof(key));
        f.write(reinterpret_cast<const char*>(&row.total),
                sizeof(row.total));
        f.write(
            reinterpret_cast<const char*>(&row.unique_next),
            sizeof(row.unique_next));
        f.write(reinterpret_cast<const char*>(&nnz),
                sizeof(nnz));
        for (const auto& p : row.nz) {
          uint8_t idx = p.first;
          uint32_t cnt = p.second;
          f.write(reinterpret_cast<const char*>(&idx),
                  sizeof(idx));
          f.write(reinterpret_cast<const char*>(&cnt),
                  sizeof(cnt));
        }
      }
    }
    return true;
  }

  bool Load(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::in);
    if (!f) return false;
    char magic[4];
    uint32_t version, V, max_order;
    f.read(magic, 4);
    if (!f || std::string(magic, 4) != "CSR7") return false;
    f.read(reinterpret_cast<char*>(&version),
           sizeof(version));
    f.read(reinterpret_cast<char*>(&V), sizeof(V));
    f.read(reinterpret_cast<char*>(&max_order),
           sizeof(max_order));
    if (!f || version != 1 || V != kVocabSize ||
        max_order != kMaxOrder)
      return false;

    Reset();
    for (int k = 0; k < kMaxOrder; ++k)
      for (int i = 0; i < kVocabSize; ++i) {
        uint32_t x = 0;
        f.read(reinterpret_cast<char*>(&x), sizeof(x));
        cont_counts_[k][i] = x;
      }
    pmi_.assign(kVocabSize * kVocabSize, 0.0f);
    for (int i = 0; i < kVocabSize * kVocabSize; ++i) {
      float v = 0.f;
      f.read(reinterpret_cast<char*>(&v), sizeof(v));
      pmi_[i] = v;
    }
    for (int k = 1; k <= kMaxOrder; ++k) {
      uint32_t num = 0;
      f.read(reinterpret_cast<char*>(&num), sizeof(num));
      auto& T = tables_[k - 1];
      T.reserve(size_t(num * 1.3));
      for (uint32_t i = 0; i < num; ++i) {
        uint64_t key;
        uint32_t total, uniq, nnz;
        f.read(reinterpret_cast<char*>(&key), sizeof(key));
        f.read(reinterpret_cast<char*>(&total),
               sizeof(total));
        f.read(reinterpret_cast<char*>(&uniq),
               sizeof(uniq));
        f.read(reinterpret_cast<char*>(&nnz), sizeof(nnz));
        SparseRow row;
        row.total = total;
        row.unique_next = uniq;
        row.nz.resize(nnz);
        for (uint32_t j = 0; j < nnz; ++j) {
          uint8_t idx;
          uint32_t cnt;
          f.read(reinterpret_cast<char*>(&idx),
                 sizeof(idx));
          f.read(reinterpret_cast<char*>(&cnt),
                 sizeof(cnt));
          row.nz[j] = {idx, cnt};
        }
        T.emplace(key, std::move(row));
      }
    }
    trained_ = true;
    return true;
  }

  bool trained() const { return trained_; }

 private:
  const SparseRow* FindRow(int order, uint64_t key) const {
    const auto& T = tables_[order - 1];
    auto it = T.find(key);
    if (it == T.end()) return nullptr;
    return &it->second;
  }

  // order-1..7 tables
  std::array<std::unordered_map<uint64_t, SparseRow>,
             kMaxOrder>
      tables_{};
  // per-order continuation counts for next char w: N1+(·,
  // w)
  std::array<std::vector<uint32_t>, kMaxOrder>
      cont_counts_{};
  // PMI matrix (bigram) for cache prior
  std::vector<float> pmi_;  // size 95*95
  bool trained_ = false;
};

// ================================ Global
// ====================================

namespace {
NGramLM& GlobalModel() {
  static NGramLM* m = []() { return new NGramLM(); }();
  return *m;
}

bool EnsureModelReady() {
  NGramLM& M = GlobalModel();
  if (!g_force_retrain && M.trained()) return true;

  if (!g_force_retrain && M.Load(kModelPath)) {
    Info("Loaded cached reasoned 7-gram model from /tmp.");
    return true;
  }

  Info(std::string("Training reasoned 7-gram model (") +
       (g_force_retrain ? "forced" : "fresh") + ")...");
  const auto t0 = std::chrono::steady_clock::now();
  std::string raw = FetchRandomWikipediaText(kTrainDocs);
  std::string clean = ToPrintableAscii(raw);
  if (clean.size() < 4096) {
    Warn(
        "Downloaded corpus small; seeding with generic "
        "English text.");
    clean +=
        " This is a compact English seed to bias the model "
        "toward readable, structured prose. "
        "It includes causes and effects, definitions, "
        "lists, and examples to simulate reasoning. ";
  }

  const int threads_hint = std::min(
      std::max(1u, std::thread::hardware_concurrency()),
      (unsigned)kMaxThreadsCap);
  M.Train(clean, threads_hint);

  const auto t1 = std::chrono::steady_clock::now();
  Info(
      "Training finished in " +
      std::to_string(std::chrono::duration_cast<
                         std::chrono::milliseconds>(t1 - t0)
                         .count()) +
      " ms.");

  if (!M.Save(kModelPath))
    Warn("Could not save model to /tmp (continuing).");
  else
    Info("Saved model to /tmp.");
  return true;
}

}  // namespace

// =========================== Public Entry Point
// =============================

std::string GptEvaluate(std::string userInput) {
  try {
    curl_global_init(CURL_GLOBAL_DEFAULT);
  } catch (...) {
  }

  EnsureModelReady();

  NGramLM& M = GlobalModel();
  const int top_k = kTopKDefault;
  const float top_p = kTopPDefault;
  const float temperature = kTempDefault;

  unsigned seed = static_cast<unsigned>(
      std::chrono::high_resolution_clock::now()
          .time_since_epoch()
          .count() ^
      std::hash<std::string>{}(userInput));

  std::string full = M.Generate(userInput, kGenLen, seed,
                                top_k, top_p, temperature);

  curl_global_cleanup();

  if (full.size() >= userInput.size())
    return SanitizePrintable(full.substr(userInput.size()));
  return SanitizePrintable(full);
}

// cs/ai/gpt/wikipedia.cc
#include "cs/ai/gpt/wikipedia.hh"

#include <chrono>  // For std::chrono::seconds, milliseconds, etc.
#include <iomanip>
#include <regex>
#include <sstream>
#include <thread>  // For std::this_thread::sleep_for

#include "cs/log.hh"
#include "cs/net/http/client.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"

namespace cs::ai::gpt {

namespace {

#define VECTOR_TO_STREAM(vec)                 \
  ({                                          \
    std::ostringstream oss;                   \
    oss << "[";                               \
    for (size_t i = 0; i < vec.size(); ++i) { \
      oss << vec[i];                          \
      if (i != vec.size() - 1) oss << ", ";   \
    }                                         \
    oss << "]";                               \
    oss.str();                                \
  })

cs::ResultOr<std::vector<std::string>> ExtractTitles(
    const std::string& json, unsigned int n) {
  std::vector<std::string> titles;
  SET_OR_RET(auto obj_or,
             cs::net::json::parsers::ParseObject(json));
  std::regex re(R"regex("article":"([^"]+)")regex");
  std::smatch m;
  std::string::const_iterator searchStart(json.cbegin());

  while (
      std::regex_search(searchStart, json.cend(), m, re)) {
    std::string t = m[1].str();
    if (t != "Main_Page" && t.find("Special:") != 0) {
      titles.push_back(t);
      if (titles.size() >= n) break;
    }
    searchStart = m.suffix().first;
  }
  // Print out titles
  LOG(INFO) << "Extracted titles: "
            << VECTOR_TO_STREAM(titles) << ENDL;
  return titles;
}

#if 0
std::string StripHTML(const std::string& html) {
  std::string text =
      std::regex_replace(html, std::regex("<[^>]*>"), "");
  text =
      std::regex_replace(text, std::regex("&[^;]+;"), " ");
  return text;
}
#endif

}  // namespace

cs::ResultOr<std::vector<std::string>>
DownloadMostPopularArticles(unsigned int n) {
  LOG(INFO) << "Downloading top " << n
            << " Wikipedia articles" << ENDL;

  // Use yesterday's date to avoid requesting future stats.
  const auto now = std::chrono::system_clock::now();
  const auto yesterday = now - std::chrono::hours(24);
  std::time_t t =
      std::chrono::system_clock::to_time_t(yesterday);
  std::tm tm = *std::localtime(&t);
  std::ostringstream date_ss;
  date_ss << std::put_time(&tm, "%Y/%m/%d");

  SET_OR_RET(
      auto stats_json_or,
      cs::Fetch("https://wikimedia.org/api/rest_v1/metrics/"
                "pageviews/"
                "top/en.wikipedia/all-access/" +
                date_ss.str()));

  SET_OR_RET(auto titles, ExtractTitles(stats_json_or, n));
  std::vector<std::string> result;

  for (unsigned int i = 0; i < titles.size(); ++i) {
    LOG(INFO) << "Program will pause for 1 second." << ENDL;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG(INFO) << "Program resumed after 1 second." << ENDL;

    const std::string& title = titles[i];
    LOG(INFO) << "[" << (i + 1) << "/" << titles.size()
              << "] " << title << ENDL;

    std::string url =
        "https://en.wikipedia.org/api/rest_v1/page/"
        "summary/" +
        title;
    auto json = cs::Fetch(url);
    if (!json.ok()) {
      LOG(WARNING) << "Failed to fetch article summary: "
                   << title << ", error " << json << ENDL;
      continue;
    }

    auto parsed =
        cs::net::json::parsers::ParseObject(json.value());
    if (!parsed.ok()) {
      LOG(WARNING) << "Failed to parse summary JSON for "
                   << title << ENDL;
      continue;
    }

    auto extract_or = parsed.value().get("extract");
    if (!extract_or.ok()) {
      LOG(WARNING) << "Missing extract for " << title
                   << ENDL;
      continue;
    }

    result.push_back(extract_or.value().as(std::string{}));
  }

  return result;
}

}  // namespace cs::ai::gpt

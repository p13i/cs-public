/*
 * Deferred + Async extensions for cs::net::proto::db
 *
 * Requirements:
 *  - C++17 (uses std::variant/optional), optionally C++20
 * is fine.
 *  - Your T must provide:
 *      ResultOr<T> T().Parse(std::string_view)
 *      std::string T::Serialize(int indent) const
 *      std::string T::uuid
 */

#ifndef CS_NET_PROTO_DB_DB_DEFERRED_ASYNC_HH
#define CS_NET_PROTO_DB_DB_DEFERRED_ASYNC_HH

#include <future>
#include <mutex>
#include <optional>

namespace cs::net::proto::db {

template <typename T>
class DeferredQueryView : public QueryView<T> {
  using Ts = std::vector<T>;

  struct Step {
    enum class Kind { kFilter, kOrder, kLimit } kind;
    // Only one of these is used depending on kind.
    std::function<bool(const T&)> filter;
    std::function<bool(const T&, const T&)> comparator;
    size_t limit = std::numeric_limits<size_t>::max();
    std::string literal;
  };

 public:
  // A loader that returns the entire collection lazily.
  using Loader = std::function<cs::ResultOr<Ts>()>;

  DeferredQueryView() = default;
  explicit DeferredQueryView(
      Loader loader, std::string collection_hint = {})
      : loader_(std::move(loader)),
        collection_hint_(std::move(collection_hint)) {}

  // ---------- API, now deferred ----------
  DeferredQueryView<T> where(
      std::function<bool(const T&)> lambda,
      std::string literal) {
    this->steps.push_back(std::string("WHERE(") + literal +
                          ")");
    steps_.push_back(Step{
        .kind = Step::Kind::kFilter,
        .filter = std::move(lambda),
        .comparator = {},
        .limit = std::numeric_limits<size_t>::max(),
        .literal = std::move(literal),
    });
    return *this;
  }

  DeferredQueryView<T> order_by(
      std::function<bool(const T&, const T&)> comparator,
      std::string literal) {
    this->steps.push_back(std::string("ORDER_BY(") +
                          literal + ")");
    steps_.push_back(Step{
        .kind = Step::Kind::kOrder,
        .filter = {},
        .comparator = std::move(comparator),
        .limit = std::numeric_limits<size_t>::max(),
        .literal = std::move(literal),
    });
    return *this;
  }

  DeferredQueryView<T> limit(unsigned int n) {
    this->steps.push_back("LIMIT(" + std::to_string(n) +
                          ")");
    steps_.push_back(Step{
        .kind = Step::Kind::kLimit,
        .filter = {},
        .comparator = {},
        .limit = static_cast<size_t>(n),
        .literal = std::to_string(n),
    });
    return *this;
  }

  // Materialization gate. Safe to call multiple times; it
  // memoizes.
  cs::Result materialize() {
    if (materialized_) {
      LOG(DEBUG) << this->query_string()
                 << "  // (memoized)" << ENDL;
      return cs::Ok();
    }
    // 1) Load base values (possibly asynchronously).
    Ts values;
    {
      auto res = loader_ ? loader_()
                         : cs::ResultOr<Ts>(TRACE(cs::Error(
                               "No loader set for "
                               "DeferredQueryView.")));
      OK_OR_RET(res);
      values = std::move(res.value());
    }

    // 2) Apply recorded steps in sequence.
    for (const auto& s : steps_) {
      switch (s.kind) {
        case Step::Kind::kFilter: {
          Ts out;
          out.reserve(values.size());
          for (const auto& v : values)
            if (s.filter(v)) out.push_back(v);
          values.swap(out);
          break;
        }
        case Step::Kind::kOrder: {
          std::stable_sort(values.begin(), values.end(),
                           s.comparator);
          break;
        }
        case Step::Kind::kLimit: {
          if (values.size() > s.limit)
            values.resize(s.limit);
          break;
        }
      }
    }

    // 3) Store on the base class' cache and mark
    // materialized.
    this->_values = std::move(values);
    materialized_ = true;

    LOG(DEBUG) << "DeferredQueryView::materialize: "
               << this->query_string() << ENDL;
    return cs::Ok();
  }

  // Override to ensure deferred materialization engages.
  cs::ResultOr<Ts> values() {
    OK_OR_RET(this->materialize());
    return this->_values;
  }

  cs::ResultOr<T> first() {
    this->steps.push_back("FIRST");
    OK_OR_RET(this->materialize());
    if (this->_values.empty()) {
      return TRACE(cs::NotFoundError(
          "No values found. query_string=" +
          this->query_string()));
    }
    if (this->_values.size() > 1) {
      return TRACE(cs::Error(
          "More than one value found. query_string=" +
          this->query_string()));
    }
    return this->_values[0];
  }

  cs::ResultOr<bool> any() {
    this->steps.push_back("ANY");
    OK_OR_RET(this->materialize());
    return !this->_values.empty();
  }

  // Optional: let callers inspect the collection involved.
  const std::string& collection_hint() const {
    return collection_hint_;
  }

  Loader loader_;
  std::string collection_hint_;
  std::vector<Step> steps_;
  bool materialized_ = false;
};

// -----------------------------------------------------------------------------

template <typename T>
class AsyncJsonProtoDatabase : public JsonProtoDatabase<T> {
 public:
  AsyncJsonProtoDatabase() = default;
  AsyncJsonProtoDatabase(
      std::string data_dir, std::string collection_name,
      size_t max_concurrency =
          std::thread::hardware_concurrency())
      : JsonProtoDatabase<T>(std::move(data_dir),
                             std::move(collection_name)),
        max_concurrency_(
            max_concurrency == 0 ? 4 : max_concurrency) {}

  // Returns a DeferredQueryView whose loader:
  //  - lists all files within the collection directory,
  //  - concurrently reads & parses each file,
  //  - aggregates results (propagates first error
  //  encountered).
  cs::ResultOr<DeferredQueryView<T>> query_view() {
    using Ts = std::vector<T>;

    const std::string collections_dir = cs::fs::Join(
        this->data_dir_, this->collection_name_);

    // Capture by value to make the loader self-contained.
    auto loader =
        [collections_dir,
         maxc = max_concurrency_]() -> cs::ResultOr<Ts> {
      SET_OR_RET(auto files, cs::fs::ls(collections_dir));

      // Partition work into up to maxc lanes using
      // std::async.
      Ts out;
      out.reserve(files.size());

      std::atomic<bool> any_error{false};
      std::mutex out_mu;
      std::vector<std::future<cs::Result>> jobs;
      jobs.reserve(files.size());

      // Simple semaphore using a counter.
      std::atomic<size_t> in_flight{0};

      auto launch = [&](const std::string& file) {
        while (in_flight.load(std::memory_order_relaxed) >=
               maxc) {
          std::this_thread::yield();
        }
        in_flight.fetch_add(1, std::memory_order_relaxed);
        jobs.emplace_back(std::async(
            std::launch::async,
            [&, f = file]() -> cs::Result {
#if VLOG
              LOG(DEBUG) << "Reading JSON file for async "
                            "DB: loading f="
                         << f << ENDL;
#endif  // VLOG
              if (any_error.load(
                      std::memory_order_acquire)) {
                in_flight.fetch_sub(
                    1, std::memory_order_relaxed);
                return cs::Ok();  // Best-effort
                                  // short-circuit.
              }
              SET_OR_RET(std::string contents,
                         cs::fs::read(f));
#if VLOG
              LOG(DEBUG) << "Reading JSON file for async "
                            "DB: parsing contents.size()="
                         << contents.size() << ENDL;
#endif  // VLOG
              SET_OR_RET(T value, T().Parse(contents));
              {
#if VLOG
                LOG(DEBUG) << "Waiting for lock to push "
                              "value into out."
                           << ENDL;
#endif  // VLOG
                std::lock_guard<std::mutex> lock(out_mu);
#if VLOG
                LOG(DEBUG) << "Acquired lock, pushing "
                              "value into out."
                           << ENDL;
#endif  // VLOG
                out.push_back(std::move(value));
#if VLOG
                LOG(DEBUG) << "Pushed value into out, "
                              "releasing lock."
                           << ENDL;
#endif  // VLOG
              }
              in_flight.fetch_sub(
                  1, std::memory_order_relaxed);
#if VLOG
              LOG(DEBUG)
                  << "Finished processing file: " << f
                  << ENDL;
#endif  // VLOG
              return cs::Ok();
            }));
      };

      for (const auto& f : files) {
#if VLOG
        LOG(DEBUG) << "Scheduling async DB job for "
                      "file: "
                   << f << ENDL;
#endif  // VLOG
        launch(f);
      }

      // Await all jobs and propagate the first error.
#if VLOG
      size_t i = 0;
#endif  // VLOG
      for (auto& fut : jobs) {
#if VLOG
        LOG(DEBUG) << "Awaiting async DB job " << (i++)
                   << " of " << jobs.size() << ENDL;
#endif  // VLOG
        auto r = fut.get();
#if VLOG
        LOG(DEBUG) << "Async DB job finished with status: "
                   << r << ENDL;
#endif  // VLOG
        if (!r.ok() && !any_error.exchange(true)) {
#if VLOG
          LOG(DEBUG) << "Async DB job encountered error: "
                     << r << ENDL;
#endif               // VLOG
          return r;  // Propagate the first error
                     // encountered.
        }
#if VLOG
        LOG(DEBUG) << "Async DB job completed successfully."
                   << ENDL;
#endif  // VLOG
      }

#if VLOG
      LOG(DEBUG)
          << "All async DB jobs completed. out.size()="
          << out.size() << ENDL;
#endif  // VLOG
      return out;
    };

    return DeferredQueryView<T>(std::move(loader),
                                this->collection_name_);
  }

 private:
  size_t max_concurrency_;
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_DB_DEFERRED_ASYNC_HH

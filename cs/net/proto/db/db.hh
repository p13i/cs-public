#ifndef CS_NET_PROTO_DB_DB_HH
#define CS_NET_PROTO_DB_DB_HH

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"

#define STR_CONTAINS(haystack, needle)               \
  ([&]() {                                           \
    auto&& _h = (haystack);                          \
    auto&& _n = (needle);                            \
    if constexpr (std::is_pointer_v<                 \
                      std::decay_t<decltype(_h)>>) { \
      if (_h == nullptr) return false;               \
    }                                                \
    if constexpr (std::is_pointer_v<                 \
                      std::decay_t<decltype(_n)>>) { \
      if (_n == nullptr) return false;               \
    }                                                \
    std::string_view _hs(_h);                        \
    std::string_view _ns(_n);                        \
    return _hs.find(_ns) != std::string_view::npos;  \
  }())

#define LAMBDA(type, predicate) \
  [&](const auto& type) { return predicate; }, #predicate

#define EQUALS(a, b) (a == b)

#define DESCENDING(key)              \
  [](const auto& a, const auto& b) { \
    return a.key > b.key;            \
  },                                 \
      std::string("DESCENDING(") + #key + ")"

#define ASCENDING(key)                \
  ([](const auto& a, const auto& b) { \
    return a.key < b.key;             \
  }),                                 \
      std::string("ASCENDING(") + #key + ")"

namespace cs::net::proto::db {

template <typename T>
class QueryView {
  typedef std::vector<T> Ts;

 public:
  std::vector<std::string> steps;
  QueryView() = default;
  QueryView(const Ts& values) : _values(values) {}

  std::string query_string() {
    std::stringstream ss;
    ss << "QUERY ";
    bool first = true;
    for (const auto& step : steps) {
      if (!first) {
        ss << " ";
      }
      ss << step;
      first = false;
    }
    ss << ";";
    return ss.str();
  }

  ResultOr<Ts> values() {
    OK_OR_RET(this->materialize());
    return _values;
  }

  QueryView<T> where(std::function<bool(T)> lambda,
                     std::string literal) {
    steps.push_back(std::string("WHERE(") +
                    std::string(literal) +
                    std::string(")"));
    Ts matching;
    for (const auto& item : _values) {
      if (lambda(item)) {
        matching.push_back(item);
      }
    }
    _values = matching;
    return *this;
  }

  QueryView<T> limit(unsigned int n) {
    steps.push_back("LIMIT(" + n + std::string(")"));
    Ts limited;
    for (unsigned int i = 0; i < n && i < _values.size();
         i++) {
      limited.push_back(_values[i]);
    }
    _values = limited;
    return *this;
  }

  QueryView<T> order_by(
      std::function<bool(T, T)> comparator,
      std::string literal) {
    steps.push_back("ORDER_BY(" + std::string(literal) +
                    std::string(")"));
    Ts sorted(_values);
    std::sort(sorted.begin(), sorted.end(), comparator);
    _values = sorted;
    return *this;
  }

  cs::ResultOr<T> first() {
    steps.push_back("FIRST");
    OK_OR_RET(this->materialize());
    if (_values.size() == 0) {
      return TRACE(cs::NotFoundError(
          "No values found. query_string=" +
          query_string()));
    } else if (_values.size() > 1) {
      return TRACE(cs::Error(
          "More than one value found. query_string=" +
          query_string()));
    }
    return _values[0];
  }

  cs::Result materialize() {
    LOG(DEBUG) << this->query_string() << ENDL;
    return cs::Ok();
  }

  cs::ResultOr<bool> any() {
    steps.push_back("ANY");
    OK_OR_RET(this->materialize());
    return _values.size() > 0;
  }

  Ts _values;
};

class LocalJsonDatabase {
 public:
  LocalJsonDatabase() = default;
  LocalJsonDatabase(std::string data_abs_dir)
      : data_abs_dir_(data_abs_dir) {}

  ::cs::ResultOr<::cs::net::json::Object> get(
      std::string path);

  ::cs::Result set(std::string path,
                   ::cs::net::json::Object obj);

 private:
  std::string data_abs_dir_;
};

template <typename T>
class JsonProtoDatabase {
 public:
  std::string data_dir_;
  std::string collection_name_;
  JsonProtoDatabase() = default;
  JsonProtoDatabase(std::string data_dir,
                    std::string collection_name)
      : data_dir_(data_dir),
        collection_name_(collection_name) {}
  cs::ResultOr<QueryView<T>> query_view() {
    std::vector<T> values;
    std::string collections_dir =
        cs::fs::Join(data_dir_, collection_name_);
    SET_OR_RET(auto files, cs::fs::ls(collections_dir));
    for (auto& file : files) {
      SET_OR_RET(std::string contents, cs::fs::read(file));
      SET_OR_RET(T value, T().Parse(contents));
      values.push_back(value);
    }
    return QueryView<T>(values);
  }

  cs::Result Insert(T value) {
    if (!cs::fs::dir_exists(data_dir_)) {
      OK_OR_RET(cs::fs::mkdir(data_dir_));
    }
    std::string collections_dir =
        cs::fs::Join(data_dir_, collection_name_);
    if (!cs::fs::dir_exists(collections_dir)) {
      OK_OR_RET(cs::fs::mkdir(collections_dir));
    }
    std::string file_path =
        cs::fs::Join(collections_dir, value.uuid + ".json");
    return cs::fs::write(file_path,
                         value.Serialize(/*indent=*/4));
  }
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_DB_HH

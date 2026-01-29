// cs/net/json/json_dsl.gpt.hh
#ifndef CS_NET_JSON_JSON_DSL_GPT_HH
#define CS_NET_JSON_JSON_DSL_GPT_HH

#include <cmath>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/json/serialize.hh"

namespace cs::net::json::dsl {

using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::cs::net::json::parsers::ParseObject;

struct Case {
  std::string id;
  std::string input;
  Object expected;
  bool should_parse;
  bool check_roundtrip;
  std::string cite;
  bool xfail;
};

inline Object MakeObject(
    std::initializer_list<std::pair<std::string, Object>>
        kvs) {
  Object::KVMap map;
  for (const auto& [k, v] : kvs) {
    map[k] = v;
  }
  return Object(map);
}

inline double AsNumber(const Object& obj) {
  const auto& val = obj.value();
  if (const auto* i = std::get_if<int>(&val)) {
    return *i;
  }
  if (const auto* f = std::get_if<float>(&val)) {
    return *f;
  }
  return std::numeric_limits<double>::quiet_NaN();
}

inline bool JsonEq(const Object& lhs, const Object& rhs,
                   std::string* why) {
  const auto lhs_type = lhs.type();
  const auto rhs_type = rhs.type();
  if (lhs_type != rhs_type) {
    // LCOV_EXCL_START
    if (lhs_type == Type::kNumber &&
        rhs_type == Type::kNumber) {
      const double lnum = AsNumber(lhs);
      const double rnum = AsNumber(rhs);
      const bool same = std::fabs(lnum - rnum) <= 1e-6;
      if (!same && why) {
        *why = "numeric mismatch";
      }
      return same;
    }
    // LCOV_EXCL_STOP
    if (why) {
      *why = "type mismatch";
    }
    return false;
  }

  switch (lhs_type) {
    case Type::kNull:
      return true;
    case Type::kBoolean: {
      const bool same = lhs.as(bool{}) == rhs.as(bool{});
      if (!same && why) *why = "bool mismatch";
      return same;
    }
    case Type::kNumber: {
      const double lnum = AsNumber(lhs);
      const double rnum = AsNumber(rhs);
      const bool same = std::fabs(lnum - rnum) <= 1e-6;
      if (!same && why) *why = "numeric mismatch";
      return same;
    }
    case Type::kString: {
      const bool same =
          lhs.as(std::string{}) == rhs.as(std::string{});
      if (!same && why) *why = "string mismatch";
      return same;
    }
    case Type::kArray: {
      const auto* l_arr =
          std::get_if<Object::Array>(&lhs.value());
      const auto* r_arr =
          std::get_if<Object::Array>(&rhs.value());
      if (!l_arr || !r_arr) return false;
      if (l_arr->size() != r_arr->size()) {
        if (why) *why = "array size mismatch";
        return false;
      }
      for (size_t i = 0; i < l_arr->size(); ++i) {
        std::string inner;
        if (!JsonEq(l_arr->at(i), r_arr->at(i), &inner)) {
          if (why)
            *why = "array[" + std::to_string(i) +
                   "]: " + inner;
          return false;
        }
      }
      return true;
    }
    case Type::kMap: {
      const auto* l_map =
          std::get_if<Object::KVMap>(&lhs.value());
      const auto* r_map =
          std::get_if<Object::KVMap>(&rhs.value());
      if (!l_map || !r_map) return false;
      if (l_map->size() != r_map->size()) {
        if (why) *why = "map size mismatch";
        return false;
      }
      for (const auto& [key, l_val] : *l_map) {
        auto it = r_map->find(key);
        if (it == r_map->end()) {
          if (why) *why = "missing key " + key;
          return false;
        }
        std::string inner;
        if (!JsonEq(l_val, it->second, &inner)) {
          if (why) *why = "key " + key + ": " + inner;
          return false;
        }
      }
      return true;
    }
  }
  return false;  // LCOV_EXCL_LINE
}

inline std::vector<Case>& Registry() {
  static std::vector<Case> cases;
  return cases;
}

struct CaseRegistrar {
  explicit CaseRegistrar(Case c) {
    Registry().push_back(std::move(c));
  }
};

inline bool RunCase(const Case& c, std::ostream& os) {
  unsigned int cursor = 0;
  auto parsed = ParseObject(c.input, &cursor);
  const bool parsed_ok = parsed.ok();
  bool pass = true;
  std::string reason;

  if (c.should_parse) {
    if (!parsed_ok) {
      pass = false;
      reason = "parse failed: " + parsed.message();
    } else if (!JsonEq(parsed.value(), c.expected,
                       &reason)) {
      pass = false;
    } else if (c.check_roundtrip) {
      auto serialized = parsed.value().str();
      auto roundtrip = ParseObject(serialized);
      // LCOV_EXCL_START
      if (!roundtrip.ok()) {
        pass = false;
        reason = "roundtrip parse failed: " +
                 roundtrip.message();
      } else if (!JsonEq(parsed.value(), roundtrip.value(),
                         &reason)) {
        pass = false;
        reason = "roundtrip mismatch: " + reason +
                 " (serialized=" + serialized + ")";
      }
      // LCOV_EXCL_STOP
    }
  } else {
    if (parsed_ok) {
      pass = false;
      reason = "expected parse error, got success";
    }
  }

  const bool muted = c.xfail;
  if (!pass && muted) {
    os << "[XFAIL] " << c.id << " :: " << c.cite << " -- "
       << reason << "\n";
    return true;
  }
  // LCOV_EXCL_START
  if (!pass) {
    os << "[FAIL ] " << c.id << " :: " << c.cite << " -- "
       << reason << "\n";
    return false;
  }
  // LCOV_EXCL_STOP
  os << "[PASS ] " << c.id;
  if (!c.cite.empty()) {
    os << " :: " << c.cite;
  }
  if (muted) {
    os << " (xfail now passing)";
  }
  os << "\n";
  return true;
}

inline int RunAll(std::ostream& os) {
  int failures = 0;
  for (const auto& c : Registry()) {
    if (!RunCase(c, os)) {
      failures += 1;  // LCOV_EXCL_LINE
    }
  }
  os << "Total cases: " << Registry().size()
     << ", failures: " << failures << "\n";
  if (failures == 0) {
    return 0;
  }
  return 1;
}

}  // namespace cs::net::json::dsl

#define CS_JSON_CONCAT_INNER(a, b) a##b
#define CS_JSON_CONCAT(a, b) CS_JSON_CONCAT_INNER(a, b)

#define J_NULL() ::cs::net::json::Object::NewNull()
#define J_BOOL(v) ::cs::net::json::Object::NewBool(v)
#define J_INT(v) ::cs::net::json::Object::NewInt(v)
#define J_FLOAT(v) ::cs::net::json::Object::NewFloat(v)
#define J_STR(v) ::cs::net::json::Object::NewString(v)
#define J_ARR(...) \
  ::cs::net::json::Object::NewArray({__VA_ARGS__})
#define J_KV(key, value) \
  std::make_pair(std::string(key), value)
#define J_OBJ(...) \
  ::cs::net::json::dsl::MakeObject({__VA_ARGS__})

#define JSON_OK(ID, TEXT, EXPECT, CITE)      \
  static ::cs::net::json::dsl::CaseRegistrar \
      CS_JSON_CONCAT(_json_case_, ID)(       \
          {#ID, TEXT, EXPECT, true, true, CITE, false});

#define JSON_OK_XFAIL(ID, TEXT, EXPECT, CITE) \
  static ::cs::net::json::dsl::CaseRegistrar  \
      CS_JSON_CONCAT(_json_case_, ID)(        \
          {#ID, TEXT, EXPECT, true, true, CITE, true});

#define JSON_OK_NORT(ID, TEXT, EXPECT, CITE) \
  static ::cs::net::json::dsl::CaseRegistrar \
      CS_JSON_CONCAT(_json_case_, ID)(       \
          {#ID, TEXT, EXPECT, true, false, CITE, false});

#define JSON_FAIL(ID, TEXT, CITE)                     \
  static ::cs::net::json::dsl::CaseRegistrar          \
      CS_JSON_CONCAT(_json_case_,                     \
                     ID)({#ID, TEXT, J_NULL(), false, \
                          false, CITE, false});

#define JSON_FAIL_XFAIL(ID, TEXT, CITE)               \
  static ::cs::net::json::dsl::CaseRegistrar          \
      CS_JSON_CONCAT(_json_case_,                     \
                     ID)({#ID, TEXT, J_NULL(), false, \
                          false, CITE, true});

#endif  // CS_NET_JSON_JSON_DSL_GPT_HH

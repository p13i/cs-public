// cs/net/json/object.hh
#ifndef CS_NET_JSON_OBJECT_HH
#define CS_NET_JSON_OBJECT_HH

#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

namespace cs::net::json {

class Object;

// JSON serialization algorithm (defined elsewhere).
std::ostream& SerializeObject(std::ostream& os,
                              const Object& object);

enum class Type : unsigned int {
  kNull = 0,
  kBoolean = 1,
  kNumber = 2,
  kString = 3,
  kArray = 4,
  kMap = 5,
};

class Object {
 public:
  using KVMap = std::map<std::string, Object>;
  using Array = std::vector<Object>;

  // Single storage for all JSON values.
  using Value = std::variant<std::monostate,  // null
                             bool, int, float, std::string,
                             Array, KVMap>;

  // Constructors
  Object() : _value(std::monostate{}) {}
  Object(std::nullptr_t) : _value(std::monostate{}) {}
  Object(bool v) : _value(v) {}
  Object(int v) : _value(v) {}
  Object(float v) : _value(v) {}
  Object(const char* s) : _value(std::string(s)) {}
  Object(std::string v) : _value(std::move(v)) {}
  Object(const Array& v) : _value(v) {}
  Object(Array&& v) : _value(std::move(v)) {}
  Object(const KVMap& v) : _value(v) {}
  Object(KVMap&& v) : _value(std::move(v)) {}

  // Helper factories (preserved)
  static Object NewMap() { return Object(KVMap{}); }
  static Object NewArray(std::vector<Object> elems) {
    return Object(std::move(elems));
  }
  static Object NewNull() { return Object(); }
  static Object NewMap(KVMap map) {
    return Object(std::move(map));
  }
  static Object NewString(std::string str) {
    return Object(std::move(str));
  }
  static Object NewFloat(float f) { return Object(f); }
  static Object NewInt(int i) { return Object(i); }
  static Object NewBool(bool b) { return Object(b); }

  // Type query
  Type type() const {
    if (std::holds_alternative<std::monostate>(_value))
      return Type::kNull;
    if (std::holds_alternative<bool>(_value))
      return Type::kBoolean;
    if (std::holds_alternative<int>(_value) ||
        std::holds_alternative<float>(_value))
      return Type::kNumber;
    if (std::holds_alternative<std::string>(_value))
      return Type::kString;
    if (std::holds_alternative<Array>(_value))
      return Type::kArray;
    return Type::kMap;
  }

  std::string type_string() const {
    switch (type()) {
      case Type::kNull:
        return "null";
      case Type::kBoolean:
        return "boolean";
      case Type::kNumber:
        return "float";
      case Type::kString:
        return "string";
      case Type::kArray:
        return "array";
      case Type::kMap:
        return "map";
      default:
        return "unknown";
    }
  }

  float as(float /*instance*/) const {
    if (std::holds_alternative<float>(_value)) {
      return std::get<float>(_value);
    }
    if (std::holds_alternative<int>(_value)) {
      return static_cast<float>(std::get<int>(_value));
    }
    return NAN;
  }

  int as(int /*instance*/) const {
    if (std::holds_alternative<int>(_value)) {
      return std::get<int>(_value);
    }
    return 0;
  }

  bool as(bool /*instance*/) const {
    if (std::holds_alternative<bool>(_value)) {
      return std::get<bool>(_value);
    }
    return false;
  }

  std::string as(std::string /*instance*/) const {
    if (const auto* p = std::get_if<std::string>(&_value)) {
      return *p;
    }
    return "";
  }

  std::string as(const char* /*instance*/) const {
    if (const auto* p = std::get_if<std::string>(&_value)) {
      return *p;
    }
    return "";
  }

  KVMap as(KVMap /*instance*/) const {
    if (const auto* p = std::get_if<KVMap>(&_value)) {
      return *p;
    }
    return {};
  }

  std::vector<Object> as(
      std::vector<Object> /*instance*/) const {
    if (const auto* p = std::get_if<Array>(&_value)) {
      return *p;
    }
    return {};
  }

  template <typename T>
  bool is(T instance) const {
    return std::holds_alternative<T>(_value);
  }

  // Index/key helpers
  bool has_key(const std::string& key) const {
    const auto* m = std::get_if<KVMap>(&_value);
    return m && m->count(key) != 0;
  }

  bool has_index(int index) const {
    const auto* a = std::get_if<Array>(&_value);
    if (!a) return false;
    return 0 <= index &&
           index < static_cast<int>(a->size());
  }

  template <typename T>
  cs::ResultOr<T> get(const std::string& key,
                      T instance) const {
    return get(key).value_or(instance).as(instance);
  }

  cs::ResultOr<Object> get(const std::string& key) const {
    const auto* m = std::get_if<KVMap>(&_value);
    if (!m) {
      return TRACE(cs::Error("Object is not a map."));
    }
    auto it = m->find(key);
    if (it == m->end()) {
      return TRACE(cs::Error("Key not found: " + key +
                             " in " + this->str()));
    }
    return it->second;
  }

  cs::ResultOr<Object> get(int index) const {
    const auto* a = std::get_if<Array>(&_value);
    if (!a) {
      return TRACE(
          cs::Error("indexing into non-array type"));
    }
    if (!has_index(index)) {
      return cs::InvalidArgument(
          FMT("index=%d out of bounds.", index));
    }
    return (*a)[static_cast<size_t>(index)];
  }

  cs::ResultOr<Object> operator[](const std::string& key) {
    return get(key);
  }
  cs::ResultOr<Object> operator[](int index) {
    return get(index);
  }

  cs::Result set(unsigned int index, Object value) {
    auto* a = std::get_if<Array>(&_value);
    if (!a) {
      return TRACE(cs::Error("Object is not an array."));
    }
    if (index < a->size()) {
      (*a)[index] = std::move(value);
      return cs::Ok();
    }
    return TRACE(cs::InvalidArgument(
        "index out of range: " + std::to_string(index) +
        ", size=" + std::to_string(a->size())));
  }

  cs::Result set(std::string key, Object value) {
    auto* m = std::get_if<KVMap>(&_value);
    if (!m) {
      return TRACE(cs::Error("Object is not a map."));
    }
    (*m)[std::move(key)] = std::move(value);
    return cs::Ok();
  }

  // Stringify via serializer
  std::string str() const {
    std::stringstream ss;
    SerializeObject(ss, *this);
    return ss.str();
  }

  bool operator==(const Object& other) const {
    return _value == other._value;
  }

  bool operator!=(const Object& other) const {
    return !(*this == other);
  }

  const Value& value() const { return _value; }

 private:
  Value _value;
};

}  // namespace cs::net::json

#endif  // CS_NET_JSON_OBJECT_HH

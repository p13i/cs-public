#include "cs/net/json/serialize.hh"

#include <cstdint>
#include <iostream>
#include <sstream>

#include "cs/net/json/object.hh"

namespace cs::net::json {

std::ostream& operator<<(std::ostream& os,
                         const Object& object) {
  return SerializeObject(os, object);
}

namespace {
std::ostream& WriteIndent(std::ostream& os,
                          unsigned int indent) {
  for (unsigned int i = 0; i < indent; ++i) {
    os << " ";
  }
  return os;
}
}  // namespace

std::ostream& SerializeObject(std::ostream& os,
                              const Object& object) {
  return SerializeObjectPrettyPrintRecurse(
      os, object, /*indent=*/0,
      /*initial_indent=*/0);
}

std::string SerializeObject(const Object& object,
                            unsigned int indent = 0) {
  std::stringstream ss;
  SerializeObjectPrettyPrintRecurse(ss, object, indent,
                                    /*initial_indent=*/0);
  return ss.str();
}

std::ostream& SerializeObjectPrettyPrintRecurse(
    std::ostream& os, const Object& object,
    unsigned int indent, unsigned int initial_indent) {
  if (object.is_null()) {
    os << "null";
  } else if (object.type() == Type::kBoolean) {
    if (object.as_bool()) {
      os << "true";
    } else {
      os << "false";
    }
  } else if (object.type() == Type::kNumber) {
    if (object.is(int())) {
      os << object.as(int());
    } else if (object.is(float())) {
      os << object.as(float());
    } else {
      // Should not reach this code.
      os << "\"INVARIANT_ERROR\"";
    }
  } else if (object.type() == Type::kString) {
    auto s = object.as_string();
    os << '"';
    for (char c : s) {
      if (c == '\\' || c == '\n' || c == '\r' ||
          c == '\"') {
        os << "\\";
        if (c == '\\') {
          os << '\\';
        } else if (c == '\n') {
          os << 'n';
        } else if (c == '\r') {
          os << 'r';
        } else if (c == '\"') {
          os << '"';
        }
      } else {
        os << c;
      }
    }
    os << '"';
  } else if (object.type() == Type::kArray) {
    os << "[";
    if (indent > 0) {
      os << "\n";
    }
    bool first = true;
    for (const auto& elem : object.as_array()) {
      if (!first) {
        os << ",";
        if (indent > 0) {
          os << "\n";
        }
      } else {
        first = false;
      }
      WriteIndent(os, initial_indent + indent);
      SerializeObjectPrettyPrintRecurse(
          os, elem, indent, initial_indent + indent);
    }
    if (indent > 0) {
      os << "\n";
    }
    WriteIndent(os, initial_indent);
    os << "]";
  } else if (object.type() == Type::kMap) {
    os << "{";
    if (indent > 0) {
      os << "\n";
    }
    bool first = true;
    for (const auto& kv : object.as_map()) {
      if (!first) {
        os << ",";
        if (indent > 0) {
          os << "\n";
        }
      } else {
        first = false;
      }
      WriteIndent(os, initial_indent + indent);
      os << '"' << kv.first << '"';
      os << ":";
      if (indent > 0) {
        os << " ";
      }
      SerializeObjectPrettyPrintRecurse(
          os, kv.second, indent, initial_indent + indent);
    }
    if (indent > 0) {
      os << "\n";
    }
    WriteIndent(os, initial_indent);
    os << "}";
  }
  return os;
};

}  // namespace cs::net::json
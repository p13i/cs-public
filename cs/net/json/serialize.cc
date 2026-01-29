// cs/net/json/serialize.cc
#include "cs/net/json/serialize.hh"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
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
  if (object.type() == Type::kNull) {
    os << "null";
  } else if (object.type() == Type::kBoolean) {
    if (object.as(bool())) {
      os << "true";
    } else {
      os << "false";
    }
  } else if (object.type() == Type::kNumber) {
    if (object.is(int())) {
      os << object.as(int());
    } else if (object.is(float())) {
      auto old_precision = os.precision();
      os << std::setprecision(
                std::numeric_limits<float>::digits10 + 1)
         << object.as(float());
      os.precision(old_precision);
    } else {
      // Should not reach this code.
      os << "\"INVARIANT_ERROR\"";  // LCOV_EXCL_LINE
    }
  } else if (object.type() == Type::kString) {
    std::string s = object.as(std::string());
    os << '"';
    for (unsigned char c : s) {
      switch (c) {
        case '\\':
          os << "\\\\";
          break;
        case '"':
          os << "\\\"";
          break;
        case '\b':
          os << "\\b";
          break;
        case '\f':
          os << "\\f";
          break;
        case '\n':
          os << "\\n";
          break;
        case '\r':
          os << "\\r";
          break;
        case '\t':
          os << "\\t";
          break;
        default:
          if (c < 0x20) {
            os << "\\u" << std::hex << std::setw(4)
               << std::setfill('0') << static_cast<int>(c)
               << std::dec << std::setfill(' ');
          } else {
            os << static_cast<char>(c);
          }
      }
    }
    os << '"';
  } else if (object.type() == Type::kArray) {
    os << "[";
    if (indent > 0) {
      os << "\n";
    }
    bool first = true;
    for (const auto& elem :
         object.as(std::vector<Object>())) {
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
    for (const auto& kv :
         object.as(std::map<std::string, Object>())) {
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

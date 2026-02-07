// cs/net/json/parsers.cc
#include "cs/net/json/parsers.hh"

#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

#define PROFILING true
#if PROFILING
#include "cs/util/timeit.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::parsers::CheckInBounds;
using ::cs::parsers::InBounds;
using ::cs::parsers::IncrementCursor;
using ::cs::parsers::MaybeConsumeWhitespace;
using ::cs::parsers::ParseFloat;
using ::cs::parsers::ParseInt;
using ::cs::parsers::TryConsumeString;
using ::cs::util::fmt;
}  // namespace

#endif  // PROFILING

namespace {

// Helper to decode a single hex digit. Returns -1 on error.
int HexToInt(char c) {
  if ('0' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'f') return 10 + (c - 'a');
  if ('A' <= c && c <= 'F') return 10 + (c - 'A');
  return -1;
}

// Encode a Unicode code point as UTF-8.
void AppendCodepoint(uint32_t codepoint, std::string* out) {
  if (codepoint <= 0x7F) {
    out->push_back(static_cast<char>(codepoint));
  } else if (codepoint <= 0x7FF) {
    out->push_back(
        static_cast<char>(0xC0 | (codepoint >> 6)));
    out->push_back(
        static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint <= 0xFFFF) {
    out->push_back(
        static_cast<char>(0xE0 | (codepoint >> 12)));
    out->push_back(static_cast<char>(
        0x80 | ((codepoint >> 6) & 0x3F)));
    out->push_back(
        static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else {
    out->push_back(static_cast<char>(
        0xF0 | ((codepoint >> 18) & 0x07)));
    out->push_back(static_cast<char>(
        0x80 | ((codepoint >> 12) & 0x3F)));
    out->push_back(static_cast<char>(
        0x80 | ((codepoint >> 6) & 0x3F)));
    out->push_back(
        static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}
}  // namespace

namespace cs::net::json::parsers {

// Very verbose logging
#define VVLOG 0

namespace {

[[maybe_unused]]
ResultOr<std::variant<float, int>> ParseJsonNumberStrict(
    std::string str, unsigned int* cursor) {
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  if (!InBounds(str, *cursor)) {
    return TRACE(  // LCOV_EXCL_LINE
        Error("Expected digit, found end of input."));
  }

  unsigned int start = *cursor;
  if (str[*cursor] == '+') {
    return TRACE(  // LCOV_EXCL_LINE
        Error("Leading '+' not allowed in JSON numbers."));
  }

  if (str[*cursor] == '-') {
    OK_OR_RET(IncrementCursor(str, cursor));
    if (!InBounds(str, *cursor) ||
        !(std::isdigit(str[*cursor]) ||
          str[*cursor] == '.')) {
      return TRACE(Error("Expected digit after '-'."));
    }
  }

  bool is_float = false;
  bool has_exponent = false;
  bool started_with_dot = false;
  if (str[*cursor] == '0') {
    OK_OR_RET(IncrementCursor(str, cursor));
    if (InBounds(str, *cursor) &&
        std::isdigit(str[*cursor])) {
      return TRACE(
          Error("Leading zeros are not allowed in JSON."));
    }
  } else if ('1' <= str[*cursor] && str[*cursor] <= '9') {
    while (InBounds(str, *cursor) &&
           std::isdigit(str[*cursor])) {
      OK_OR_RET(IncrementCursor(str, cursor));
    }
  } else if (str[*cursor] == '.') {
    started_with_dot = true;
  } else {
    return TRACE(Error(
        "Expected digit in number."));  // LCOV_EXCL_LINE
  }

  if (InBounds(str, *cursor) && str[*cursor] == '.') {
    is_float = true;
    OK_OR_RET(IncrementCursor(str, cursor));
    if (!InBounds(str, *cursor) ||
        !std::isdigit(str[*cursor])) {
      return TRACE(
          Error("Digits required after decimal point."));
    }
    while (InBounds(str, *cursor) &&
           std::isdigit(str[*cursor])) {
      OK_OR_RET(IncrementCursor(str, cursor));
    }
  }

  if (InBounds(str, *cursor) &&
      (str[*cursor] == 'e' || str[*cursor] == 'E')) {
    has_exponent = true;
    is_float = true;
    OK_OR_RET(IncrementCursor(str, cursor));
    if (InBounds(str, *cursor) &&
        (str[*cursor] == '+' || str[*cursor] == '-')) {
      OK_OR_RET(IncrementCursor(str, cursor));
    }
    if (!InBounds(str, *cursor) ||
        !std::isdigit(str[*cursor])) {
      return TRACE(
          Error("Digits required after exponent."));
    }
    while (InBounds(str, *cursor) &&
           std::isdigit(str[*cursor])) {
      OK_OR_RET(IncrementCursor(str, cursor));
    }
  }

  if (started_with_dot && !has_exponent) {
    return TRACE(
        Error("Digit required before decimal point."));
  }

  std::string number_text =
      str.substr(start, *cursor - start);
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  const char* begin = number_text.c_str();
  char* endptr = nullptr;
  if (is_float) {
    errno = 0;
    float value = std::strtof(begin, &endptr);
    if (endptr != begin + number_text.size()) {
      return TRACE(Error(fmt("Failed to parse float '%s'",
                             number_text.c_str())));
    }
    // Allow underflow (ERANGE with finite value), but
    // reject overflow (infinity)
    if (errno == ERANGE && std::isinf(value)) {
      return TRACE(Error(fmt("Failed to parse float '%s'",
                             number_text.c_str())));
    }
    return std::variant<float, int>(value);
  }

  errno = 0;
  long value = std::strtol(begin, &endptr, 10);
  if (endptr != begin + number_text.size() ||
      errno == ERANGE) {
    return TRACE(Error(fmt("Failed to parse int '%s'",
                           number_text.c_str())));
  }
  return std::variant<float, int>(static_cast<int>(value));
}

ResultOr<std::string> ParseJsonStringStrict(
    std::string str, unsigned int* cursor) {
  OK_OR_RET(CheckInBounds(str, *cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  if (str[*cursor] != '"') {
    return TRACE(Error(
        "Did not find leading '\"' in string at cursor=" +
        std::to_string(*cursor) + ": " + str));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  std::string result;
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];

    if (c == '\\') {
      OK_OR_RET(IncrementCursor(str, cursor));
      OK_OR_RET(CheckInBounds(str, *cursor));
      char esc = str[*cursor];
      switch (esc) {
        case '\\': {
          result.push_back('\\');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case '"': {
          result.push_back('"');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case '/': {
          result.push_back('/');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 'b': {
          result.push_back('\b');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 'f': {
          result.push_back('\f');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 'n': {
          result.push_back('\n');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 'r': {
          result.push_back('\r');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 't': {
          result.push_back('\t');
          OK_OR_RET(IncrementCursor(str, cursor));
          break;
        }
        case 'u': {
          OK_OR_RET(IncrementCursor(str, cursor));
          uint32_t code_unit = 0;
          for (int i = 0; i < 4; ++i) {
            OK_OR_RET(CheckInBounds(str, *cursor));
            int hex = HexToInt(str[*cursor]);
            if (hex < 0) {
              return TRACE(Error(
                  "Invalid hex digit in \\u escape."));
            }
            code_unit = (code_unit << 4) |
                        static_cast<uint32_t>(hex);
            OK_OR_RET(IncrementCursor(str, cursor));
          }

          uint32_t codepoint = code_unit;
          if (0xD800 <= code_unit && code_unit <= 0xDBFF) {
            // High surrogate, expect a following low
            // surrogate.
            OK_OR_RET(CheckInBounds(str, *cursor));
            if (str[*cursor] != '\\' ||
                !InBounds(str, *cursor + 1) ||
                str[*cursor + 1] != 'u') {
              return TRACE(
                  Error("Invalid unicode surrogate pair."));
            }
            OK_OR_RET(IncrementCursor(str, cursor));
            OK_OR_RET(IncrementCursor(str, cursor));
            uint32_t low = 0;
            for (int i = 0; i < 4; ++i) {
              OK_OR_RET(CheckInBounds(str, *cursor));
              int hex = HexToInt(str[*cursor]);
              if (hex < 0) {
                return TRACE(Error(
                    "Invalid hex digit in \\u escape."));
              }
              low = (low << 4) | static_cast<uint32_t>(hex);
              OK_OR_RET(IncrementCursor(str, cursor));
            }
            if (low < 0xDC00 || low > 0xDFFF) {
              return TRACE(
                  Error("Invalid unicode surrogate pair."));
            }
            codepoint = 0x10000 +
                        ((code_unit - 0xD800) << 10) +
                        (low - 0xDC00);
          } else if (0xDC00 <= code_unit &&
                     code_unit <= 0xDFFF) {
            return TRACE(Error(
                "Unexpected low surrogate without lead."));
          }

          AppendCodepoint(codepoint, &result);
          continue;
        }
        default:
          return TRACE(Error(
              "Did not find a valid escape sequence."));
      }
      continue;
    }

    if (c == '"') {
      break;
    }
    if (static_cast<unsigned char>(c) < 0x20) {
      return TRACE(
          Error("Unescaped control character in string."));
    }
    result.push_back(c);
    OK_OR_RET(IncrementCursor(str, cursor));
  }
  if (!InBounds(str, *cursor) || str[*cursor] != '"') {
    return TRACE(
        Error("Didn't find ending '\"' character."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  return result;
}

}  // namespace

Result operator>>(std::string str, Object* object) {
  unsigned int cursor = 0;
  SET_OR_RET(*object, ParseObject(str, &cursor));
  return Ok();
}

ResultOr<bool> ParseBoolean(std::string str,
                            unsigned int* cursor) {
#if VVLOG
  LOG(DEBUG) << "ParseBoolean(str, *cursor=" << *cursor
             << ")" << ENDL;
#endif  // VVLOG
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  if (TryConsumeString(str, "true", cursor)) {
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    return true;
  }

  if (TryConsumeString(str, "false", cursor)) {
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    return false;
  }

  return TRACE(Error("Didn't find boolean at *cursor=" +
                     std::to_string(*cursor) + "."));
}

ResultOr<std::variant<float, int>> ParseNumber(
    std::string str) {
  unsigned int cursor = 0;
  return ParseNumber(str, &cursor);
}

ResultOr<std::variant<float, int>> ParseNumber(
    std::string str, unsigned int* cursor) {
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  unsigned int cursor_copy = *cursor;
  ResultOr<int> int_result = ParseInt(str, &cursor_copy);
  if (int_result.ok()) {
    if (!InBounds(str, cursor_copy) ||
        (str[cursor_copy] != '.' &&
         str[cursor_copy] != 'e' &&
         str[cursor_copy] != 'E')) {
      *cursor = cursor_copy;
      return std::variant<float, int>(int_result.value());
    }
  }

  SET_OR_RET(float result, ParseFloat(str, cursor));
  return std::variant<float, int>(result);
}

ResultOr<std::string> ParseString(std::string str) {
  unsigned int cursor = 0;
  return ParseString(str, &cursor);
}

ResultOr<std::string> ParseString(std::string str,
                                  unsigned int* cursor) {
#if VVLOG
  LOG(DEBUG) << "ParseString(str, *cursor=" << *cursor
             << ")" << ENDL;
#endif  // VVLOG
  OK_OR_RET(CheckInBounds(str, *cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  if (str[*cursor] != '"') {
    return TRACE(Error(
        "Did not find leading '\"' in string at cursor=" +
        std::to_string(*cursor) + ": " + str));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  std::stringstream ss;
  bool reading_escaped = false;
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];

    if (reading_escaped) {
      if (c == '\\' || c == '\n' || c == '\r' ||
          c == '\"') {
        ss << c;
      }
      reading_escaped = false;
    } else if (c == '\\') {
      reading_escaped = true;
    } else if (str[*cursor] == '"') {
      break;
    } else {
      ss << c;
    }

    OK_OR_RET(IncrementCursor(str, cursor));
  }
  if (str[*cursor] != '"') {
    return TRACE(
        Error("Didn't find ending '\"' character."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  return ss.str();
}

ResultOr<std::vector<Object>> ParseArray(
    std::string str, unsigned int* cursor) {
#if VVLOG
  LOG(DEBUG) << "ParseArray(str, *cursor=" << *cursor << ")"
             << ENDL;
#endif  // VVLOG
  OK_OR_RET(CheckInBounds(str, *cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  if (str[*cursor] != '[') {
    return TRACE(
        Error("Did not find leading '[' in array."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  std::vector<Object> array;
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  if (!InBounds(str, *cursor)) {
    return TRACE(
        Error("Did not find ending ']' in array."));
  }
  if (str[*cursor] == ']') {
    OK_OR_RET(IncrementCursor(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    return array;
  }

  while (true) {
    Object object;
    SET_OR_RET(object, ParseObject(str, cursor));
    array.push_back(object);
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (!InBounds(str, *cursor)) {
      return TRACE(
          Error("Did not find ending ']' in array."));
    }
    char c = str[*cursor];
    if (c == ',') {
      OK_OR_RET(IncrementCursor(str, cursor));
      OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
      if (!InBounds(str, *cursor) || str[*cursor] == ']') {
        return TRACE(
            Error("Trailing comma found in array."));
      }
      continue;
    }
    if (c == ']') {
      OK_OR_RET(IncrementCursor(str, cursor));
      OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
      return array;
    }
    return TRACE(
        Error("Expected ',' or ']' when parsing array."));
  }
}

ResultOr<std::map<std::string, Object>> ParseMap(
    std::string str, unsigned int* cursor) {
#if VVLOG
  LOG(DEBUG) << "ParseMap(str, *cursor=" << *cursor << ")"
             << ENDL;
#endif  // VVLOG
  OK_OR_RET(CheckInBounds(str, *cursor));

  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  if (str[*cursor] != '{') {
    return TRACE(Error("Did not find leading '{' in map."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  std::map<std::string, Object> map;
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  if (!InBounds(str, *cursor)) {
    return TRACE(Error("Did not find ending '}' in map."));
  }
  if (str[*cursor] == '}') {
    OK_OR_RET(IncrementCursor(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    return map;
  }

  while (true) {
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    std::string key;
    SET_OR_RET(key, ParseJsonStringStrict(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (!InBounds(str, *cursor) || str[*cursor] != ':') {
      return TRACE(
          Error("Did not find ':' after key in map."));
    }
    OK_OR_RET(IncrementCursor(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    Object object;
    SET_OR_RET(object, ParseObject(str, cursor));
    map[key] = object;
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (!InBounds(str, *cursor)) {
      return TRACE(
          Error("Did not find ending '}' in map."));
    }
    char c = str[*cursor];
    if (c == ',') {
      OK_OR_RET(IncrementCursor(str, cursor));
      OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
      if (!InBounds(str, *cursor) || str[*cursor] == '}') {
        return TRACE(Error("Trailing comma found in map."));
      }
      continue;
    }
    if (c == '}') {
      OK_OR_RET(IncrementCursor(str, cursor));
      OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
      return map;
    }
    return TRACE(
        Error("Expected ',' or '}' when parsing map."));
  }
}

ResultOr<Object> ParseObject(std::string str) {
#if PROFILING
  ResultOr<Object> result = Error("Result not set.");
  // TIME_IT_START
  unsigned int cursor = 0;
  result = ParseObject(str, &cursor);
  // TIME_IT_END("Parsing took ");
  return result;
#else
  unsigned int cursor = 0;
  return ParseObject(str, &cursor);
#endif  // #if PROFILING
}  // LCOV_EXCL_LINE

ResultOr<Object> ParseObject(std::string str,
                             unsigned int* cursor) {
#if VVLOG
  LOG(DEBUG) << "ParseObject(str, *cursor=" << *cursor
             << ")" << ENDL;
#endif  // VVLOG

  OK_OR_RET(CheckInBounds(str, *cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  Object object = Object();
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];
    if (c == '{') {
      // Parse map
      SET_OR_RET(auto map, ParseMap(str, cursor));
      object = Object(map);
      break;
    } else if (c == '[') {
      // Parse array
      SET_OR_RET(auto array, ParseArray(str, cursor));
      object = Object(array);
      break;
    } else if (c == '-' || ('0' <= c && c <= '9')) {
      // Parse number (strict JSON rules)
      const auto& n = ParseJsonNumberStrict(str, cursor);
      if (!n.ok()) {
        return n;
      }
      if (std::holds_alternative<int>(n.value())) {
        object = Object(std::get<int>(n.value()));
      } else if (std::holds_alternative<float>(n.value())) {
        object = Object(std::get<float>(n.value()));
      }
      break;
    } else if (c == '\"') {
      // Parse string (strict JSON rules)
      SET_OR_RET(auto parsed_str,
                 ParseJsonStringStrict(str, cursor));
      object = Object(parsed_str);
      break;
    } else if (c == 't' || c == 'f') {
      // Parse bool
      SET_OR_RET(auto b, ParseBoolean(str, cursor));
      object = Object(b);
      break;
    } else if (c == 'n') {
      if (!TryConsumeString(str, "null", cursor)) {
        return TRACE(Error(
            fmt("Reached unexpected character ('%c') at "
                "cursor=%d in str=" +
                    str,
                c, *cursor)));
      }
      OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
      object = Object();  // null
      break;
    } else {
      return TRACE(Error(
          fmt("Reached unexpected character ('%c') at "
              "cursor=%d in str=" +
                  str,
              c, *cursor)));
    }
  }

  return object;
}
}  // namespace cs::net::json::parsers

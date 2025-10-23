#include "cs/net/json/parsers.hh"

#include <cmath>
#include <cstdint>
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
#endif  // PROFILING

namespace {
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

namespace cs::net::json::parsers {

// Very verbose logging
#define VVLOG 0

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
  // LOG(DEBUG) << "ParseNumber(str=" << str << ", *cursor="
  // << *cursor
  //            << ")"  <<  ENDL;
  unsigned int cursor_copy = *cursor;
  ResultOr<int> int_result = ParseInt(str, &cursor_copy);
  // LOG(DEBUG) << "ParseNumber: int_result=" << int_result
  //            << ", cursor_copy=" << cursor_copy
  //             <<  ENDL;
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
  unsigned int cursor;
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
#if 0
while (InBounds(str, *cursor)) {
  if (str[*cursor] == '\\') {
    OK_OR_RET(IncrementCursor(str, cursor));
    char c = str[*cursor];
    switch (c) {
      case '\\':
        ss << '\\';
        break;
      case 'n':
        ss << '\n';
        break;
      case 'r':
        ss << '\r';
        break;
      default:
        return TRACE(
            Error("Did not find a valid post-backslash "
                  "character."));
    }
  } else {
    ss << str[*cursor];
    if (str[*cursor] == '"') {
      break;
    }
  }
  OK_OR_RET(IncrementCursor(str, cursor));
}
#endif
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

  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  std::vector<Object> array;
  while (InBounds(str, *cursor)) {
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (str[*cursor] == ']') {
      break;
    }
    if (str[*cursor] == ',') {
      OK_OR_RET(IncrementCursor(str, cursor));
      continue;
    }
    Object object;
    SET_OR_RET(object, ParseObject(str, cursor));
    array.push_back(object);
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  }

  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
  if (str[*cursor] != ']') {
    return TRACE(
        Error("Did not find ending ']' in array."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  return array;
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
  while (InBounds(str, *cursor)) {
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (str[*cursor] == '}') {
      break;
    }
    if (str[*cursor] == ',') {
      OK_OR_RET(IncrementCursor(str, cursor));
      continue;
    }
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    std::string key;
    SET_OR_RET(key, ParseString(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    if (str[*cursor] != ':') {
      return TRACE(
          Error("Did not find ':' after key in map."));
    }
    OK_OR_RET(IncrementCursor(str, cursor));
    OK_OR_RET(MaybeConsumeWhitespace(str, cursor));
    Object object;
    SET_OR_RET(object, ParseObject(str, cursor));
    map[key] = object;
  }

  if (str[*cursor] != '}') {
    return TRACE(Error("Did not find ending '}' in map."));
  }
  OK_OR_RET(IncrementCursor(str, cursor));

  return map;
}

ResultOr<Object> ParseObject(std::string str) {
#if PROFILING
  ResultOr<Object> result = cs::Error("Result not set.");
  // TIME_IT_START
  unsigned int cursor = 0;
  result = ParseObject(str, &cursor);
  // TIME_IT_END("Parsing took ");
  return result;
#else
  unsigned int cursor = 0;
  return ParseObject(str, &cursor);
#endif  // #if PROFILING
}

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
      SET_OR_RET(object._value, ParseMap(str, cursor));
      break;
    } else if (c == '[') {
      // Parse array
      SET_OR_RET(object._value, ParseArray(str, cursor));
      break;
    } else if (c == '+' || c == '-' || c == '.' ||
               ('0' <= c && c <= '9')) {
      // Parse float
      const auto& n = ParseNumber(str, cursor);
      if (!n.ok()) {
        return n;
      }
      if (std::holds_alternative<int>(n.value())) {
        object._value = std::get<int>(n.value());
      } else if (std::holds_alternative<float>(n.value())) {
        object._value = std::get<float>(n.value());
      }
      break;
    } else if (c == '\"') {
      // Parse string
      SET_OR_RET(object._value, ParseString(str, cursor));
      break;
    } else if (c == 't' || c == 'f') {
      // Parse bool
      SET_OR_RET(object._value, ParseBoolean(str, cursor));
      break;
    } else {
      return TRACE(Error(cs::util::fmt(
          "Reached unexpected character ('%c') at "
          "cursor=%d in str=" +
              str,
          c, *cursor)));
    }
  }

  return object;
}
}  // namespace cs::net::json::parsers
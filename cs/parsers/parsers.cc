// cs/parsers/parsers.cc
#include "cs/parsers/parsers.hh"

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
#include "cs/result.hh"
#include "cs/util/fmt.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
}  // namespace

namespace cs::parsers {

bool InBounds(std::string str, unsigned int cursor) {
  return cursor < str.size();
}

Result IncrementCursor(std::string str,
                       unsigned int* cursor) {
  // LOG(DEBUG) << "IncrementCursor(str=" << str
  //            << ", *cursor=" << *cursor << ")"  <<  ENDL;
  OK_OR_RET(CheckInBounds(str, *cursor));
  *cursor += 1;
  return Ok();
}

bool TryConsumeString(std::string str, std::string target,
                      unsigned int* cursor) {
  unsigned int str_cursor = *cursor;
  unsigned int target_cursor = 0;

  while (InBounds(target, target_cursor)) {
    if (!InBounds(str, str_cursor)) {
      return false;
    }
    if (str[str_cursor] != target[target_cursor]) {
      return false;
    }
    str_cursor += 1;
    target_cursor += 1;
  }
  *cursor = str_cursor;
  return true;
}

Result MaybeConsumeWhitespace(std::string str,
                              unsigned int* cursor) {
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];
    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
      OK_OR_RET(IncrementCursor(str, cursor));
    } else {
      break;
    }
  }
  return Ok();
}

Result CheckInBounds(std::string str, unsigned int cursor) {
  if (!InBounds(str, cursor)) {
    std::stringstream ss;
    ss << "CheckInBounds: Cursor out of bounds: "
          "str.size()="
       << str.size() << ", str=" << str
       << ", cursor=" << cursor << ".";
    return TRACE(Error(ss.str()));
  }
  return Ok();
}

ResultOr<unsigned int> _ParseUnsignedInt(
    std::string str, unsigned int* cursor) {
  OK_OR_RET(CheckInBounds(str, *cursor));

  unsigned int result = 0;

  // Handle sign.
  if (str[*cursor] == '+') {
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  // Parse digits and build the integer part.
  std::vector<unsigned int> digits;
  while (InBounds(str, *cursor) &&
         std::isdigit(str[*cursor])) {
    char this_digit = str[*cursor] - '0';
    digits.push_back(this_digit);
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  float tens_multiplier = 1;
  for (unsigned int i = digits.size(); i > 0; i--) {
    result += digits[i - 1] * tens_multiplier;
    tens_multiplier *= 10;
  }

  return result;
}

ResultOr<int> ParseInt(std::string str,
                       unsigned int* cursor) {
  OK_OR_RET(CheckInBounds(str, *cursor));

  bool is_negative = false;
  int result = 0;

  // Handle sign.
  if (str[*cursor] == '+') {
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  if (str[*cursor] == '-') {
    is_negative = (str[*cursor] == '-');
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  // Parse digits and build the integer part.
  std::vector<unsigned int> digits;
  while (InBounds(str, *cursor) &&
         std::isdigit(str[*cursor])) {
    unsigned int this_digit = str[*cursor] - '0';
    digits.push_back(this_digit);
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  int tens_multiplier = 1;
  for (int i = digits.size() - 1; i >= 0; i--) {
    result += digits[i] * tens_multiplier;
    tens_multiplier *= 10;
  }

  // Apply sign
  result = (is_negative ? -result : result);

  return result;
}

ResultOr<int> ParseInt(std::string str) {
  unsigned int cursor = 0;
  auto result = ParseInt(str, &cursor);
  if (result.ok() && cursor == 0) {
    return TRACE(
        InvalidArgument("Didn't find int in string."));
  }
  return result;
}

ResultOr<unsigned int> ParseUnsignedInt(std::string str) {
  unsigned int cursor = 0;
  auto result = _ParseUnsignedInt(str, &cursor);
  if (result.ok() && cursor == 0) {
    return TRACE(
        InvalidArgument("Didn't find int in string."));
  }
  return result;
}

ResultOr<float> ParseFloat(std::string str) {
  unsigned int cursor = 0;
  return ParseFloat(str, &cursor);
}

ResultOr<float> ParseFloat(std::string str,
                           unsigned int* cursor) {
  OK_OR_RET(CheckInBounds(str, *cursor));
  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  bool is_negative = false;
  bool has_exponent = false;
  bool negative_exponent = false;
  float exponent_value = 0;
  float result = 0.0f;

  // Handle sign.
  if (str[*cursor] == '+') {
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  if (str[*cursor] == '-') {
    is_negative = (str[*cursor] == '-');
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  // Parse digits and build the integer part.
  std::vector<unsigned int> digits;
  while (InBounds(str, *cursor) &&
         std::isdigit(str[*cursor])) {
    unsigned int this_digit = str[*cursor] - '0';
    digits.push_back(this_digit);
    OK_OR_RET(IncrementCursor(str, cursor));
  }

  float tens_multiplier = 1;
  for (int i = digits.size() - 1; i >= 0; i--) {
    result += digits[i] * tens_multiplier;
    tens_multiplier *= 10;
  }

  // Parse decimal point and digits for the fractional
  // part.
  if (InBounds(str, *cursor) && str[*cursor] == '.') {
    OK_OR_RET(IncrementCursor(str, cursor));
    float decimal_multiplier = 0.1f;
    while (InBounds(str, *cursor) &&
           std::isdigit(str[*cursor])) {
      auto digit = str[*cursor] - '0';
      result += digit * decimal_multiplier;
      decimal_multiplier *= 0.1f;
      OK_OR_RET(IncrementCursor(str, cursor));
    }
  }

  // Parse exponent and digits for the exponent part.
  if (InBounds(str, *cursor) &&
      (str[*cursor] == 'e' || str[*cursor] == 'E')) {
    has_exponent = true;
    OK_OR_RET(IncrementCursor(str, cursor));

    if (InBounds(str, *cursor)) {
      if (str[*cursor] == '+') {
        OK_OR_RET(IncrementCursor(str, cursor));
      } else if (str[*cursor] == '-') {
        negative_exponent = true;
        OK_OR_RET(IncrementCursor(str, cursor));
      }
    }

    while (InBounds(str, *cursor) &&
           std::isdigit(str[*cursor])) {
      exponent_value =
          exponent_value * 10 + (str[*cursor] - '0');
      OK_OR_RET(IncrementCursor(str, cursor));
    }
  }

  // Apply sign and exponent.
  result = (is_negative ? -result : result);
  if (has_exponent) {
    if (negative_exponent) {
      exponent_value *= -1;
    }
    result *= std::pow(10.f, exponent_value);
  }

  OK_OR_RET(MaybeConsumeWhitespace(str, cursor));

  return result;
}

ResultOr<std::string> ConsumePrefix(std::string prefix,
                                    std::string str) {
  if (prefix.size() == 0) {
    return TRACE(InvalidArgument("prefix.size() is 0"));
  }
  if (str.size() == 0) {
    return TRACE(InvalidArgument("str.size() is 0"));
  }
  if (str.size() < prefix.size()) {
    return InvalidArgument(
        FMT("str.size()=%d must be greater than "
            "prefix.size()=%d",
            str.size(), prefix.size()));
  }
  if (str.rfind(prefix, /*start=*/0) == 0) {
    return str.substr(prefix.length());
  } else {
    return TRACE(Error(
        FMT("Error: prefix not found. prefix=%s, str=%s",
            prefix.c_str(), str.c_str())));
  }
}

ResultOr<bool> HasFileExtension(std::string str,
                                std::string extension) {
  LOG(DEBUG) << "HasFileExtension(str=" << str
             << ", extension=" << extension << ")" << ENDL;
  if (str.size() < extension.size()) {
    return false;
  }
  if (str.rfind(extension,
                str.length() - extension.size()) ==
      (str.length() - extension.size())) {
    return true;
  }
  return false;
}

cs::ResultOr<unsigned int> FindFirstIndexOf(
    std::string str, std::string target) {
  unsigned int index = str.find_first_of(target);
  OK_OR_RETURN(CheckInBounds(str, index));
  return index;
}

ResultOr<std::string> ConsumeUntil(
    std::string str, unsigned int* cursor,
    std::string stopping_characters) {
  std::stringstream ss;
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];
    if (stopping_characters.find(c) != std::string::npos) {
      break;
    }
    ss << c;
    OK_OR_RETURN(IncrementCursor(str, cursor));
  }
  return ss.str();
}

ResultOr<std::string> Consume(
    std::string str, unsigned int* cursor,
    std::string stopping_characters, std::string alphabet) {
  std::stringstream ss;
  while (InBounds(str, *cursor)) {
    char c = str[*cursor];
    if (stopping_characters.find(c) != std::string::npos) {
      break;
    }
    if (alphabet.find(c) == std::string::npos) {
      return TRACE(InvalidArgument(
          FMT("Character '%c' not in alphabet '%s'", c,
              alphabet.c_str())));
    }
    ss << c;
    OK_OR_RETURN(IncrementCursor(str, cursor));
  }
  return ss.str();
}

}  // namespace cs::parsers

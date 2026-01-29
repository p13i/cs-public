// cs/parsers/parsers.hh
#ifndef CS_PARSERS_PARSERS_HH
#define CS_PARSERS_PARSERS_HH

#include <cstdint>
#include <string>

#include "cs/result.hh"

namespace cs::parsers {

bool InBounds(std::string str, unsigned int cursor);

cs::Result IncrementCursor(std::string str,
                           unsigned int* cursor);

bool TryConsumeString(std::string str, std::string target,
                      unsigned int* cursor);

cs::Result MaybeConsumeWhitespace(std::string str,
                                  unsigned int* cursor);

cs::Result CheckInBounds(std::string str,
                         unsigned int cursor);

cs::ResultOr<int> ParseInt(std::string str);

cs::ResultOr<int> ParseInt(std::string str,
                           unsigned int* cursor);

cs::ResultOr<unsigned int> ParseUnsignedInt(
    std::string str);

cs::ResultOr<float> ParseFloat(std::string str);

cs::ResultOr<float> ParseFloat(std::string str,
                               unsigned int* cursor);

// Produces a new string with given prefix removed from the
// given string. If the prefix wasn't found, an error is
// returned.
ResultOr<std::string> ConsumePrefix(std::string prefix,
                                    std::string str);

cs::ResultOr<bool> HasFileExtension(std::string str,
                                    std::string extension);

cs::ResultOr<unsigned int> FindFirstIndexOf(
    std::string str, std::string target);

ResultOr<std::string> ConsumeUntil(
    std::string str, unsigned int* cursor,
    std::string stopping_characters);

ResultOr<std::string> Consume(
    std::string str, unsigned int* cursor,
    std::string stopping_characters, std::string alphabet);

}  // namespace cs::parsers

#endif
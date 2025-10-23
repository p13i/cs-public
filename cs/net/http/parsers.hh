#ifndef CS_NET_HTTP_PARSERS_HH
#define CS_NET_HTTP_PARSERS_HH

#include <stdint.h>

#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include "cs/result.hh"

namespace cs::net::http::parsers {

cs::Result AtEndOfLine(std::string str,
                       unsigned int cursor);

cs::Result IncrementCursor(std::string str,
                           unsigned int* cursor);

#define STRING_CONTAINS(str, ch) \
  (str.find(ch) != std::string::npos)

cs::Result ReadWord(std::string str, unsigned int* cursor,
                    std::string* token,
                    std::string ending_tokens);

cs::Result ReadThroughNewline(std::string str,
                              unsigned int* cursor);

cs::Result ParsePath(
    std::string original_path, std::string* just_path,
    std::map<std::string, std::string>* query_params);

}  // namespace cs::net::http::parsers

#endif  // CS_NET_HTTP_PARSERS_HH
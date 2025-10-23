#ifndef CS_NET_JSON_PARSERS_HH
#define CS_NET_JSON_PARSERS_HH

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

#include "cs/net/json/object.hh"
#include "cs/result.hh"

namespace cs::net::json::parsers {

cs::Result operator>>(std::string str, Object object);

cs::ResultOr<Object> ParseObject(std::string str,
                                 unsigned int* cursor);

cs::ResultOr<Object> ParseObject(std::string str);

cs::ResultOr<std::map<std::string, Object>> ParseMap(
    std::string str, unsigned int* cursor);

cs::ResultOr<std::string> ParseString(std::string str);

cs::ResultOr<std::string> ParseString(std::string str,
                                      unsigned int* cursor);

cs::ResultOr<bool> ParseBoolean(std::string str,
                                unsigned int* cursor);

cs::ResultOr<std::variant<float, int>> ParseNumber(
    std::string str);

cs::ResultOr<std::variant<float, int>> ParseNumber(
    std::string str, unsigned int* cursor);

cs::ResultOr<std::vector<Object>> ParseArray(
    std::string str, unsigned int* cursor);

}  // namespace cs::net::json::parsers

#endif  // CS_NET_JSON_PARSERS_HH
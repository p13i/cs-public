#include "cs/net/rpc/rpc.hh"

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/json/object.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"

namespace cs::net::rpc {

std::string QueryParamsToJson(
    std::map<std::string, std::string> map) {
  cs::net::json::Object::KVMap kv;
  for (const auto& [key, value] : map) {
    cs::net::json::Object obj;
    if (auto result = cs::parsers::ParseInt(value);
        result.ok()) {
      obj = cs::net::json::Object::NewInt(result.value());
    } else {
      obj = cs::net::json::Object::NewString(value);
    }
    kv[key] = obj;
  }
  cs::net::json::Object json(kv);
  return json.str();
}

}  // namespace cs::net::rpc
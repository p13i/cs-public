// cs/net/rpc/rpc.cc
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

namespace {  // use_usings
using ::cs::net::json::Object;
using ::cs::parsers::ParseInt;
}  // namespace

namespace cs::net::rpc {

std::string QueryParamsToJson(
    std::map<std::string, std::string> map) {
  Object::KVMap kv;
  for (const auto& [key, value] : map) {
    Object obj;
    if (auto result = ParseInt(value); result.ok()) {
      obj = Object::NewInt(result.value());
    } else {
      obj = Object::NewString(value);
    }
    kv[key] = obj;
  }
  Object json(kv);
  return json.str();
}

}  // namespace cs::net::rpc
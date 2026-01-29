// cs/net/proto/api.cc
#include "cs/net/proto/api.hh"

#include <chrono>
#include <fstream>
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

namespace cs::net::proto::api {

std::string QueryParamsToJson(
    std::map<std::string, std::string> map) {
  cs::net::json::Object::KVMap kv;
  for (const auto& [key, value] : map) {
    LOG(INFO) << "[DEBUG] QueryParamsToJson - key=" << key
              << " value=" << value << ENDL;
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
  std::string json_str = json.str();
  LOG(INFO) << "[DEBUG] Generated JSON from " << map.size()
            << " params: " << json_str << ENDL;
  return json_str;
}

}  // namespace cs::net::proto::api
#ifndef CS_WWW_APP_EMSCRIPTEN_EMSCRIPTEN_HH
#define CS_WWW_APP_EMSCRIPTEN_EMSCRIPTEN_HH

#include <stdint.h>
#include <stdio.h>

#include <cstring>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"

// ProtoToQueryString

#ifndef __EMSCRIPTEN__
#define FETCH_API(method_, url_, req_, resp_, request_, \
                  callback_)
#endif  // ifndef __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__

#define FETCH_API(method_, url_, req_, resp_, request_, \
                  callback_)                            \
  cs::www::app::DoApiFetch<req_, resp_>(                \
      #method_, request_, url_, callback_)

namespace cs::www::app {

template <typename ProtoT>
ResultOr<std::string> ProtoToQueryString(ProtoT p) {
  std::string s = p.Serialize();
  SET_OR_RET(cs::net::json::Object o,
             cs::net::json::parsers::ParseObject(s));
  if (o.type() != cs::net::json::Type::kMap) {
    return cs::InvalidArgument(
        "Unable to re-parse proto, must be a map string to "
        "parse.");
  }
  std::stringstream ss;
  bool first = true;
  for (const auto& [key, value] : o.as_map()) {
    if (value.type() == cs::net::json::Type::kNumber ||
        value.type() == cs::net::json::Type::kBoolean ||
        value.type() == cs::net::json::Type::kString) {
      if (!first) {
        ss << "&";
      } else {
        first = false;
      }
      ss << key << "=";
      if (value.is(float())) {
        ss << value.as(float());
      } else if (value.is(int())) {
        ss << value.as(int());
      } else if (value.is(bool())) {
        ss << value.as(bool());
      } else if (value.is(std::string())) {
        ss << value.as(std::string());
      } else {
        return cs::Error(
            "Unable to find handler for type: " +
            value.type_string());
      }
    } else {
      LOG(WARNING)
          << "Ignoring non-number/bool/string type "
          << value.type_string() << ENDL;
    }
  }
  return ss.str();
}

#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

// Function pointer to parse and handle the response
template <typename ResponseProto>
using ProtoCallback =
    std::function<void(cs::ResultOr<ResponseProto>)>;

struct FetchPayload {
  std::function<void(const std::string&)>* parse_fn;
};

void downloadSucceededAdapter(emscripten_fetch_t* fetch) {
  LOG(DEBUG) << "downloadSucceededAdapter" << ENDL;
  auto* payload =
      static_cast<FetchPayload*>(fetch->userData);
  std::string body(fetch->data, fetch->numBytes);
  (*payload->parse_fn)(body);

  delete payload->parse_fn;
  delete payload;
  emscripten_fetch_close(fetch);
}

void downloadFailed(emscripten_fetch_t* fetch) {
  LOG(DEBUG) << "downloadFailed" << ENDL;
  auto* payload =
      static_cast<FetchPayload*>(fetch->userData);

  delete payload->parse_fn;
  delete payload;
  emscripten_fetch_close(fetch);
}

template <typename RequestProto, typename ResponseProto>
void DoApiFetch(const std::string& method,
                RequestProto request,
                const std::string& url,
                ProtoCallback<ResponseProto> callback) {
  LOG(INFO) << "Starting API fetch: " << method << " "
            << url << ENDL;
  std::string qs_params = "";
  if (method == "GET") {
    auto qs_result = ProtoToQueryString(request);
    if (qs_result.ok()) {
      qs_params = "?" + qs_result.value();
    } else {
      LOG(WARNING)
          << "Unable to serialize proto into query string."
          << ENDL;
    }
  };

  LOG(DEBUG) << "Creating parse function." << ENDL;
  // Create parse function
  auto* parse_fn =
      new std::function<void(const std::string&)>(
          [callback](const std::string& body) {
            LOG(DEBUG) << "In callback wrapper." << ENDL;
            auto parsed = ResponseProto().Parse(body);
            if (!parsed.ok()) {
              LOG(ERROR)
                  << "Parse failed:" << parsed.message()
                  << ENDL;
              return callback(parsed);
            }
            LOG(DEBUG) << "In callback wrapper." << ENDL;
            callback(parsed.value());
            LOG(DEBUG) << "Invoked callback." << ENDL;
          });

  // Allocate userData payload
  auto* payload = new FetchPayload{parse_fn};
  LOG(DEBUG) << "Allocated payload." << ENDL;

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strncpy(attr.requestMethod, method.c_str(),
          sizeof(attr.requestMethod) - 1);
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = downloadSucceededAdapter;
  attr.onerror = downloadFailed;
  attr.userData = payload;

  if (method == "GET") {
    attr.requestData = "";
    attr.requestDataSize = 0;
  } else {
    std::string data = request.Serialize();
    attr.requestData = data.data();
    attr.requestDataSize = data.size();
  }

  static const char* headers[] = {
      "Content-Type", "application/json", nullptr};
  attr.requestHeaders = headers;

  std::string final_url = std::string(url) + qs_params;
  emscripten_fetch(&attr, final_url.c_str());
  LOG(DEBUG) << "Sent fetch." << ENDL;
}
}  // namespace cs::www::app

#endif  // __EMSCRIPTEN__
#endif  // CS_WWW_APP_EMSCRIPTEN_EMSCRIPTEN_HH
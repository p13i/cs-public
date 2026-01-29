// cs/net/http/web_app.hh
#ifndef CS_NET_HTTP_WEB_APP_HH
#define CS_NET_HTTP_WEB_APP_HH

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>

#include "cs/apps/trycopilot.ai/api/logs.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/log.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/result.hh"

namespace cs::net::http {

#define ADD_ROUTE(_app, _method, _path, _function) \
  OK_OR_RET(_app.Register(_method, _path, _function))

class WebApp {
 public:
  Result Register(std::string method, std::string pattern,
                  std::function<cs::net::http::Response(
                      cs::net::http::Request)>
                      handler) {
    handlers_.push_back(
        std::make_tuple(method, pattern, handler));
    return Ok();
  }

  Result RunServer(std::string ip_address, int port) {
    server_ = std::make_unique<cs::net::http::Server>(
        ip_address, port);
    OK_OR_RET(server_->Bind());
    OK_OR_RET(server_->StartListening(
        std::bind(&WebApp::main_handler, this,
                  std::placeholders::_1)));
    return Ok();
  }

  // Return routes of (HTTP method, pattern)
  std::vector<std::tuple<std::string, std::string>>
  Routes() {
    std::vector<std::tuple<std::string, std::string>>
        routes;
    for (auto path_info : handlers_) {
      const auto [method, pattern, handler] = path_info;
      routes.push_back(std::make_tuple(method, pattern));
    }
    return routes;
  }

  // Get current thread pool load as ratio of active threads
  // to total threads
  float GetThreadLoad() const {
    return server_ ? server_->GetThreadLoad() : 0.0f;
  }

 private:
  bool PathMatches(std::string path, std::string pattern) {
    if (pattern.size() == 0 || path.size() == 0) {
      return false;
    }
    if (path == pattern) {
      return true;
    }
    if (pattern[pattern.size() - 1] == '*') {
      if (path.rfind(pattern.substr(0, pattern.size() - 1),
                     /*start=*/0) == 0) {
        return true;
      }
    }
    return false;
  }

  cs::net::http::Response main_handler(
      cs::net::http::Request request) {
    // Attach this WebApp instance to the request using
    // shared_ptr with no-op deleter (WebApp outlives the
    // request, so we don't need to manage lifetime)
    request.set_app(
        std::shared_ptr<WebApp>(this, [](WebApp*) {}));

    cs::net::http::Response response(HTTP_404_NOT_FOUND);
    for (auto path_info : handlers_) {
      const auto [method, pattern, handler] = path_info;
      if (request.method() != method) {
        continue;
      }
      if (!PathMatches(request.path(), pattern)) {
        continue;
      }
      response = handler(request);
      break;
    }

#if 0
    // Log http request and response.
    // Skip logging database-service requests to prevent
    // recursive JSON escaping
    bool is_database_service_request =
        request.path() == "/rpc/upsert/" ||
        request.path() == "/rpc/query/";
    // Disabled: http_logs and app_logs logging to prevent database-service requests
    if (!is_database_service_request) {
      LOG(DEBUG)
          << cs::apps::trycopilotai::api::CreateLogAPI().Process(
               cs::apps::trycopilotai::protos::gencode::log::CreateLogRequestBuilderImpl()
                   .set_request(
                       cs::apps::trycopilotai::protos::gencode::
                           log::HttpRequestBuilderImpl()
                               .set_receive_timestamp(
                                   cs::apps::trycopilotai::
                                       protos::gencode::log::
                                           TimestampBuilderImpl()
                                               .set_iso8601(
                                                   cs::util::time::
                                                       NowAsISO8601TimeUTC())
                                               .Build())
                               .set_method(request.method())
                               .set_path(request.path())
                               .set_content_type(
                                   response._content_type)
                               .set_content_length(
                                   response.body().size())
                               .set_body(request.body())
                               .Build())
                   .set_response(
                       cs::apps::trycopilotai::protos::gencode::
                           log::HttpResponseBuilderImpl()
                               .set_send_timestamp(
                                   cs::apps::trycopilotai::
                                       protos::gencode::log::
                                           TimestampBuilderImpl()
                                               .set_iso8601(
                                                   cs::util::time::
                                                       NowAsISO8601TimeUTC())
                                               .Build())
                               .set_status(
                                   cs::apps::trycopilotai::
                                       protos::gencode::log::
                                           HttpStatusBuilderImpl()
                                               .set_code(
                                                   response
                                                       ._status
                                                       .code)
                                               .set_name(
                                                   response
                                                       ._status
                                                       .name)
                                               .Build())
                               .set_content_type(
                                   response._content_type)
                               .set_content_length(
                                   response.body().size())
                               .set_body(response.body())
                               .Build())
                   .set_result(
                       cs::apps::trycopilotai::protos::
                           gencode::log::ResultBuilderImpl()
                               .set_code(
                                   response._result.code())
                               .set_message(response._result
                                                .message())
                               .Build())
                   .Build())
          << ENDL;
    }
#endif  // #if 0
    return response;
  };

  std::vector<
      std::tuple<std::string, std::string,
                 std::function<cs::net::http::Response(
                     cs::net::http::Request)>>>
      handlers_;
  std::unique_ptr<cs::net::http::Server> server_;
};
}  // namespace cs::net::http

#endif  // CS_NET_HTTP_WEB_APP_HH

// cs/net/http/web_app.hh
#ifndef CS_NET_HTTP_WEB_APP_HH
#define CS_NET_HTTP_WEB_APP_HH

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <type_traits>

#include "cs/apps/trycopilot.ai/api/logs.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/log.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::net::http {

#define ADD_ROUTE(_app, _method, _path, _fn)              \
  OK_OR_RET(                                              \
      _app.Register(_method, _path,                       \
                    [](cs::net::http::Request r,          \
                       decltype(_app)::context_type& c) { \
                      return _fn(r, c);                   \
                    }))
// Use for handlers that must capture (e.g. load endpoint).
#define ADD_ROUTE_CAPTURE(_app, _method, _path, _fn) \
  OK_OR_RET(_app.Register(_method, _path, _fn))

template <typename ContextType = ::cs::util::di::Context<>>
class WebApp {
 public:
  using context_type = ContextType;

  explicit WebApp(ContextType& ctx) : context_(&ctx) {}

  // Default constructor when ContextType is Context<> (no
  // local ctx needed).
  template <typename T = ContextType>
  explicit WebApp(
      std::enable_if_t<
          std::is_same_v<T, ::cs::util::di::Context<>>>* =
          nullptr)
      : context_(&DefaultEmptyContext()) {}

  static ::cs::util::di::Context<>& DefaultEmptyContext() {
    static ::cs::util::di::Context<> ctx;
    return ctx;
  }

  float GetThreadLoad() const {
    return server_ ? server_->GetThreadLoad() : 0.0f;
  }

  ContextType& ctx() { return *context_; }
  const ContextType& ctx() const { return *context_; }

  Result Register(std::string method, std::string pattern,
                  std::function<cs::net::http::Response(
                      cs::net::http::Request, ContextType&)>
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
        std::bind(&WebApp<ContextType>::main_handler, this,
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
    cs::net::http::Response response(HTTP_404_NOT_FOUND);
    for (auto path_info : handlers_) {
      const auto [method, pattern, handler] = path_info;
      if (request.method() != method) {
        continue;
      }
      if (!PathMatches(request.path(), pattern)) {
        continue;
      }
      response = handler(request, ctx());
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

  ContextType* context_;
  std::vector<std::tuple<
      std::string, std::string,
      std::function<cs::net::http::Response(
          cs::net::http::Request, ContextType&)>>>
      handlers_;
  std::unique_ptr<cs::net::http::Server> server_;
};
}  // namespace cs::net::http

#endif  // CS_NET_HTTP_WEB_APP_HH

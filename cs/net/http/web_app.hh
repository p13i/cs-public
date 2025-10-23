#ifndef CS_NET_HTTP_WEB_APP_HH
#define CS_NET_HTTP_WEB_APP_HH

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/result.hh"
#include "cs/www/app/api/logs.hh"
#include "cs/www/app/protos/gencode/log.proto.hh"

namespace cs::net::http {

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
    auto server = cs::net::http::Server(ip_address, port);
    OK_OR_RET(server.Bind());
    OK_OR_RET(server.StartListening(
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
      response = handler(request);
      break;
    }
    // Log http request and response.
    LOG(DEBUG)
        << cs::www::app::api::CreateLogAPI().Process(
               cs::www::app::protos::gencode::log::CreateLogRequestBuilderImpl()
                   .set_request(
                       cs::www::app::protos::gencode::log::
                           HttpRequestBuilderImpl()
                               .set_receive_timestamp(
                                   cs::www::app::protos::gencode::
                                       log::TimestampBuilderImpl()
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
                       cs::www::app::protos::gencode::log::
                           HttpResponseBuilderImpl()
                               .set_send_timestamp(
                                   cs::www::app::protos::gencode::
                                       log::TimestampBuilderImpl()
                                           .set_iso8601(
                                               cs::util::time::
                                                   NowAsISO8601TimeUTC())
                                           .Build())
                               .set_status(
                                   cs::www::app::protos::
                                       gencode::log::
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
                       cs::www::app::protos::gencode::log::
                           ResultBuilderImpl()
                               .set_code(
                                   response._result.code())
                               .set_message(response._result
                                                .message())
                               .Build())
                   .Build())
        << ENDL;
    return response;
  };

  std::vector<
      std::tuple<std::string, std::string,
                 std::function<cs::net::http::Response(
                     cs::net::http::Request)>>>
      handlers_;
};
}  // namespace cs::net::http

#endif  // CS_NET_HTTP_WEB_APP_HH

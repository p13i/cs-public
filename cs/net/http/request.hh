// cs/net/http/request.hh
#ifndef CS_NET_HTTP_REQUEST_HH
#define CS_NET_HTTP_REQUEST_HH

#include <stdint.h>

#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

#include "cs/result.hh"

namespace cs::net::http {
class WebApp;  // Forward declaration.

class Request {
 public:
  Request()
      : _method(""),
        _path("/"),
        _query_params({}),
        _headers({}),
        _body(""),
        _str(""),
        _app(nullptr) {}

  std::string summary() {
    std::stringstream ss;
    ss << "HTTP " << this->_method << " " << this->_path
       << " " << STREAM_STRING_MAP(this->_query_params);
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const Request& request) {
    return os << request._str;
  }

  friend cs::Result operator>>(char buffer[],
                               Request& request) {
    return request.Parse(std::string(buffer));
  }

  cs::Result Parse(std::string str);

  std::string path() { return _path; }
  std::string path() const { return _path; }

  std::string method() { return _method; }
  std::string method() const { return _method; }

  cs::ResultOr<std::string> GetQueryParam(
      std::string name) const {
    auto found = _query_params.find(name);
    if (found == _query_params.end()) {
      return TRACE(cs::Error(
          FMT("No query param with name %s found.",
              name.c_str())));
    }
    return found->second;
  }

  std::string body() { return _body; }
  std::string body() const { return _body; }

  void set_body(std::string val) {
    _body = val;
    _str = _str + _body;
  }

  std::map<std::string, std::string> headers() {
    return _headers;
  }
  std::map<std::string, std::string> headers() const {
    return _headers;
  }

  // Returns the normalized Host header (lowercase, port
  // stripped). Returns an error if the Host header is
  // missing.
  cs::ResultOr<std::string> NormalizedHost() const;

  std::map<std::string, std::string> query_params() {
    return _query_params;
  }
  std::map<std::string, std::string> query_params() const {
    return _query_params;
  }

  void set_app(std::shared_ptr<WebApp> app) { _app = app; }

  std::shared_ptr<WebApp> app() { return _app; }

  std::shared_ptr<const WebApp> app() const { return _app; }

  std::string _method;
  std::string _path;
  std::map<std::string, std::string> _query_params;
  std::map<std::string, std::string> _headers;
  std::string _body;
  std::string _str;
  std::shared_ptr<WebApp> _app;

  std::string UrlDecode(const std::string& str);

  std::map<std::string, std::string> ParseFormUrlEncoded(
      const std::string& body);
};

}  // namespace cs::net::http

#endif  // CS_NET_HTTP_REQUEST_HH

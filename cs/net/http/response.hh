// cs/net/http/response.hh
#ifndef CS_NET_HTTP_RESPONSE_HH
#define CS_NET_HTTP_RESPONSE_HH

#include <stdint.h>

#include <functional>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include "cs/net/http/parsers.hh"
#include "cs/net/http/status.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

namespace cs::net::http {

const std::string kContentTypeTextPlain = "text/plain";
const std::string kContentTypeTextHtml = "text/html";
const std::string kContentTypeApplicationJson =
    "application/json";
const std::string kContentTypeXWwwFormUrlEncoded =
    "application/x-www-form-urlencoded";

class Response {
 public:
  Response() : Response(HTTP_200_OK) {}
  Response(std::string content_type, std::string s)
      : Response(HTTP_200_OK, content_type, s) {}
  Response(Status status)
      : Response(status, kContentTypeTextPlain,
                 status.str()) {}
  Response(Status status, std::string content_type,
           std::stringstream ss)
      : Response(status, content_type, ss.str()) {}
  Response(Status status, std::string content_type,
           std::string body)
      : _status(status),
        _content_type(content_type),
        _body(body) {}
  Response(const cs::Result& result) {
    _result = result;
    if (result.ok()) {
      _status = HTTP_200_OK;
    } else {
      _status = HTTP_400_BAD_REQUEST;
    }
    _content_type = kContentTypeTextPlain;
    std::stringstream ss;
    ss << _status;
    if (result.message().size() > 0) {
      ss << ": " << result.message();
    }
    _body = ss.str();
  }
  Response(std::string status_str) {
    if (status_str == HTTP_200_OK.str()) {
      _status = HTTP_200_OK;
    } else if (status_str == HTTP_400_BAD_REQUEST.str()) {
      _status = HTTP_400_BAD_REQUEST;
    }
    _content_type = kContentTypeTextPlain;
  }

  cs::Result Parse(std::string str) {
    _str = str;
    if (str.size() == 0) {
      _status = HTTP_400_BAD_REQUEST;
      return TRACE(cs::Error(
          "Unable to parse Response from empty string."));
    }
    _str = str;
    unsigned int cursor = 0;
    // Read any leading whitespace.
    while (str[cursor] == ' ' || str[cursor] == '\n' ||
           str[cursor] == '\r') {
      OK_OR_RET(cs::net::http::parsers::IncrementCursor(
          str, &cursor));
    }
    // Read HTTP/1.1 tag
    std::string http_tag = "";
    if (cursor == str.size()) {
      _status = HTTP_500_INTERNAL_SERVER_ERROR;
      return TRACE(cs::Error(
          "Unable to finding anything but whitespace while "
          "parsing str=`" +
          str + "`"));
    }
    // Read HTTP/1.1 tag
    OK_OR_RET(cs::net::http::parsers::ReadWord(
        str, &cursor, &http_tag, " \n"));
    if (http_tag != "HTTP/1.1") {
      return TRACE(cs::Error(
          "Didn't find HTTP/1.1 tag at beginning of first "
          "line but found:`" +
          http_tag + "` in `" + str + "`"));
    }
    OK_OR_RET(cs::net::http::parsers::IncrementCursor(
        str, &cursor));
    // Read HTTP status code and message ("400 BAD REQUEST")
    std::string status_str;
    OK_OR_RET(cs::net::http::parsers::ReadWord(
        str, &cursor, &status_str, " "));
    if (status_str == "400") {
      _status = cs::net::http::HTTP_400_BAD_REQUEST;
    } else if (status_str == "200") {
      _status = cs::net::http::HTTP_200_OK;
    } else if (status_str == "500") {
      _status =
          cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
    }
    OK_OR_RET(cs::net::http::parsers::ReadThroughNewline(
        str, &cursor));

    // Read headers
    bool reading_headers = true;
    while (reading_headers) {
      std::string name;
      OK_OR_RET(cs::net::http::parsers::ReadWord(
          str, &cursor, &name, ":"));
      OK_OR_RET(cs::net::http::parsers::IncrementCursor(
          str, &cursor));
      if (str.at(cursor) != ' ') {
        return TRACE(
            cs::Error("Expected space after colon."));
      }
      OK_OR_RET(cs::net::http::parsers::IncrementCursor(
          str, &cursor));
      std::string value;
      OK_OR_RET(cs::net::http::parsers::ReadWord(
          str, &cursor, &value, "\n"));
      OK_OR_RET(cs::net::http::parsers::ReadThroughNewline(
          str, &cursor));
      _headers[name] = value;
      if (cs::net::http::parsers::AtEndOfLine(str, cursor)
              .ok()) {
        reading_headers = false;
      }
    }
    OK_OR_RET(cs::net::http::parsers::ReadThroughNewline(
        str, &cursor));
    // Read body
    _body = str.substr(cursor, str.length() - cursor);
    return cs::Ok();
  }

  Response SetHeader(std::string name, std::string value) {
    _headers[name] = value;
    return *this;
  }

  friend std::ostream& operator<<(
      std::ostream& os, const Response& response) {
    os << "HTTP/1.1 " << response._status << "\r\n"
       << "Content-Type: " << response._content_type
       << "\r\n"
       << "Content-Length: " << response._body.size()
       << "\r\n";
    for (const auto& [name, value] : response._headers) {
      os << name << ": " << value << "\r\n";
    }
    os << "\r\n" << response._body;
    return os;
  }

  std::string body() { return _body; }

  Status status() { return _status; }

  std::string to_string() {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  cs::net::http::Status _status;
  std::string _content_type;
  std::string _body;
  std::string _str;
  std::map<std::string, std::string> _headers;
  cs::Result _result = cs::Ok();
};

Response HtmlResponse(std::string body);

Response HtmlResponse(std::stringstream ss);
}  // namespace cs::net::http

#endif  // CS_NET_HTTP_RESPONSE_HH
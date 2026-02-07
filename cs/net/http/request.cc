// cs/net/http/request.cc
#include "cs/net/http/request.hh"

#include <stdint.h>

#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include "cs/net/http/parsers.hh"
#include "cs/result.hh"
#include "cs/util/string.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::net::http::parsers::AtEndOfLine;
using ::cs::net::http::parsers::IncrementCursor;
using ::cs::net::http::parsers::ParsePath;
using ::cs::net::http::parsers::ReadThroughNewline;
using ::cs::net::http::parsers::ReadWord;
using ::cs::util::string::ToLowercase;
}  // namespace

namespace cs::net::http {

Result Request::Parse(std::string str) {
  _str = str;
  unsigned int cursor = 0;
  // Read HTTP method
  OK_OR_RET(ReadWord(str, &cursor, &_method, " "));
  OK_OR_RET(IncrementCursor(str, &cursor));
  // Read HTTP path
  OK_OR_RET(ReadWord(str, &cursor, &_path, " "));
  OK_OR_RET(ParsePath(_path, &_path, &_query_params));
  OK_OR_RET(IncrementCursor(str, &cursor));
  // Read HTTP/1.1 tag
  std::string http_tag = "";
  OK_OR_RET(ReadWord(str, &cursor, &http_tag, "\n"));
  if (http_tag != "HTTP/1.1") {
    return TRACE(Error(
        "Didn't find HTTP/1.1 tag at end of first line."));
  }
  OK_OR_RET(IncrementCursor(str, &cursor));
  // Read headers
  bool reading_headers = true;
  while (reading_headers) {
    std::string name;
    OK_OR_RET(ReadWord(str, &cursor, &name, ":"));
    OK_OR_RET(IncrementCursor(str, &cursor));
    if (str.at(cursor) != ' ') {
      return TRACE(Error("Expected space after colon."));
    }
    OK_OR_RET(IncrementCursor(str, &cursor));
    std::string value;
    OK_OR_RET(ReadWord(str, &cursor, &value, "\n"));
    OK_OR_RET(ReadThroughNewline(str, &cursor));
    _headers[name] = value;
    if (AtEndOfLine(str, cursor).ok()) {
      reading_headers = false;
    }
  }
  OK_OR_RET(ReadThroughNewline(str, &cursor));
  // Read body
  _body = str.substr(cursor, str.length() - cursor);
  return Ok();
}

std::string Request::UrlDecode(const std::string& str) {
  std::ostringstream decoded;
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == '%') {
      if (i + 2 < str.length()) {
        int hexValue;
        std::istringstream hexStream(str.substr(i + 1, 2));
        hexStream >> std::hex >> hexValue;
        decoded << static_cast<char>(hexValue);
        i += 2;
      }
    } else if (str[i] == '+') {
      decoded << ' ';
    } else {
      decoded << str[i];
    }
  }
  return decoded.str();
}

std::map<std::string, std::string>
Request::ParseFormUrlEncoded(const std::string& body) {
  std::map<std::string, std::string> formData;
  std::istringstream ss(body);
  std::string pair;
  while (std::getline(ss, pair, '&')) {
    size_t equalPos = pair.find('=');
    if (equalPos != std::string::npos) {
      std::string key = UrlDecode(pair.substr(0, equalPos));
      std::string value =
          UrlDecode(pair.substr(equalPos + 1));
      formData[key] = value;
    }
  }
  return formData;
}

ResultOr<std::string> Request::NormalizedHost() const {
  for (const auto& [name, value] : _headers) {
    if (ToLowercase(name) == "host") {
      std::string host = ToLowercase(value);
      std::string::size_type colon = host.find(':');
      if (colon != std::string::npos) {
        host = host.substr(0, colon);
      }
      return host;
    }
  }
  return TRACE(NotFoundError("Host header missing."));
}

}  // namespace cs::net::http
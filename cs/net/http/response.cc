// cs/net/http/response.cc
#include "cs/net/http/response.hh"

#include <stdint.h>

#include <map>
#include <ostream>
#include <sstream>
#include <string>

namespace cs::net::http {

Response HtmlResponse(std::string body) {
  return Response(HTTP_200_OK, kContentTypeTextHtml, body);
}

Response HtmlResponse(std::stringstream ss) {
  return Response(HTTP_200_OK, kContentTypeTextHtml,
                  ss.str());
}

}  // namespace cs::net::http
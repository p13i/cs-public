// cs/net/http/client.hh
#ifndef CS_NET_HTTP_CLIENT_HH
#define CS_NET_HTTP_CLIENT_HH

#include <map>
#include <string>

#include "cs/net/http/response.hh"
#include "cs/result.hh"

namespace cs {
ResultOr<std::string> Fetch(
    const std::string& url,
    const std::string& method = "GET",
    const std::map<std::string, std::string>& headers = {},
    const std::string& body = "");

ResultOr<cs::net::http::Response> FetchResponse(
    const std::string& url,
    const std::string& method = "GET",
    const std::map<std::string, std::string>& headers = {},
    const std::string& body = "");
}  // namespace cs
#endif  // CS_NET_HTTP_CLIENT_HH

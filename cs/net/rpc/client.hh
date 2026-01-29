// cs/net/rpc/client.hh
#ifndef CS_NET_RPC_CLIENT_HH
#define CS_NET_RPC_CLIENT_HH

#include <map>
#include <string>

#include "cs/net/http/client.hh"
#include "cs/net/http/response.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/result.hh"

namespace cs::net::rpc {

template <typename RpcType>
class RPCClient {
 public:
  explicit RPCClient(const std::string& base_url)
      : base_url_(base_url) {}

  using RequestType = typename RpcType::RequestType;
  using ResponseType = typename RpcType::ResponseType;

  ResultOr<ResponseType> Call(const std::string& endpoint,
                              const RequestType& request) {
    // Build full URL from base URL + endpoint
    SET_OR_RET(
        auto http_response,
        cs::FetchResponse(
            base_url_ + endpoint, "POST",
            {{"Content-Type",
              cs::net::http::kContentTypeApplicationJson}},
            request.Serialize()));

    // Check HTTP status code
    if (http_response.status().code != 200) {
      return cs::Error(
          "HTTP " +
          std::to_string(http_response.status().code) +
          " " + http_response.status().name);
    }

    return ResponseType().Parse(http_response.body());
  }

 private:
  std::string base_url_;
};

}  // namespace cs::net::rpc

#endif  // CS_NET_RPC_CLIENT_HH

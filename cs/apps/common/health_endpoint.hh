// cs/apps/common/health_endpoint.hh
#ifndef CS_APPS_COMMON_HEALTH_ENDPOINT_HH
#define CS_APPS_COMMON_HEALTH_ENDPOINT_HH

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"

namespace cs::apps::common {

// Shared handler for /health/ endpoint that returns OK.
// Context is ignored; health check has no dependencies.
template <typename Ctx>
inline cs::net::http::Response GetHealth(
    cs::net::http::Request, Ctx&) {
  return cs::net::http::Response(
      cs::net::http::HTTP_200_OK,
      cs::net::http::kContentTypeTextPlain, "OK");
}

}  // namespace cs::apps::common

#endif  // CS_APPS_COMMON_HEALTH_ENDPOINT_HH

// cs/apps/common/load_endpoint.gpt.hh
#ifndef CS_APPS_COMMON_LOAD_ENDPOINT_GPT_HH
#define CS_APPS_COMMON_LOAD_ENDPOINT_GPT_HH

#include <functional>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/load/balancer/protos/downstream.proto.hh"

namespace cs::apps::common {

// Shared handler for /load endpoints that reports thread
// load. Caller passes a get_load functor (e.g. capturing
// the WebApp) to avoid coupling Request to the app.
template <typename Ctx>
cs::net::http::Response GetLoad(
    cs::net::http::Request request, Ctx&,
    std::function<float()> get_load) {
  [[maybe_unused]]
  cs::net::load::balancer::protos::GetLoadRequest
      request_proto;
  cs::net::load::balancer::protos::GetLoadResponse
      response_proto;
  response_proto.load = get_load();
  return cs::net::http::Response(
      cs::net::http::HTTP_200_OK,
      cs::net::http::kContentTypeApplicationJson,
      response_proto.Serialize());
}

}  // namespace cs::apps::common

#endif  // CS_APPS_COMMON_LOAD_ENDPOINT_GPT_HH

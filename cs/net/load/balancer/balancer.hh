// cs/net/load/balancer/balancer.hh
#ifndef CS_NET_LOAD_BALANCER_BALANCER_HH
#define CS_NET_LOAD_BALANCER_BALANCER_HH

#include <functional>
#include <map>
#include <vector>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/load/balancer/protos/downstream.proto.hh"
#include "cs/result.hh"

namespace cs::net::load::balancer {

// Select the downstream client with the lowest reported
// load.
cs::ResultOr<protos::DownstreamClient> PickLeastLoaded(
    const std::vector<protos::DownstreamClient>& clients);

// Proxy an HTTP request to the provided downstream client.
cs::net::http::Response ProxyToDownstream(
    const cs::net::http::Request& request,
    const protos::DownstreamClient& downstream);

}  // namespace cs::net::load::balancer

#endif  // CS_NET_LOAD_BALANCER_BALANCER_HH

// cs/net/load/balancer/protos/downstream.proto.hh
#ifndef CS_NET_LOAD_BALANCER_PROTOS_DOWNSTREAM_PROTO_HH
#define CS_NET_LOAD_BALANCER_PROTOS_DOWNSTREAM_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::net::load::balancer::protos {

// Downstream client info.
DECLARE_PROTO(DownstreamClient) {
  // Client UUID.
  std::string uuid;
  // Client host.
  std::string host;
  // Client port.
  int port;
  // Client load value.
  float load;
  // Service name (e.g., "www-trycopilot-ai",
  // "code-viewer").
  std::string service_name;
};

// Load fetch request.
DECLARE_PROTO(GetLoadRequest){
    // Empty request for GET endpoint.
};

// Load query response.
DECLARE_PROTO(GetLoadResponse) {
  // Reported load value.
  float load;
};

}  // namespace cs::net::load::balancer::protos

#endif  // CS_NET_LOAD_BALANCER_PROTOS_DOWNSTREAM_PROTO_HH

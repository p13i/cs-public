// cs/net/load/balancer/protos/config.proto.hh
#ifndef CS_NET_LOAD_BALANCER_PROTOS_CONFIG_PROTO_HH
#define CS_NET_LOAD_BALANCER_PROTOS_CONFIG_PROTO_HH

#include <map>
#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::net::load::balancer::protos {

// Routes configuration.
DECLARE_PROTO(Routes) {
  // Domain to service name routing map.
  std::map<std::string, std::string> routes;
};

// Load balancer configuration.
DECLARE_PROTO(Config) {
  // Host address to bind to.
  std::string host;
  // Port number to listen on.
  int port;
  // Environment: "prod" or "dev".
  [[enum_in("prod", "dev")]]
  std::string environment;
  // Production routes.
  Routes prod;
  // Development routes.
  Routes dev;
};

}  // namespace cs::net::load::balancer::protos

#endif  // CS_NET_LOAD_BALANCER_PROTOS_CONFIG_PROTO_HH

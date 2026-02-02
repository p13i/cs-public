// cs/apps/service-registry/protos/config.proto.hh
#ifndef CS_APPS_SERVICE_REGISTRY_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_SERVICE_REGISTRY_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::service_registry::protos {

// Service registry application configuration.
DECLARE_PROTO(Config) {
  // Host address to listen on.
  std::string host;
  // Port number to listen on.
  int port;
};

}  // namespace cs::apps::service_registry::protos

#endif  // CS_APPS_SERVICE_REGISTRY_PROTOS_CONFIG_PROTO_HH

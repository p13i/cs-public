// cs/apps/service-registry/protos/docker.proto.hh
#ifndef CS_APPS_SERVICE_REGISTRY_PROTOS_DOCKER_PROTO_HH
#define CS_APPS_SERVICE_REGISTRY_PROTOS_DOCKER_PROTO_HH

#include <map>
#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::apps::service_registry::protos {

// Docker container basic details.
DECLARE_PROTO(DockerContainerInfo) {
  // Container name.
  std::string Name;
  // Container IPv4 address.
  std::string IPv4Address;
};

// Docker network inspection data.
DECLARE_PROTO(DockerNetworkInspectElement) {
  // Containers on network map.
  std::map<std::string, DockerContainerInfo> Containers;
};

// Docker port binding mapping.
DECLARE_PROTO(DockerPortBinding) { std::string HostPort; };

// Network settings payload.
DECLARE_PROTO(DockerNetworkSettings) {
  // Port bindings map.
  std::map<std::string, std::vector<DockerPortBinding>>
      Ports;
};

// Docker config payload.
DECLARE_PROTO(DockerConfig) {
  // Exposed ports map.
  std::map<std::string, std::string> ExposedPorts;
};

// Docker inspect response subset.
DECLARE_PROTO(DockerInspectElement) {
  // Network settings info.
  DockerNetworkSettings NetworkSettings;
  // Container config info.
  DockerConfig Config;
};

}  // namespace cs::apps::service_registry::protos

#endif  // CS_APPS_SERVICE_REGISTRY_PROTOS_DOCKER_PROTO_HH

// cs/apps/service-registry/protos/service.proto.hh
#ifndef CS_APPS_SERVICE_REGISTRY_PROTOS_SERVICE_PROTO_HH
#define CS_APPS_SERVICE_REGISTRY_PROTOS_SERVICE_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::apps::service_registry::protos {

// Registered service instance record.
DECLARE_PROTO(ServiceEntry) {
  // Service name.
  [[required]]
  std::string name;
  // Container name.
  [[required]]
  std::string container_name;
  // Service IP address.
  [[required]]
  std::string ip_address;
  // Hostname value.
  [[required]]
  std::string host;
  // Service port number.
  [[required]]
  int port;
  // Last update timestamp.
  [[required]] [[iso8601]]
  std::string last_updated;
};

// Service registry snapshot.
DECLARE_PROTO(ServiceRegistry) {
  // Service entries list.
  std::vector<ServiceEntry> services;
  // Docker network name.
  [[required]]
  std::string network_name;
  // Registry updated time.
  [[required]] [[iso8601]]
  std::string last_updated;
  // Key prefix value.
  [[required]]
  std::string prefix;
  // Registry UUID value.
  std::string uuid;
};

// Lookup request payload.
DECLARE_PROTO(LookupServiceRequest) {
  // Target service name.
  [[required]]
  std::string service_name;
};

// Lookup response payload.
DECLARE_PROTO(LookupServiceResponse) {
  // Service found flag.
  bool found;
  // Resolved host.
  std::string host;
  // Resolved IP address.
  std::string ip_address;
  // Resolved port number.
  int port;
  // Resolved container name.
  std::string container_name;
};

}  // namespace cs::apps::service_registry::protos

#endif  // CS_APPS_SERVICE_REGISTRY_PROTOS_SERVICE_PROTO_HH

// cs/apps/service-registry/service_registry_app.gpt.hh
#ifndef CS_APPS_SERVICE_REGISTRY_SERVICE_REGISTRY_APP_GPT_HH
#define CS_APPS_SERVICE_REGISTRY_SERVICE_REGISTRY_APP_GPT_HH

#include <string>

#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/result.hh"

namespace cs::apps::service_registry {

// Run a shell command and capture stdout.
ResultOr<std::string> RunCommand(const std::string& cmd);

// Strip CIDR suffix from an IPv4 address string.
std::string ExtractIPAddress(
    const std::string& ip_with_cidr);

// Inspect a container and determine its exposed port.
ResultOr<std::string> GetContainerPort(
    const std::string& container_id);

// Background loop that refreshes service registry data.
void StartDiscoveryThread(
    const std::string& network_name,
    const std::string& prefix,
    cs::net::proto::db::IDatabaseClient& db);

}  // namespace cs::apps::service_registry

#endif  // CS_APPS_SERVICE_REGISTRY_SERVICE_REGISTRY_APP_GPT_HH

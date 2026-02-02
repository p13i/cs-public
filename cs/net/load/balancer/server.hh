// cs/net/load/balancer/server.hh
#ifndef CS_NET_LOAD_BALANCER_SERVER_HH
#define CS_NET_LOAD_BALANCER_SERVER_HH

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/load/balancer/protos/downstream.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/result.hh"

namespace cs::net::http {
template <typename T>
class WebApp;
}

namespace cs::net::load::balancer {

// Starts an HTTP server on the given host and port.
cs::Result Server(std::string host, int port,
                  std::string environment);

// Discovers downstream clients from service registry.
// Requires non-null db_client for remote queries.
cs::ResultOr<std::vector<protos::DownstreamClient>>
DiscoverDownstreamClients(
    const std::string& environment,
    std::shared_ptr<cs::net::proto::db::IDatabaseClient>
        db_client);

// Refreshes the discovery loop in a background thread.
void RefreshDiscoveryLoop(
    std::string environment,
    std::shared_ptr<cs::net::proto::db::IDatabaseClient>
        db_client);

// Starts the background discovery thread.
void StartDiscoveryThread(
    std::string environment,
    std::shared_ptr<cs::net::proto::db::IDatabaseClient>
        db_client);

// Loads domain-to-service routing for the given
// environment.
cs::ResultOr<std::map<std::string, std::string>>
LoadRoutingConfig(const std::string& environment);

}  // namespace cs::net::load::balancer

#endif  // CS_NET_LOAD_BALANCER_SERVER_HH

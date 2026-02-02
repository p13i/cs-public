// cs/apps/service-registry/rpc.gpt.cc
#include "cs/apps/service-registry/rpc.gpt.hh"

#include <string>
#include <vector>

#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/log.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"

namespace {  // use_usings
using ::cs::apps::service_registry::protos::
    LookupServiceRequest;
using ::cs::apps::service_registry::protos::
    LookupServiceResponse;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::util::Context;
}  // namespace

namespace cs::apps::service_registry::rpc {

IMPLEMENT_RPC(LookupServiceRPC, LookupServiceRequest,
              LookupServiceResponse) {
  LookupServiceResponse response;
  response.found = false;

  if (request.service_name.empty()) {
    return response;
  }

  SET_OR_RET(auto* rpc_ctx, GetContext());
  auto db = rpc_ctx->Get<IDatabaseClient>();
  QueryView<ServiceRegistry> query("service-registry", db);
  SET_OR_RET(auto values, query.values());
  if (values.empty()) {
    LOG(DEBUG) << "No registry entries found in database"
               << ENDL;
    return response;
  }

  ServiceRegistry registry = values[0];
  for (const auto& reg : values) {
    if (reg.last_updated > registry.last_updated) {
      registry = reg;
    }
  }

  // Search for the requested service
  for (const auto& entry : registry.services) {
    if (entry.name == request.service_name) {
      response.found = true;
      response.host = entry.host;
      response.ip_address = entry.ip_address;
      response.port = entry.port;
      response.container_name = entry.container_name;
      LOG(INFO) << "Found service: " << request.service_name
                << " -> " << entry.host << ":"
                << std::to_string(entry.port) << " ("
                << entry.ip_address << ")" << ENDL;
      return response;
    }
  }

  LOG(DEBUG) << "Service not found: "
             << request.service_name << ENDL;
  return response;
}

}  // namespace cs::apps::service_registry::rpc

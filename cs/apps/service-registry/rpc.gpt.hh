// cs/apps/service-registry/rpc.gpt.hh
// cs/apps/service-registry/rpc.gpt.hh
#ifndef CS_APPS_SERVICE_REGISTRY_RPC_GPT_HH
#define CS_APPS_SERVICE_REGISTRY_RPC_GPT_HH

#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::service_registry::rpc {

DECLARE_RPC(LookupServiceRPC,
            cs::apps::service_registry::protos::
                LookupServiceRequest,
            cs::apps::service_registry::protos::
                LookupServiceResponse,
            CTX(cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::service_registry::rpc

#endif  // CS_APPS_SERVICE_REGISTRY_RPC_GPT_HH

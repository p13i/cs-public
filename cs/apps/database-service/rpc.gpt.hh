// cs/apps/database-service/rpc.gpt.hh
#ifndef CS_APPS_DATABASE_SERVICE_RPC_GPT_HH
#define CS_APPS_DATABASE_SERVICE_RPC_GPT_HH

#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::database_service::rpc {

DECLARE_RPC(QueryRPC,
            cs::net::proto::database::QueryRequest,
            cs::net::proto::database::QueryResponse, CTX());

DECLARE_RPC(UpsertRPC,
            cs::net::proto::database::UpsertRequest,
            cs::net::proto::database::UpsertResponse,
            CTX());

}  // namespace cs::apps::database_service::rpc

#endif  // CS_APPS_DATABASE_SERVICE_RPC_GPT_HH

// cs/apps/trycopilot.ai/api/logs.hh
// cs/apps/trycopilot.ai/api/logs.hh
// cs/apps/trycopilot.ai/api/logs.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_LOGS_HH
#define CS_APPS_TRYCOPILOT_AI_API_LOGS_HH

#include "cs/apps/trycopilot.ai/protos/log.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_RPC(
    CreateLogRPC,
    cs::apps::trycopilotai::protos::CreateLogRequest,
    cs::apps::trycopilotai::protos::CreateLogResponse,
    CTX(cs::net::proto::db::DatabaseBaseUrl,
        cs::net::proto::db::IDatabaseClient));

DECLARE_RPC(GetLogRPC,
            cs::apps::trycopilotai::protos::GetLogRequest,
            cs::apps::trycopilotai::protos::GetLogResponse,
            CTX());

DECLARE_RPC(
    ListLogsRPC,
    cs::apps::trycopilotai::protos::ListLogsRequest,
    cs::apps::trycopilotai::protos::ListLogsResponse,
    CTX(cs::net::proto::db::DatabaseBaseUrl,
        cs::net::proto::db::IDatabaseClient));

DECLARE_RPC(
    ListAppLogsRPC,
    cs::apps::trycopilotai::protos::ListAppLogsRequest,
    cs::apps::trycopilotai::protos::ListAppLogsResponse,
    CTX(cs::net::proto::db::DatabaseBaseUrl,
        cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_LOGS_HH
// cs/apps/trycopilot.ai/api/user.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_USER_HH
#define CS_APPS_TRYCOPILOT_AI_API_USER_HH

#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_RPC(
    CreateUserRPC,
    cs::apps::trycopilotai::protos::CreateUserRequest,
    cs::apps::trycopilotai::protos::CreateUserResponse,
    CTX(cs::net::proto::db::DatabaseBaseUrl,
        cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::trycopilotai::api

namespace cs::apps::trycopilotai::rpc {

DECLARE_RPC(
    CreateUserRPC,
    cs::apps::trycopilotai::protos::CreateUserRequest,
    cs::apps::trycopilotai::protos::CreateUserResponse,
    CTX(cs::net::proto::db::DatabaseBaseUrl,
        cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::trycopilotai::rpc

#endif  // CS_APPS_TRYCOPILOT_AI_API_USER_HH
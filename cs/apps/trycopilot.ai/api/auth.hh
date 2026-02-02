// cs/apps/trycopilot.ai/api/auth.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_AUTH_HH
#define CS_APPS_TRYCOPILOT_AI_API_AUTH_HH

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_RPC(LoginRPC,
            cs::apps::trycopilotai::protos::LoginRequest,
            cs::apps::trycopilotai::protos::LoginResponse,
            CTX(cs::net::proto::db::DatabaseBaseUrl,
                cs::net::proto::db::IDatabaseClient));

DECLARE_RPC(LogoutRPC,
            cs::apps::trycopilotai::protos::LogoutRequest,
            cs::apps::trycopilotai::protos::LogoutResponse,
            CTX(cs::net::proto::db::DatabaseBaseUrl,
                cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_AUTH_HH
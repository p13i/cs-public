// cs/apps/trycopilot.ai/api/user.hh
// cs/apps/trycopilot.ai/api/user.hh
// cs/apps/trycopilot.ai/api/user.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_USER_HH
#define CS_APPS_TRYCOPILOT_AI_API_USER_HH

#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/rpc/rpc.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(
    CreateUserAPI,
    cs::apps::trycopilotai::protos::CreateUserRequest,
    cs::apps::trycopilotai::protos::CreateUserResponse);

}

namespace cs::apps::trycopilotai::rpc {

DECLARE_RPC(
    CreateUserRPC,
    cs::apps::trycopilotai::protos::CreateUserRequest,
    cs::apps::trycopilotai::protos::CreateUserResponse);

}  // namespace cs::apps::trycopilotai::rpc

#endif  // CS_APPS_TRYCOPILOT_AI_API_USER_HH
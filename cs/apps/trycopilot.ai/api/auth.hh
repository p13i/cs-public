// cs/apps/trycopilot.ai/api/auth.hh
// cs/apps/trycopilot.ai/api/auth.hh
// cs/apps/trycopilot.ai/api/auth.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_AUTH_HH
#define CS_APPS_TRYCOPILOT_AI_API_AUTH_HH

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/net/proto/api.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(LoginAPI,
            cs::apps::trycopilotai::protos::LoginRequest,
            cs::apps::trycopilotai::protos::LoginResponse);

DECLARE_API(LogoutAPI,
            cs::apps::trycopilotai::protos::LogoutRequest,
            cs::apps::trycopilotai::protos::LogoutResponse);

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_AUTH_HH
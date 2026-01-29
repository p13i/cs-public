// cs/apps/trycopilot.ai/api/game.hh
// cs/apps/trycopilot.ai/api/game.hh
// cs/apps/trycopilot.ai/api/game.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_GAME_HH
#define CS_APPS_TRYCOPILOT_AI_API_GAME_HH

#include "cs/apps/trycopilot.ai/protos/game.proto.hh"
#include "cs/net/proto/api.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(
    GetAnimationAPI,
    cs::apps::trycopilotai::protos::GetAnimationRequest,
    cs::apps::trycopilotai::protos::GetAnimationResponse);

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_GAME_HH
// cs/apps/trycopilot.ai/api/game.hh
// cs/apps/trycopilot.ai/api/game.hh
// cs/apps/trycopilot.ai/api/game.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_GAME_HH
#define CS_APPS_TRYCOPILOT_AI_API_GAME_HH

#include "cs/apps/trycopilot.ai/protos/game.proto.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_RPC(
    GetAnimationRPC,
    cs::apps::trycopilotai::protos::GetAnimationRequest,
    cs::apps::trycopilotai::protos::GetAnimationResponse,
    CTX());

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_GAME_HH
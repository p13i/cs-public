// cs/apps/trycopilot.ai/api/prompt.hh
// cs/apps/trycopilot.ai/api/prompt.hh
// cs/apps/trycopilot.ai/api/prompt.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH
#define CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH

#include "cs/apps/trycopilot.ai/protos/prompt.proto.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_RPC(PromptRPC,
            cs::apps::trycopilotai::protos::PromptRequest,
            cs::apps::trycopilotai::protos::PromptResponse,
            CTX());

DECLARE_RPC(LocalPromptRPC,
            cs::apps::trycopilotai::protos::PromptRequest,
            cs::apps::trycopilotai::protos::PromptResponse,
            CTX());

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH

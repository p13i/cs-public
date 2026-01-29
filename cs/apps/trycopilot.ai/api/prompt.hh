// cs/apps/trycopilot.ai/api/prompt.hh
// cs/apps/trycopilot.ai/api/prompt.hh
// cs/apps/trycopilot.ai/api/prompt.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH
#define CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH

#include "cs/apps/trycopilot.ai/protos/prompt.proto.hh"
#include "cs/net/proto/api.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(PromptAPI,
            cs::apps::trycopilotai::protos::PromptRequest,
            cs::apps::trycopilotai::protos::PromptResponse);

DECLARE_API(LocalPromptAPI,
            cs::apps::trycopilotai::protos::PromptRequest,
            cs::apps::trycopilotai::protos::PromptResponse);

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_PROMPT_HH

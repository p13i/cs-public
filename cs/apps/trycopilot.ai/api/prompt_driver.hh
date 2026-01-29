// cs/apps/trycopilot.ai/api/prompt_driver.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_PROMPT_DRIVER_HH
#define CS_APPS_TRYCOPILOT_AI_API_PROMPT_DRIVER_HH

#include "cs/apps/trycopilot.ai/protos/prompt_driver.proto.hh"
#include "cs/net/proto/api.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(
    PromptDriverAPI,
    cs::apps::trycopilotai::protos::PromptDriverRequest,
    cs::apps::trycopilotai::protos::PromptDriverResponse);

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_PROMPT_DRIVER_HH

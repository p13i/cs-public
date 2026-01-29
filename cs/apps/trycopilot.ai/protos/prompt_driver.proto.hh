// cs/apps/trycopilot.ai/protos/prompt_driver.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_PROMPT_DRIVER_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_PROMPT_DRIVER_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Prompt driver request.
DECLARE_PROTO(PromptDriverRequest) {
  // Prompt message text.
  std::string msg;
};

// Prompt driver response.
DECLARE_PROTO(PromptDriverResponse) {
  // Stdout content.
  std::string stdout_output;
  // Process exit code.
  int exit_code;
  // Success flag.
  bool ok;
  // Error message text.
  std::string error_message;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_PROMPT_DRIVER_PROTO_HH

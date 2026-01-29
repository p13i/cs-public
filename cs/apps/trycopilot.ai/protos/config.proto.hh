// cs/apps/trycopilot.ai/protos/config.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// TryCopilot.ai application configuration.
DECLARE_PROTO(Config) {
  // Host address to bind to.
  std::string host;
  // Port number to listen on.
  int port;
  // Application version.
  std::string version;
  // Git commit hash.
  std::string commit;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_CONFIG_PROTO_HH

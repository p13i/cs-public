// cs/apps/trycopilot.ai/protos/context.proto.hh
// cs/apps/trycopilot.ai/protos/context.proto.hh
// cs/apps/trycopilot.ai/protos/context.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_CONTEXT_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_CONTEXT_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// API route mapping.
DECLARE_PROTO(Route) {
  // HTTP method.
  std::string method;
  // Route path.
  std::string path;
  // Handler function.
  std::string function;
};

// API context routes.
DECLARE_PROTO(Context) {
  // Route list.
  std::vector<Route> routes;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_CONTEXT_PROTO_HH

// cs/apps/trycopilot.ai/protos/acl.proto.hh
// cs/apps/trycopilot.ai/protos/acl.proto.hh
// cs/apps/trycopilot.ai/protos/acl.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_ACL_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_ACL_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Access policy entry.
DECLARE_PROTO(Policy) {
  // Principal identifiers.
  std::vector<std::string> principals;
  // Resource identifiers.
  std::vector<std::string> resources;
  // Allow decision flag.
  bool allow;
};

// ACL definition payload.
DECLARE_PROTO(AccessControlList) {
  // Policy list.
  std::vector<Policy> policies;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_ACL_PROTO_HH
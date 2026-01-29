// cs/apps/trycopilot.ai/protos/uuid.proto.hh
// cs/apps/trycopilot.ai/protos/uuid.proto.hh
// cs/apps/trycopilot.ai/protos/uuid.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_UUID_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_UUID_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// UUID generation request.
DECLARE_PROTO(GetUuidRequest) {
  // Seed value.
  int seed;
};

// UUID generation response.
DECLARE_PROTO(GetUuidResponse) {
  // Generated UUID.
  std::string uuid;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APP_PROTOS_AUTH_PROTO_HH

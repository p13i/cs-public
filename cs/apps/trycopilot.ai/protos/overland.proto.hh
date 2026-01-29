// cs/apps/trycopilot.ai/protos/overland.proto.hh
// cs/apps/trycopilot.ai/protos/overland.proto.hh
// cs/apps/trycopilot.ai/protos/overland.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_OVERLAND_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_OVERLAND_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Geographic position payload.
DECLARE_PROTO(Position) {
  // Latitude degrees.
  float latitude;
  // Longitude degrees.
  float longitude;
  // Altitude meters.
  float altitude;
};

// Timestamp payload.
DECLARE_PROTO(Timestamp) {
  // Unix time microseconds.
  float unix_micros;
};

// User position report.
DECLARE_PROTO(UserPosition) {
  // User UUID.
  std::string user_uuid;
  // Position timestamp.
  Timestamp timestamp;
  // Position value.
  Position position;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_OVERLAND_PROTO_HH

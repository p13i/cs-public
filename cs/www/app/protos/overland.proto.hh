#ifndef CS_WWW_APP_PROTOS_OVERLAND_PROTO_HH
#define CS_WWW_APP_PROTOS_OVERLAND_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Position) {
  float latitude;
  float longitude;
  float altitude;
};

DECLARE_PROTO(Timestamp) { float unix_micros; };

DECLARE_PROTO(UserPosition) {
  std::string user_uuid;
  Timestamp timestamp;
  Position position;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_OVERLAND_PROTO_HH

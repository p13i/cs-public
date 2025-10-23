#ifndef CS_WWW_APP_PROTOS_UUID_PROTO_HH
#define CS_WWW_APP_PROTOS_UUID_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(GetUuidRequest) { int seed; };

DECLARE_PROTO(GetUuidResponse) { std::string uuid; };

}  // namespace cs::www::app::protos

#endif  // CS_APP_PROTOS_AUTH_PROTO_HH

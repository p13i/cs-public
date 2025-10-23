#ifndef CS_WWW_APP_PROTOS_ACL_PROTO_HH
#define CS_WWW_APP_PROTOS_ACL_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Policy) {
  std::vector<std::string> principals;
  std::vector<std::string> resources;
  bool allow;
};

DECLARE_PROTO(AccessControlList) {
  std::vector<Policy> policies;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_ACL_PROTO_HH
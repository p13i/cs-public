#ifndef CS_WWW_APP_PROTOS_CONTEXT_PROTO_HH
#define CS_WWW_APP_PROTOS_CONTEXT_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Route) {
  std::string method;
  std::string path;
  std::string function;
};

DECLARE_PROTO(Context) { std::vector<Route> routes; };

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_CONTEXT_PROTO_HH

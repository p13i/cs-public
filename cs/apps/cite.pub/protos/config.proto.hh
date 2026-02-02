// cs/apps/cite.pub/protos/config.proto.hh
#ifndef CS_APPS_CITE_PUB_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_CITE_PUB_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::citepub::protos {

// Configuration for cite.pub service.
DECLARE_PROTO(Config) {
  // Host to bind to.
  std::string host;
  // Port to listen on.
  int port;
};

}  // namespace cs::apps::citepub::protos

#endif  // CS_APPS_CITE_PUB_PROTOS_CONFIG_PROTO_HH

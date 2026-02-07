// cs/apps/blob-service/protos/config.proto.hh
#ifndef CS_APPS_BLOB_SERVICE_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_BLOB_SERVICE_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::blob_service::protos {

// Blob service application configuration.
DECLARE_PROTO(Config) {
  // Host to bind to.
  [[required]] [[host]]
  std::string host;
  // Port number to listen on.
  [[required]] [[port]]
  int port;
};

}  // namespace cs::apps::blob_service::protos

#endif  // CS_APPS_BLOB_SERVICE_PROTOS_CONFIG_PROTO_HH

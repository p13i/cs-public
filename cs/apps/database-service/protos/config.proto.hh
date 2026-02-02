// cs/apps/database-service/protos/config.proto.hh
#ifndef CS_APPS_DATABASE_SERVICE_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_DATABASE_SERVICE_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::database_service::protos {

// Database service application configuration.
DECLARE_PROTO(Config) {
  // Host to bind to.
  std::string host;
  // Path to data directory.
  std::string data_dir;
  // Port number to listen on.
  int port;
};

}  // namespace cs::apps::database_service::protos

#endif  // CS_APPS_DATABASE_SERVICE_PROTOS_CONFIG_PROTO_HH

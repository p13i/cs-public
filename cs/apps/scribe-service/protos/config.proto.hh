// cs/apps/scribe-service/protos/config.proto.hh
#ifndef CS_APPS_SCRIBE_SERVICE_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_SCRIBE_SERVICE_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::scribe_service::protos {

DECLARE_PROTO(Config) {
  [[required]]
  std::string host;
  [[required]]
  int port;
  std::string blob_base_url;
  std::string database_base_url;
  std::string task_queue_base_url;
};

}  // namespace cs::apps::scribe_service::protos

#endif  // CS_APPS_SCRIBE_SERVICE_PROTOS_CONFIG_PROTO_HH

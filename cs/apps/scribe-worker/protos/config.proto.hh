// cs/apps/scribe-worker/protos/config.proto.hh
#ifndef CS_APPS_SCRIBE_WORKER_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_SCRIBE_WORKER_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::scribe_worker::protos {

DECLARE_PROTO(Config) {
  [[required]]
  std::string broker_base_url;
  [[required]]
  std::string blob_base_url;
  [[required]]
  std::string database_base_url;
  [[required]]
  std::string workspace_dir;
  [[required]]
  std::string docker_socket;
  std::string hf_token;
  std::string whisper_image;
  int batch_size;
};

}  // namespace cs::apps::scribe_worker::protos

#endif  // CS_APPS_SCRIBE_WORKER_PROTOS_CONFIG_PROTO_HH

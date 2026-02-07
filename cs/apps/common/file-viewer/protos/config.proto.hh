// cs/apps/common/file-viewer/protos/config.proto.hh
#ifndef CS_APPS_COMMON_FILE_VIEWER_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_COMMON_FILE_VIEWER_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::common::file_viewer::protos {

// File viewer application configuration (shared by
// data-viewer, code-viewer, blob-viewer).
DECLARE_PROTO(Config) {
  // Host to bind to.
  [[required]] [[host]]
  std::string host;
  // Port number to listen on.
  [[required]] [[port]]
  int port;
  // Application version.
  std::string version;
  // Git commit hash.
  std::string commit;
};

}  // namespace cs::apps::common::file_viewer::protos

#endif  // CS_APPS_COMMON_FILE_VIEWER_PROTOS_CONFIG_PROTO_HH

// cs/apps/code-viewer/protos/config.proto.hh
#ifndef CS_APPS_CODE_VIEWER_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_CODE_VIEWER_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::code_viewer::protos {

// Code viewer application configuration.
DECLARE_PROTO(Config) {
  // Host to bind to.
  std::string host;
  // Port number to listen on.
  int port;
  // Application version.
  std::string version;
  // Git commit hash.
  std::string commit;
};

}  // namespace cs::apps::code_viewer::protos

#endif  // CS_APPS_CODE_VIEWER_PROTOS_CONFIG_PROTO_HH

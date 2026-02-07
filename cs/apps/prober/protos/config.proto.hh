// cs/apps/prober/protos/config.proto.hh
#ifndef CS_APPS_PROBER_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_PROBER_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::prober::protos {

// Prober application configuration.
DECLARE_PROTO(Config) {
  // Destination host to probe.
  [[required]] [[host]]
  std::string host;
  // Destination port.
  [[required]] [[port]]
  int port;
  // Path to the probes.json file.
  [[required]]
  std::string probes_file;
};

}  // namespace cs::apps::prober::protos

#endif  // CS_APPS_PROBER_PROTOS_CONFIG_PROTO_HH

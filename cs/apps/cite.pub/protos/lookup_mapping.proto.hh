// cs/apps/cite.pub/protos/lookup_mapping.proto.hh
#ifndef CS_APPS_CITE_PUB_PROTOS_LOOKUP_MAPPING_PROTO_HH
#define CS_APPS_CITE_PUB_PROTOS_LOOKUP_MAPPING_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::citepub::protos {

// Lookup mapping for short codes to URLs.
DECLARE_PROTO(LookupMapping) {
  // UUID identifier.
  [[required]]
  std::string uuid;
  // Short code identifier.
  [[required]]
  std::string short_code;
  // Original URL.
  [[required]]
  std::string original_url;
  // Creation timestamp.
  [[required]] [[iso8601]]
  std::string created_at;
  // HTML content.
  std::string content;
};

}  // namespace cs::apps::citepub::protos

#endif  // CS_APPS_CITE_PUB_PROTOS_LOOKUP_MAPPING_PROTO_HH

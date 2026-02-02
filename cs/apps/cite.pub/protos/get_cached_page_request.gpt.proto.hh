// cs/apps/cite.pub/protos/get_cached_page_request.gpt.proto.hh
#ifndef CS_APPS_CITE_PUB_PROTOS_GET_CACHED_PAGE_REQUEST_GPT_PROTO_HH
#define CS_APPS_CITE_PUB_PROTOS_GET_CACHED_PAGE_REQUEST_GPT_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::citepub::protos {

// Request to get a cached page by short code.
DECLARE_PROTO(GetCachedPageRequest) {
  // Short code identifier.
  [[required]]
  std::string short_code;
};

}  // namespace cs::apps::citepub::protos

#endif  // CS_APPS_CITE_PUB_PROTOS_GET_CACHED_PAGE_REQUEST_GPT_PROTO_HH

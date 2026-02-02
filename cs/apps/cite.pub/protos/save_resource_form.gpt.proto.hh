// cs/apps/cite.pub/protos/save_resource_form.gpt.proto.hh
#ifndef CS_APPS_CITE_PUB_PROTOS_SAVE_RESOURCE_FORM_GPT_PROTO_HH
#define CS_APPS_CITE_PUB_PROTOS_SAVE_RESOURCE_FORM_GPT_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::citepub::protos {

// Save page form data.
DECLARE_PROTO(SaveResourceForm) {
  // Web page URL to save.
  [[required]]
  std::string url;
};

}  // namespace cs::apps::citepub::protos

#endif  // CS_APPS_CITE_PUB_PROTOS_SAVE_RESOURCE_FORM_GPT_PROTO_HH

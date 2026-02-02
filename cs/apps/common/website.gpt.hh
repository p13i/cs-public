// cs/apps/common/website.gpt.hh
#ifndef CS_APPS_COMMON_WEBSITE_GPT_HH
#define CS_APPS_COMMON_WEBSITE_GPT_HH

#include <string>
#include <vector>

#include "cs/apps/common/protos/website.proto.hh"

namespace cs::apps::common::website {

using ::cs::apps::common::protos::NavLink;
using ::cs::apps::common::protos::WebsiteConfig;
using ::cs::apps::common::protos::WebsiteUser;

// Renders the shared website shell (head, header, footer)
// around provided HTML content.
std::string MakeWebsite(const WebsiteConfig& config);

// Renders the trycopilot.ai hero section (diagonal stripe,
// title, subtitle).
std::string Hero();

}  // namespace cs::apps::common::website

#endif  // CS_APPS_COMMON_WEBSITE_GPT_HH

// cs/apps/common/website_nav.gpt.hh
// Shared helpers for building website navigation and
// configs.
#ifndef CS_APPS_COMMON_WEBSITE_NAV_GPT_HH
#define CS_APPS_COMMON_WEBSITE_NAV_GPT_HH

#include <string>
#include <vector>

#include "cs/apps/common/protos/website.proto.hh"
#include "cs/apps/common/website.gpt.hh"

namespace cs::apps::common::website {

using ::cs::apps::common::protos::NavItemSpec;
using ::cs::apps::common::protos::PageOptions;
using ::cs::apps::common::protos::WebsiteUserInfo;

std::vector<NavLink> BuildNavLinks(
    const std::string& nav_base,
    const std::vector<NavItemSpec>& nav_specs,
    bool logged_in, bool is_admin);

WebsiteConfig BuildWebsiteConfig(const PageOptions& opts);

}  // namespace cs::apps::common::website

#endif  // CS_APPS_COMMON_WEBSITE_NAV_GPT_HH

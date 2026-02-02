// cs/apps/common/website_nav.gpt.cc
// Shared helpers for building website navigation and
// configs.
#include "cs/apps/common/website_nav.gpt.hh"

#include <utility>

#include "cs/log.hh"

namespace cs::apps::common::website {
namespace {

static std::string NavHref(const std::string& nav_base,
                           const std::string& href) {
  if (href.empty() || href.rfind("http", 0) == 0 ||
      href.rfind("//", 0) == 0 || nav_base.empty() ||
      href.front() != '/') {
    return href;
  }
  if (nav_base.back() == '/') {
    return nav_base + href.substr(1);
  }
  return nav_base + href;
}

}  // namespace

std::vector<NavLink> BuildNavLinks(
    const std::string& nav_base,
    const std::vector<NavItemSpec>& nav_specs,
    bool logged_in, bool is_admin) {
  std::vector<NavLink> links;
  links.reserve(nav_specs.size());
  LOG(INFO) << "[DEBUG] BuildNavLinks - nav_specs.size()="
            << nav_specs.size()
            << " logged_in=" << logged_in
            << " is_admin=" << is_admin << ENDL;
  for (const auto& spec : nav_specs) {
    LOG(INFO) << "[DEBUG] Item: " << spec.label
              << " auth=" << spec.requires_auth
              << " admin=" << spec.requires_admin
              << " logged_out=" << spec.requires_logged_out
              << ENDL;
    if (spec.requires_logged_out && logged_in) {
      LOG(INFO) << "[DEBUG] Skipping " << spec.label
                << " (requires logged out)" << ENDL;
      continue;
    }
    if (spec.requires_auth && !logged_in) {
      LOG(INFO) << "[DEBUG] Skipping " << spec.label
                << " (requires auth)" << ENDL;
      continue;
    }
    if (spec.requires_admin && !is_admin) {
      LOG(INFO) << "[DEBUG] Skipping " << spec.label
                << " (requires admin)" << ENDL;
      continue;
    }

    NavLink link;
    link.href = NavHref(nav_base, spec.href);
    link.label = spec.label;
    LOG(INFO) << "[DEBUG] Adding link: " << link.label
              << " -> " << link.href << ENDL;
    links.push_back(std::move(link));
  }
  LOG(INFO) << "[DEBUG] BuildNavLinks - returning "
            << links.size() << " links" << ENDL;
  return links;
}

WebsiteConfig BuildWebsiteConfig(const PageOptions& opts) {
  WebsiteConfig config{};

  if (opts.user.logged_in) {
    config.has_user = true;
    config.user.email = opts.user.email;
    config.user.admin = opts.user.admin;
  }

  const bool is_logged_in = opts.user.logged_in;
  const bool is_admin = opts.user.admin;
  config.nav_links =
      BuildNavLinks(opts.nav_base, opts.nav_items,
                    is_logged_in, is_admin);
  config.active_path = opts.active_path;
  config.content_html = opts.content_html;
  config.title = opts.title;
  if (opts.include_login_cta && !is_logged_in) {
    config.has_login_cta_href = true;
    config.login_cta_href = opts.login_cta_href;
    config.has_login_cta_label = true;
    config.login_cta_label = opts.login_cta_label;
  }

  return config;
}

}  // namespace cs::apps::common::website

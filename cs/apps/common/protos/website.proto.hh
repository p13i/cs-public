// cs/apps/common/protos/website.proto.hh
#ifndef CS_APPS_COMMON_PROTOS_WEBSITE_PROTO_HH
#define CS_APPS_COMMON_PROTOS_WEBSITE_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::apps::common::protos {

// Website user identity.
DECLARE_PROTO(WebsiteUser) {
  // User email.
  std::string email;
  // Admin flag.
  bool admin;
};

// Navigation link entry.
DECLARE_PROTO(NavLink) {
  // Link href.
  std::string href;
  // Link label.
  std::string label;
};

// Navigation item specification.
DECLARE_PROTO(NavItemSpec) {
  // Item href.
  std::string href;
  // Item label.
  std::string label;
  // Requires authentication.
  bool requires_auth;
  // Requires admin.
  bool requires_admin;
  // Requires logged out.
  bool requires_logged_out;
};

// Website user information.
DECLARE_PROTO(WebsiteUserInfo) {
  // Logged in flag.
  bool logged_in;
  // User email.
  std::string email;
  // Admin flag.
  bool admin;
};

// Website rendering configuration.
DECLARE_PROTO(WebsiteConfig) {
  // Whether user exists flag.
  bool has_user;
  // Website user data.
  WebsiteUser user;
  // Navigation links list.
  std::vector<NavLink> nav_links;
  // Active path.
  std::string active_path;
  // Content HTML string.
  std::string content_html;
  // Page title.
  std::string title;
  // Has nav base href flag.
  bool has_nav_base_href;
  // Nav base href value.
  std::string nav_base_href;
  // Has login CTA href flag.
  bool has_login_cta_href;
  // Login CTA href.
  std::string login_cta_href;
  // Has login CTA label flag.
  bool has_login_cta_label;
  // Login CTA label.
  std::string login_cta_label;
};

// Page options for building website config.
DECLARE_PROTO(PageOptions) {
  // Navigation base path.
  std::string nav_base;
  // Navigation items.
  std::vector<NavItemSpec> nav_items;
  // User information.
  WebsiteUserInfo user;
  // Active path.
  std::string active_path;
  // Page title.
  std::string title;
  // Content HTML.
  std::string content_html;
  // Include login CTA flag.
  bool include_login_cta;
  // Login CTA href.
  std::string login_cta_href;
  // Login CTA label.
  std::string login_cta_label;
};

}  // namespace cs::apps::common::protos

#endif  // CS_APPS_COMMON_PROTOS_WEBSITE_PROTO_HH

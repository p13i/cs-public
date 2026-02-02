// cs/net/html/bootstrap/dsl.hh
#ifndef CS_NET_HTML_BOOTSTRAP_DSL_HH
#define CS_NET_HTML_BOOTSTRAP_DSL_HH

#include <string>
#include <utility>
#include <vector>

namespace cs::net::html::bootstrap {

struct DropdownItem {
  std::string href;
  std::string label;
};

struct NavbarConfig {
  std::string brand_href;
  std::string brand_label;
  std::string expand_breakpoint;
  std::string collapse_id;
  std::vector<std::pair<std::string, std::string>> links;
  std::string active_path;
};

std::string StylesheetLink();

std::string IconsStylesheetLink();

std::string ScriptTag();

std::string Container(const std::string& variant,
                      const std::string& children);

std::string Container(const std::string& children);

std::string Row(const std::string& children,
                const std::string& gutter = "g-3");

std::string Col(const std::string& classes,
                const std::string& content);

std::string Col(int span, const std::string& content);

std::string Navbar(const NavbarConfig& config);

std::string Footer(const std::string& content);

std::string Dropdown(
    const std::string& toggle_label,
    const std::string& dropdown_id,
    const std::vector<DropdownItem>& items);

std::string WithClass(const std::string& existing,
                      const std::string& to_add);

// Bootstrap alert (variant: "primary", "secondary",
// "success", "danger", "warning", "info", "light", "dark").
std::string Alert(const std::string& variant,
                  const std::string& content);

}  // namespace cs::net::html::bootstrap

#endif  // CS_NET_HTML_BOOTSTRAP_DSL_HH

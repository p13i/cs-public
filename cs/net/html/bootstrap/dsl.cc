// cs/net/html/bootstrap/dsl.cc
#include "cs/net/html/bootstrap/dsl.hh"

#include <sstream>

#include "cs/net/html/dom.hh"

namespace cs::net::html::bootstrap {

namespace {

constexpr char kBootstrapCssUrl[] =
    "https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/"
    "bootstrap.min.css";
constexpr char kBootstrapJsUrl[] =
    "https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/"
    "bootstrap.bundle.min.js";
constexpr char kBootstrapIconsCssUrl[] =
    "https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/"
    "font/"
    "bootstrap-icons.min.css";

using namespace ::cs::net::html::dom;

}  // namespace

std::string StylesheetLink() {
  return link({{"href", kBootstrapCssUrl},
               {"rel", "stylesheet"},
               {"crossorigin", "anonymous"}});
}

std::string IconsStylesheetLink() {
  return link({{"href", kBootstrapIconsCssUrl},
               {"rel", "stylesheet"},
               {"crossorigin", "anonymous"}});
}

std::string ScriptTag() {
  return script({{"src", kBootstrapJsUrl},
                 {"crossorigin", "anonymous"}});
}

std::string Container(const std::string& variant,
                      const std::string& children) {
  return div({{"class", variant}}, children);
}

std::string Container(const std::string& children) {
  return Container("container", children);
}

std::string Row(const std::string& children,
                const std::string& gutter) {
  return div({{"class", "row " + gutter}}, children);
}

std::string Col(const std::string& classes,
                const std::string& content) {
  return div({{"class", classes}}, content);
}

std::string Col(int span, const std::string& content) {
  return Col("col-" + std::to_string(span), content);
}

std::string WithClass(const std::string& existing,
                      const std::string& to_add) {
  if (existing.empty()) {
    return to_add;
  }
  return existing + " " + to_add;
}

std::string Navbar(const NavbarConfig& config) {
  const std::string nav_class =
      "navbar navbar-expand-" + config.expand_breakpoint;

  std::string brand =
      a({{"class", "navbar-brand"}}, config.brand_href,
        config.brand_label);

  std::string toggler =
      button({{"class", "navbar-toggler"},
              {"type", "button"},
              {"data-bs-toggle", "collapse"},
              {"data-bs-target", "#" + config.collapse_id},
              {"aria-controls", config.collapse_id},
              {"aria-expanded", "false"},
              {"aria-label", "Toggle navigation"}},
             span({{"class", "navbar-toggler-icon"}}, ""));

  std::ostringstream nav_items;
  for (const auto& [href, label] : config.links) {
    std::string link_class = "nav-link";
    if (href == config.active_path) {
      link_class = WithClass(link_class, "active");
    }
    nav_items << li(
        {{"class", "nav-item"}},
        a({{"class", link_class}, {"href", href}}, href,
          label));
  }

  std::string collapse_div =
      div({{"class", "collapse navbar-collapse"},
           {"id", config.collapse_id}},
          ul({{"class", "navbar-nav ms-auto"}},
             nav_items.str()));

  std::string inner =
      div({{"class", "container-md"},
           {"style",
            "max-width: 720px; margin-left: auto; "
            "margin-right: auto;"}},
          brand + toggler + collapse_div);

  return nav({{"class", nav_class}}, inner);
}

std::string Footer(const std::string& content) {
  return div({{"class", "container-md mt-auto py-3"},
              {"style",
               "max-width: 720px; margin-left: auto; "
               "margin-right: auto;"}},
             content);
}

std::string Dropdown(
    const std::string& toggle_label,
    const std::string& dropdown_id,
    const std::vector<DropdownItem>& items) {
  std::ostringstream menu_items;
  for (const auto& item : items) {
    menu_items << li(a({{"class", "dropdown-item"}},
                       item.href, item.label));
  }

  std::string toggle =
      a({{"class", "nav-link dropdown-toggle"},
         {"role", "button"},
         {"data-bs-toggle", "dropdown"},
         {"aria-expanded", "false"}},
        "#", toggle_label);

  std::string menu =
      ul({{"class", "dropdown-menu"}}, menu_items.str());

  return li({{"class", "nav-item dropdown"}},
            toggle + menu);
}

std::string Alert(const std::string& variant,
                  const std::string& content) {
  std::string inner = content;
  if (variant == "warning") {
    std::string icon = span(
        {{"class", "bi bi-exclamation-triangle me-3"}}, "");
    std::string content_wrapper =
        div({{"class", "flex-grow-1 min-w-0"}}, content);
    inner = div({{"class", "d-flex align-items-center"}},
                icon + content_wrapper);
  }
  return div({{"class", "alert alert-" + variant}}, inner);
}

}  // namespace cs::net::html::bootstrap

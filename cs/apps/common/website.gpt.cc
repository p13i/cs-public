// cs/apps/common/website.gpt.cc
#include "cs/apps/common/website.gpt.hh"

#include <sstream>
#include <utility>
#include <vector>

#include "cs/apps/common/protos/website.proto.hh"
#include "cs/net/html/bootstrap/dsl.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/html/minify.hh"
#include "cs/util/context.hh"

namespace {  // use_usings
using ::cs::apps::common::protos::WebsiteConfig;
using ::cs::net::html::dom::a;
using ::cs::net::html::dom::body;
using ::cs::net::html::dom::code;
using ::cs::net::html::dom::div;
using ::cs::net::html::dom::head;
using ::cs::net::html::dom::html;
using ::cs::net::html::dom::meta;
using ::cs::net::html::dom::p;
using ::cs::net::html::dom::span;
using ::cs::net::html::dom::title;
using ::cs::util::Context;
}  // namespace

namespace {
namespace bootstrap = ::cs::net::html::bootstrap;

std::string NormalizeBase(const WebsiteConfig& config) {
  if (!config.has_nav_base_href ||
      config.nav_base_href.empty()) {
    return "";
  }
  std::string out = config.nav_base_href;
  if (out.back() == '/') {
    return out.substr(0, out.size() - 1);
  }
  return out;
}

std::string WithBase(const WebsiteConfig& config,
                     const std::string& href) {
  if (href.empty() || href.rfind("http", 0) == 0 ||
      href.rfind("//", 0) == 0) {
    return href;
  }
  const std::string norm_base = NormalizeBase(config);
  if (norm_base.empty() || href.front() != '/') {
    return href;
  }
  return norm_base + href;
}

}  // namespace

namespace cs::apps::common::website {

static std::string MakeHeader(const WebsiteConfig& config) {
  bootstrap::NavbarConfig navbar_config;
  navbar_config.brand_href = WithBase(config, "/");
  navbar_config.brand_label =
      config.title.empty() ? "COPILOT AI" : config.title;
  navbar_config.expand_breakpoint = "lg";
  navbar_config.collapse_id = "navbarMain";
  navbar_config.active_path = config.active_path;

  for (const auto& link : config.nav_links) {
    const std::string href = WithBase(config, link.href);
    navbar_config.links.emplace_back(href, link.label);
  }

  return bootstrap::Navbar(navbar_config);
}

static std::string MakeFooter() {
  using namespace ::cs::net::html::dom;
  std::ostringstream ss;
  ss << code(cs::util::Context::Read("VERSION").value_or(
            "v0"))
     << nbsp() << "("
     << code(cs::util::Context::Read("COMMIT").value_or(
            "commit"))
     << ")" << nbsp() << copy() << nbsp() << "2025"
     << nbsp() << a("https://p13i.io/tweets/", "@p13i.io")
     << span({{"id", "load-balancer-here"}}, "");
  return bootstrap::Footer(ss.str());
}

std::string Hero() {
  using namespace ::cs::net::html::dom;

  // Diagonal stripe background
  std::string diagonal =
      div({{"style",
            "position: absolute; background: #000000; "
            "transform-origin: bottom left; "
            "width: 200vmax; height: 350px; "
            "bottom: 0; left: 0; "
            "transform: rotate(-45deg) translateY(50%); "
            "border-bottom: 50px solid #9333EA; "
            "box-shadow: 0px 0px 20px 0px white;"}},
          "");

  // Background layer
  std::string bg_layer = div(
      {{"class", "position-absolute w-100 h-100"},
       {"style", "background-color: black; z-index: 0;"}},
      diagonal);

  // Title
  std::string title_el = h1(
      {{"class", "fw-bold text-white text-uppercase mb-3"},
       {"style",
        "font-size: 1.5rem; letter-spacing: 0.3em; "
        "text-shadow: 0px 0px 2px rgba(0, 0, 0, 0.8);"}},
      "trycopilot.ai");

  // Subtitle with accent
  std::string subtitle_el =
      p({{"class", "text-uppercase mb-0"},
         {"style",
          "font-size: 0.8rem; letter-spacing: 0.1em; "
          "color: rgba(255,255,255,0.7); "
          "max-width: min(90vw, 32rem); "
          "text-shadow: 0px 0px 2px rgba(0, 0, 0, 0.8);"}},
        "augmented reality for" + nbsp() +
            span({{"style", "color: #9333EA;"}},
                 "autonomous") +
            nbsp() + "agents");

  // Content layer
  std::string content_layer =
      div({{"class",
            "position-relative d-flex flex-column "
            "justify-content-center align-items-start "
            "h-100 px-4 px-md-5"},
           {"style", "z-index: 1; min-height: 75vh;"}},
          title_el + subtitle_el);

  // Outer section
  return div(
      {{"class", "position-relative overflow-hidden"},
       {"style",
        "min-height: 75vh; pointer-events: none;"}},
      bg_layer + content_layer);
}

std::string MakeWebsite(const WebsiteConfig& config) {
  using namespace ::cs::net::html::dom;

  std::string title_value;
  if (config.title.empty()) {
    title_value = "COPILOT AI";
  } else {
    title_value = config.title;
  }

  return cs::net::html::minify::MinifyHTML(html(
      {{"lang", "en"}},
      head(
          meta({{"charset", "utf-8"}}),
          meta({{"http-equiv", "Content-Type"},
                {"content", "text/html; charset=utf-8"}}),
          R"html(<meta name="viewport" content="width=device-width, initial-scale=1">)html",
          title(title_value), bootstrap::StylesheetLink(),
          bootstrap::IconsStylesheetLink()),
      body(MakeHeader(config),
           div({{"class", "container-md"},
                {"style",
                 "max-width: 720px; margin-left: auto; "
                 "margin-right: auto;"}},
               config.content_html),
           MakeFooter(), bootstrap::ScriptTag())));
}

}  // namespace cs::apps::common::website

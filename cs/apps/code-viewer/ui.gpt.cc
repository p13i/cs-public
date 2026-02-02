// cs/apps/code-viewer/ui.gpt.cc
#include "cs/apps/code-viewer/ui.gpt.hh"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "cs/apps/common/domain_resolver.gpt.hh"
#include "cs/apps/common/load_endpoint.gpt.hh"
#include "cs/apps/common/website.gpt.hh"
#include "cs/apps/common/website_nav.gpt.hh"
#include "cs/log.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::PermissionDenied;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::common::GetDomainForService;
using ::cs::apps::common::website::BuildWebsiteConfig;
using ::cs::apps::common::website::MakeWebsite;
using ::cs::apps::common::website::NavItemSpec;
using ::cs::apps::common::website::NavLink;
using ::cs::apps::common::website::PageOptions;
using ::cs::apps::common::website::WebsiteConfig;
using ::cs::apps::common::website::WebsiteUserInfo;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::util::di::Context;
}  // namespace

namespace {  // impl
static constexpr const char kKatexScripts[] = R"html(
<link rel="stylesheet" href="https://p13i.io/static/katex-v0.11.1/katex.css">
<script defer src="https://p13i.io/static/katex-v0.11.1/katex.js"></script>
<script defer src="https://p13i.io/static/katex-v0.11.1/contrib/auto-render.js"></script>
<script>
document.addEventListener('DOMContentLoaded', function() {
  if (window.renderMathInElement) {
    renderMathInElement(document.body, {
      delimiters: [
        {left: "$$", right: "$$", display: true},
        {left: "$", right: "$", display: false},
        {left: "\\\\(", right: "\\\\)", display: false},
        {left: "\\\\[", right: "\\\\]", display: true}
      ]
    });
  }
});
</script>
)html";

static std::string EscapeForHtml(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    switch (c) {
      case '&':
        out += "&amp;";
        break;
      case '<':
        out += "&lt;";
        break;
      case '>':
        out += "&gt;";
        break;
      case '"':
        out += "&quot;";
        break;
      case '\'':
        out += "&#39;";
        break;
      default:
        out += c;
        break;
    }
  }
  return out;
}

static std::string UrlEncode(const std::string& s) {
  std::ostringstream oss;
  oss << std::hex;
  for (unsigned char c : s) {
    if (std::isalnum(c) || c == '-' || c == '.' ||
        c == '_' || c == '~' || c == '/') {
      oss << c;
    } else {
      oss << '%' << std::uppercase << std::setw(2)
          << std::setfill('0') << int(c)
          << std::nouppercase;
      oss << std::dec;
    }
  }
  return oss.str();
}

static bool IsSubpath(const std::filesystem::path& base,
                      const std::filesystem::path& child) {
  auto bi = base.begin(), be = base.end();
  auto ci = child.begin(), ce = child.end();
  for (; bi != be && ci != ce; ++bi, ++ci) {
    if (*bi != *ci) return false;
  }
  return bi == be;
}

static std::string NormalizeBasePath(
    const std::string& base_path) {
  if (base_path.empty()) return "/";
  if (base_path.back() == '/') return base_path;
  return base_path + "/";
}

static std::string BuildCodeHref(
    const std::string& base_path,
    const std::string& relative) {
  std::string norm_base = NormalizeBasePath(base_path);
  std::string rel = relative;
  if (!rel.empty() && rel.front() == '/')
    rel.erase(rel.begin());
  if (norm_base == "/") {
    return "/" + rel;
  }
  return norm_base + rel;
}

static std::string MakeBreadcrumbs(
    const std::string& rel_path,
    const std::string& base_path) {
  using namespace ::cs::net::html::dom;
  std::ostringstream html;
  std::filesystem::path p =
      std::filesystem::path(rel_path).lexically_normal();

  std::string accum;
  for (auto it = p.begin(); it != p.end(); ++it) {
    accum = (std::filesystem::path(accum) / *it)
                .lexically_normal()
                .string();
    const std::string sep = code("&nbsp;/&nbsp;");
    const std::string link =
        a({}, BuildCodeHref(base_path, UrlEncode(accum)),
          code(EscapeForHtml(it->string())));
    html << sep << link;
  }

  if (p.empty()) {
    html << code("&nbsp;/");
  }

  return nav({{"class", "breadcrumbs"}}, html.str());
}

static ResultOr<std::string> MakePage(
    const std::string& title, const std::string& body_html,
    const Request& request) {
  std::vector<NavItemSpec> nav_items;
  {
    NavItemSpec item;
    SET_OR_RET(
        std::string index_domain,
        GetDomainForService("www-trycopilot-ai", request));
    item.href = "https://" + index_domain + "/";
    item.label = "Index";
    nav_items.push_back(item);
  }
  std::string code_domain;
  {
    NavItemSpec item;
    SET_OR_RET(code_domain,
               GetDomainForService("code-viewer", request));
    item.href = "https://" + code_domain + "/";
    item.label = "Code (cs)";
    nav_items.push_back(item);
  }

  PageOptions opts{};
  opts.nav_base = "";
  opts.nav_items = std::move(nav_items);
  {
    WebsiteUserInfo user_info{};
    user_info.logged_in = false;
    user_info.email = "";
    user_info.admin = false;
    opts.user = user_info;
  }
  SET_OR_RET(std::string current_host,
             request.NormalizedHost());
  if (current_host == code_domain) {
    opts.active_path = "https://" + code_domain + "/";
  } else {
    opts.active_path = "";
  }
  opts.content_html =
      std::string(kKatexScripts) + body_html;
  opts.title = EscapeForHtml(title);
  WebsiteConfig config = BuildWebsiteConfig(opts);
  return MakeWebsite(config);
}

static ResultOr<std::string> RenderHtmlForSourceCodePath(
    const std::string& relative_path,
    const std::filesystem::path& base_dir) {
  std::ostringstream html;
  const std::string normalized_base = "/";

  std::filesystem::path base =
      std::filesystem::absolute(base_dir);
  std::filesystem::path target =
      base / std::filesystem::path(relative_path);

  std::error_code ec;
  std::filesystem::path base_canon =
      std::filesystem::weakly_canonical(base, ec);
  if (ec) base_canon = std::filesystem::absolute(base);

  std::filesystem::path target_canon =
      std::filesystem::weakly_canonical(target, ec);
  if (ec)
    target_canon = std::filesystem::absolute(target)
                       .lexically_normal();

  if (!IsSubpath(base_canon, target_canon)) {
    return TRACE(PermissionDenied(
        "Access denied: Requested path is outside of the "
        "permitted folder."));
  }

  if (!std::filesystem::exists(target_canon)) {
    return TRACE(NotFoundError("Path does not exist: " +
                               relative_path));
  }

  using namespace ::cs::net::html::dom;

  if (std::filesystem::is_directory(target_canon)) {
    std::string breadcrumb_target = relative_path;
    if (breadcrumb_target.empty()) {
      breadcrumb_target = ".";
    }
    html << MakeBreadcrumbs(breadcrumb_target,
                            normalized_base)
         << hr();

    std::filesystem::path rel_p(relative_path);
    if (!rel_p.empty() && rel_p != ".") {
      std::filesystem::path parent = rel_p.parent_path();
      std::string parent_str = parent.string();
      parent_str = BuildCodeHref(normalized_base,
                                 UrlEncode(parent_str));
      html << div({{"class", "entry"}},
                  a(parent_str, "[.. parent]"))
           << hr();
    }

    std::vector<std::filesystem::directory_entry> entries;
    for (auto& de :
         std::filesystem::directory_iterator(target_canon))
      entries.push_back(de);
    std::sort(entries.begin(), entries.end(),
              [](auto& a, auto& b) {
                return a.path().filename().string() <
                       b.path().filename().string();
              });

    std::ostringstream list_ss;
    for (auto& de : entries) {
      std::string name = de.path().filename().string();
      std::string display = name;
      if (de.is_directory()) {
        display += "/";
      }
      display = EscapeForHtml(display);

      std::filesystem::path link_path =
          std::filesystem::path(relative_path) /
          std::filesystem::path(name);
      std::string link_str = BuildCodeHref(
          normalized_base,
          UrlEncode(link_path.lexically_normal().string()));

      std::string meta_html;
      if (de.is_regular_file()) {
        std::error_code e2;
        auto sz = de.file_size(e2);
        if (!e2) {
          meta_html =
              nbsp() + span({{"class", "meta"}},
                            std::to_string(sz) + " bytes");
        }
      }
      list_ss << div(
          {{"class", "entry"}},
          a(link_str, code(display)) + meta_html);
    }
    html << div({{"class", "listing"}}, list_ss.str());
    return html.str();
  }

  if (std::filesystem::is_regular_file(target_canon)) {
    std::ifstream ifs(target_canon, std::ios::binary);
    if (!ifs) {
      html << h2("Error") << p("Failed to open file.");
      return html.str();
    }

    std::vector<std::string> lines;
    std::string raw;
    const std::size_t probeSize = 4096;
    std::string probe;
    probe.resize(probeSize);
    ifs.read(&probe[0], (std::streamsize)probeSize);
    std::streamsize readBytes = ifs.gcount();
    bool binary = false;
    for (std::streamsize i = 0; i < readBytes; ++i) {
      unsigned char c =
          static_cast<unsigned char>(probe[i]);
      if (c == 0) {
        binary = true;
        break;
      }
    }
    ifs.clear();
    ifs.seekg(0);
    if (binary)
      return TRACE(
          Error("Cannot render binary file: The "
                "requested file appears to be binary "
                "and cannot be rendered as text."));

    while (std::getline(ifs, raw)) {
      if (!raw.empty() && raw.back() == '\r')
        raw.pop_back();
      lines.push_back(raw);
    }

    std::size_t nlines = lines.size();
    std::size_t width =
        std::to_string(std::max<std::size_t>(1, nlines))
            .size();

    std::ostringstream ln_ss, code_ss;

    for (size_t i = 0; i < nlines; ++i) {
      ln_ss << std::setw((int)width) << std::setfill(' ')
            << (i + 1) << '\n';
      code_ss << EscapeForHtml(lines[i]) << '\n';
    }

    if (nlines == 0) {
      ln_ss << std::setw((int)width) << 1 << '\n';
      code_ss << '\n';
    }

    const std::string ln_pre =
        pre({{"class", "code_pre"}}, ln_ss.str());
    const std::string code_pre =
        pre({{"class", "code_pre"}}, code_ss.str());
    const std::string code_block = div(
        {{"class", "code_block"},
         {"style",
          "display: flex; flex-direction: row; "
          "overflow: auto;"}},
        div({{"class", "ln_col"},
             {"style",
              "flex: 0 0 auto; min-width: 3em; "
              "padding-right: 1em; user-select: none; "
              "color: #6c757d; text-align: right;"}},
            ln_pre),
        div({{"class", "code_col"},
             {"style", "flex: 1 1 auto; overflow: auto;"}},
            code_pre));

    html << MakeBreadcrumbs(
        relative_path.empty()
            ? target_canon.filename().string()
            : relative_path,
        normalized_base);

    std::error_code e3;
    auto fsize =
        std::filesystem::file_size(target_canon, e3);
    html << hr();
    if (!e3) {
      html << p(span(
          {{"class", "meta"}},
          "Size: " + std::to_string(fsize) + " bytes."));
    }
    html << hr();

    html << code_block;
    return html.str();
  }

  return TRACE(Error("Unsupported path type."));
}

static Response ErrorResponse(const Result& result) {
  auto status = HTTP_400_BAD_REQUEST;
  if (result.code() == cs::kNotFound) {
    status = HTTP_404_NOT_FOUND;
  } else if (result.code() >= 500) {
    status = HTTP_500_INTERNAL_SERVER_ERROR;
  }
  return Response(status,
                  cs::net::http::kContentTypeTextPlain,
                  result.message());
}

}  // namespace

namespace cs::apps::code_viewer::ui {

Response GetCodePage(Request request, ::Context<>&) {
  // TODO: Get code directory from service-registry or
  // environment variable Code-viewer serves files from
  // filesystem, not database records This needs a different
  // mechanism than DATA_DIR
  std::string code_dir =
      "/cs-public";  // Matches docker-compose volume mount

  std::string rel_path = request.path();
  if (!rel_path.empty() && rel_path.front() == '/')
    rel_path.erase(rel_path.begin());
  if (rel_path.empty()) rel_path = ".";

  auto html_or =
      RenderHtmlForSourceCodePath(rel_path, code_dir);
  if (!html_or.ok()) {
    return ErrorResponse(html_or);
  }

  std::string page_path = rel_path;
  if (page_path == ".") {
    page_path = "/";
  }

  auto page_or =
      MakePage(page_path, html_or.value(), request);
  if (!page_or.ok()) {
    return ErrorResponse(page_or);
  }
  return HtmlResponse(page_or.value());
}

}  // namespace cs::apps::code_viewer::ui

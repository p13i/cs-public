// cs/apps/common/file-viewer/quoted_include_link.gpt.cc
#include "cs/apps/common/file-viewer/quoted_include_link.gpt.hh"

#include <cctype>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

#include "cs/net/html/dom.hh"
#include "cs/net/html/escape.gpt.hh"

namespace {  // use_usings
using ::cs::net::html::EscapeForHtml;
}  // namespace

namespace {

static std::string UrlEncode(const std::string& s) {
  std::ostringstream oss;
  oss << std::hex;
  for (unsigned char c : s) {
    if (std::isalnum(c) || c == '-' || c == '.' ||
        c == '_' || c == '~' || c == '/') {
      oss << c;
    } else {
      oss << '%' << std::uppercase << std::setw(2)
          << std::setfill('0') << static_cast<int>(c)
          << std::nouppercase;
      oss << std::dec;
    }
  }
  return oss.str();
}

static std::string NormalizeBasePath(
    const std::string& base_path) {
  if (base_path.empty()) return "/";
  if (base_path.back() == '/') return base_path;
  return base_path + "/";
}

static std::string BuildHref(const std::string& base_path,
                             const std::string& relative) {
  std::string norm_base = NormalizeBasePath(base_path);
  std::string rel = relative;
  if (!rel.empty() && rel.front() == '/') {
    rel.erase(rel.begin());
  }
  if (norm_base == "/") {
    return "/" + rel;
  }
  return norm_base + rel;
}

// Returns true if the line has #include "path" (quoted
// form). Does not match #include <path>. On success, prefix
// is before the path (including #include "), path is the
// content between the quotes, suffix is the closing " and
// the rest of the line.
static bool ParseQuotedInclude(const std::string& line,
                               std::string* prefix,
                               std::string* path,
                               std::string* suffix) {
  const std::string include_keyword("include");
  std::size_t i = 0;
  while (i < line.size() && line[i] != '#') {
    ++i;
  }
  if (i >= line.size()) return false;
  ++i;  // skip '#'
  while (i < line.size() &&
         (line[i] == ' ' || line[i] == '\t')) {
    ++i;
  }
  if (i + include_keyword.size() > line.size() ||
      line.substr(i, include_keyword.size()) !=
          include_keyword) {
    return false;
  }
  i += include_keyword.size();
  while (i < line.size() &&
         (line[i] == ' ' || line[i] == '\t')) {
    ++i;
  }
  if (i >= line.size() || line[i] != '"') {
    return false;  // angle include or malformed
  }
  ++i;  // skip opening "
  std::size_t start_path = i;
  std::size_t end_path = line.find('"', i);
  if (end_path == std::string::npos) return false;
  *prefix = line.substr(0, start_path);
  *path = line.substr(start_path, end_path - start_path);
  *suffix = line.substr(end_path);
  return true;
}

}  // namespace

namespace cs::apps::common::file_viewer {

std::string ResolveQuotedIncludePath(
    const std::string& quoted_include_path,
    const std::string& current_file_relative_path,
    const std::string& repo_include_prefix) {
  if (!repo_include_prefix.empty() &&
      quoted_include_path.size() >=
          repo_include_prefix.size() &&
      quoted_include_path.compare(
          0, repo_include_prefix.size(),
          repo_include_prefix) == 0) {
    return quoted_include_path;
  }
  std::filesystem::path current_dir =
      std::filesystem::path(current_file_relative_path)
          .parent_path();
  return (current_dir /
          std::filesystem::path(quoted_include_path))
      .lexically_normal()
      .string();
}

std::string RenderCodeLineWithQuotedIncludeLinks(
    const std::string& line, bool link_quoted_includes,
    const std::string& relative_path,
    const std::string& normalized_base,
    const std::string& repo_include_prefix) {
  if (!link_quoted_includes) {
    return EscapeForHtml(line);
  }
  std::string prefix;
  std::string path;
  std::string suffix;
  if (!ParseQuotedInclude(line, &prefix, &path, &suffix)) {
    return EscapeForHtml(line);
  }
  std::string resolved = ResolveQuotedIncludePath(
      path, relative_path, repo_include_prefix);
  std::string href =
      BuildHref(normalized_base, UrlEncode(resolved));
  return EscapeForHtml(prefix) +
         ::cs::net::html::dom::a(href,
                                 EscapeForHtml(path)) +
         EscapeForHtml(suffix);
}

}  // namespace cs::apps::common::file_viewer

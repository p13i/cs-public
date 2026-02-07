// cs/apps/common/file-viewer/file_viewer.gpt.hh
#ifndef CS_APPS_COMMON_FILE_VIEWER_FILE_VIEWER_GPT_HH
#define CS_APPS_COMMON_FILE_VIEWER_FILE_VIEWER_GPT_HH

#include <string>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::common::file_viewer {

// Options for file-viewer page rendering (data-viewer vs
// code-viewer).
struct FileViewerOptions {
  std::string title;
  std::string nav_label;
  std::string service_name;
  std::string breadcrumb_root;
  std::string base_dir;
  std::string access_denied_suffix;
  bool show_github_link = false;
  std::string github_base_url;
  bool pretty_print_json = false;
  bool link_quoted_includes = false;
};

cs::net::http::Response GetFileViewerPage(
    cs::net::http::Request request,
    ::cs::util::di::Context<>& c,
    const FileViewerOptions& options);

}  // namespace cs::apps::common::file_viewer

#endif  // CS_APPS_COMMON_FILE_VIEWER_FILE_VIEWER_GPT_HH

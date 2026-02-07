// cs/apps/code-viewer/ui.gpt.cc
#include "cs/apps/code-viewer/ui.gpt.hh"

#include "cs/apps/common/file-viewer/file_viewer.gpt.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::apps::common::file_viewer::FileViewerOptions;
using ::cs::apps::common::file_viewer::GetFileViewerPage;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::util::di::Context;
}  // namespace

namespace cs::apps::code_viewer::ui {

namespace {
const FileViewerOptions kCodeViewerOptions{
    .title = "Code Viewer",
    .nav_label = "Code (cs)",
    .service_name = "code-viewer",
    .breadcrumb_root = "cs",
    .base_dir = "/cs-public",
    .access_denied_suffix = "code viewer",
    .show_github_link = true,
    .github_base_url =
        "https://github.com/p13i/cs-public/blob/main/",
    .link_quoted_includes = true,
};
}  // namespace

Response GetCodePage(Request request, Context<>& c) {
  return GetFileViewerPage(request, c, kCodeViewerOptions);
}

}  // namespace cs::apps::code_viewer::ui

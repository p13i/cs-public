// cs/apps/blob-viewer/ui.gpt.cc
#include "cs/apps/blob-viewer/ui.gpt.hh"

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

namespace cs::apps::blob_viewer::ui {

namespace {
const FileViewerOptions kBlobViewerOptions{
    .title = "Blob Viewer",
    .nav_label = "Blob",
    .service_name = "blob-viewer",
    .breadcrumb_root = "blob",
    .base_dir = "/data",
    .access_denied_suffix = "blob viewer",
    .show_github_link = false,
    .github_base_url = "",
    .pretty_print_json = false,
};
}  // namespace

Response GetBlobPage(Request request, Context<>& c) {
  return GetFileViewerPage(request, c, kBlobViewerOptions);
}

}  // namespace cs::apps::blob_viewer::ui

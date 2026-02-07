// cs/apps/data-viewer/ui.gpt.cc
#include "cs/apps/data-viewer/ui.gpt.hh"

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

namespace cs::apps::data_viewer::ui {

namespace {
const FileViewerOptions kDataViewerOptions{
    .title = "Data Viewer",
    .nav_label = "Data",
    .service_name = "data-viewer",
    .breadcrumb_root = "data",
    .base_dir = "/data",
    .access_denied_suffix = "data viewer",
    .show_github_link = false,
    .github_base_url = "",
    .pretty_print_json = true,
};
}  // namespace

Response GetDataPage(Request request, Context<>& c) {
  return GetFileViewerPage(request, c, kDataViewerOptions);
}

}  // namespace cs::apps::data_viewer::ui

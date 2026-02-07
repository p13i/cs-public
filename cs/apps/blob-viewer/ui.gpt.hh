// cs/apps/blob-viewer/ui.gpt.hh
#ifndef CS_APPS_BLOB_VIEWER_UI_GPT_HH
#define CS_APPS_BLOB_VIEWER_UI_GPT_HH

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::blob_viewer::ui {

cs::net::http::Response GetBlobPage(
    cs::net::http::Request request,
    ::cs::util::di::Context<>&);

}  // namespace cs::apps::blob_viewer::ui

#endif  // CS_APPS_BLOB_VIEWER_UI_GPT_HH

// cs/apps/data-viewer/ui.gpt.hh
#ifndef CS_APPS_DATA_VIEWER_UI_GPT_HH
#define CS_APPS_DATA_VIEWER_UI_GPT_HH

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::data_viewer::ui {

cs::net::http::Response GetDataPage(
    cs::net::http::Request request,
    ::cs::util::di::Context<>&);

}  // namespace cs::apps::data_viewer::ui

#endif  // CS_APPS_DATA_VIEWER_UI_GPT_HH

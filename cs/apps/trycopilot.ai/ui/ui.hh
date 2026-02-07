// cs/apps/trycopilot.ai/ui/ui.hh
// cs/apps/trycopilot.ai/ui/ui.hh
// cs/apps/trycopilot.ai/ui/ui.hh
#ifndef CS_APPS_TRYCOPILOT_AI_UI_UI_HH
#define CS_APPS_TRYCOPILOT_AI_UI_UI_HH

#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/http/handler.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/util/di/context.gpt.hh"

using UIContext = ::cs::util::di::Context<
    ::cs::net::proto::db::IDatabaseClient>;

namespace cs::apps::trycopilotai::ui {

DECLARE_HANDLER(GetIndexPage, UIContext);
DECLARE_HANDLER(GetGame, UIContext);
DECLARE_HANDLER(GetHealthPage, UIContext);
DECLARE_HANDLER(Render, UIContext);
DECLARE_HANDLER(GetRegisterPage, UIContext);
DECLARE_HANDLER(PostRegisterPage, UIContext);
DECLARE_HANDLER(GetLoginPage, UIContext);
DECLARE_HANDLER(PostLoginPage, UIContext);
DECLARE_HANDLER(GetLogoutPage, UIContext);
DECLARE_HANDLER(PostLogoutPage, UIContext);
DECLARE_HANDLER(GetHomePage, UIContext);
DECLARE_HANDLER(GetLogsPage, UIContext);
DECLARE_HANDLER(GetAppLogsPage, UIContext);
DECLARE_HANDLER(GetQuitPage, UIContext);
DECLARE_HANDLER(ApiListLogs, UIContext);
DECLARE_HANDLER(GetCodePage, UIContext);
DECLARE_HANDLER(GetNewWebsite, UIContext);
DECLARE_HANDLER(GetAboutPage, UIContext);

std::string extractAuthToken(const std::string& cookie);

ResultOr<std::string> GetToken(
    const cs::net::http::Request& request);

ResultOr<cs::apps::trycopilotai::protos::User>
GetAuthenticatedUser(cs::net::http::Request request,
                     UIContext& ctx);
}  // namespace cs::apps::trycopilotai::ui

#endif  // CS_APPS_TRYCOPILOT_AI_UI_UI_HH

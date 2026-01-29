// cs/apps/trycopilot.ai/ui/ui.hh
// cs/apps/trycopilot.ai/ui/ui.hh
// cs/apps/trycopilot.ai/ui/ui.hh
#ifndef CS_APPS_TRYCOPILOT_AI_UI_UI_HH
#define CS_APPS_TRYCOPILOT_AI_UI_UI_HH

#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"

#define DECLARE_HANDLER(_name)   \
  cs::net::http::Response _name( \
      cs::net::http::Request request);

namespace cs::apps::trycopilotai::ui {

DECLARE_HANDLER(GetIndexPage);
DECLARE_HANDLER(GetGame);
DECLARE_HANDLER(GetHealthPage);
DECLARE_HANDLER(Render);
DECLARE_HANDLER(GetRegisterPage);
DECLARE_HANDLER(PostRegisterPage);
DECLARE_HANDLER(GetLoginPage);
DECLARE_HANDLER(PostLoginPage);
DECLARE_HANDLER(GetLogoutPage);
DECLARE_HANDLER(PostLogoutPage);
DECLARE_HANDLER(GetHomePage);
DECLARE_HANDLER(GetLogsPage);
DECLARE_HANDLER(GetAppLogsPage);
DECLARE_HANDLER(GetQuitPage);
DECLARE_HANDLER(ApiListLogs);
DECLARE_HANDLER(GetCodePage);
DECLARE_HANDLER(GetNewWebsite);
DECLARE_HANDLER(GetAboutPage);

std::string extractAuthToken(const std::string &cookie);

ResultOr<std::string> GetToken(
    cs::net::http::Request request);

ResultOr<cs::apps::trycopilotai::protos::User>
GetAuthenticatedUser(cs::net::http::Request request);
}  // namespace cs::apps::trycopilotai::ui

#endif  // CS_APPS_TRYCOPILOT_AI_UI_UI_HH

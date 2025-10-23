#ifndef CS_WWW_APP_UI_UI_HH
#define CS_WWW_APP_UI_UI_HH

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/www/app/protos/user.proto.hh"

#define DECLARE_HANDLER(_name)   \
  cs::net::http::Response _name( \
      cs::net::http::Request request);

namespace cs::www::app::ui {

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

ResultOr<cs::www::app::protos::User> GetAuthenticatedUser(
    cs::net::http::Request request);
}  // namespace cs::www::app::ui

#endif  // CS_WWW_APP_UI_UI_HH

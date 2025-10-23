#ifndef CS_WWW_APP_API_AUTH_HH
#define CS_WWW_APP_API_AUTH_HH

#include "cs/net/proto/api.hh"
#include "cs/www/app/protos/auth.proto.hh"

namespace cs::www::app::api {

DECLARE_API(LoginAPI, cs::www::app::protos::LoginRequest,
            cs::www::app::protos::LoginResponse);

DECLARE_API(LogoutAPI, cs::www::app::protos::LogoutRequest,
            cs::www::app::protos::LogoutResponse);

}  // namespace cs::www::app::api

#endif  // CS_WWW_APP_API_AUTH_HH
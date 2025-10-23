#ifndef CS_WWW_APP_PROTOS_AUTH_PROTO_HH
#define CS_WWW_APP_PROTOS_AUTH_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Token) {
  std::string uuid;
  std::string user_uuid;
  bool active;
};

DECLARE_PROTO(LoginRequest) {
  std::string email;
  std::string password;
};

DECLARE_PROTO(LoginResponse) {
  std::string email;
  Token token;
};

DECLARE_PROTO(LogoutRequest) { std::string token_uuid; };

DECLARE_PROTO(LogoutResponse) {
  std::string email;
  bool logged_out;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_AUTH_PROTO_HH

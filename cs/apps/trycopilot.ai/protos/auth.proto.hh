// cs/apps/trycopilot.ai/protos/auth.proto.hh
// cs/apps/trycopilot.ai/protos/auth.proto.hh
// cs/apps/trycopilot.ai/protos/auth.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_AUTH_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_AUTH_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Token payload.
DECLARE_PROTO(Token) {
  // Token UUID.
  std::string uuid;
  // User UUID.
  std::string user_uuid;
  // Token active flag.
  bool active;
};

// Login request payload.
DECLARE_PROTO(LoginRequest) {
  // Login email address.
  [[required]] [[email]]
  std::string email;
  // Login password.
  [[required]]
  std::string password;
};

// Login response payload.
DECLARE_PROTO(LoginResponse) {
  // User email.
  std::string email;
  // Issued token.
  Token token;
};

// Logout request payload.
DECLARE_PROTO(LogoutRequest) {
  // Token to invalidate.
  std::string token_uuid;
};

// Logout response payload.
DECLARE_PROTO(LogoutResponse) {
  // User email.
  std::string email;
  // Logout success flag.
  bool logged_out;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_AUTH_PROTO_HH

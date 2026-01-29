// cs/apps/trycopilot.ai/protos/user.proto.hh
// cs/apps/trycopilot.ai/protos/user.proto.hh
// cs/apps/trycopilot.ai/protos/user.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_USER_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_USER_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Password hash data.
DECLARE_PROTO(Password) {
  // Password salt.
  std::string salt;
  // Password hash.
  std::string hash;
};

// User account payload.
DECLARE_PROTO(User) {
  // User UUID.
  std::string uuid;
  // User email.
  std::string email;
  // Password data.
  Password password;
  // Admin flag.
  bool admin;
};

// Create user request.
DECLARE_PROTO(CreateUserRequest) {
  // User email.
  std::string email;
  // Password text.
  std::string password;
  // Confirm password text.
  std::string confirm_password;
};

// Create user response.
DECLARE_PROTO(CreateUserResponse) {
  // User email.
  std::string email;
  // User UUID.
  std::string user_uuid;
  // Created flag.
  bool created;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_USER_PROTO_HH

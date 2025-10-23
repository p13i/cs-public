#ifndef CS_WWW_APP_PROTOS_USER_PROTO_HH
#define CS_WWW_APP_PROTOS_USER_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Password) {
  std::string salt;
  std::string hash;
};

DECLARE_PROTO(User) {
  std::string uuid;
  std::string email;
  Password password;
  bool admin;
};

DECLARE_PROTO(CreateUserRequest) {
  std::string email;
  std::string password;
  std::string confirm_password;
};

DECLARE_PROTO(CreateUserResponse) {
  std::string email;
  bool created;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_USER_PROTO_HH

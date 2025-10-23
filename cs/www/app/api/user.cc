#include "cs/www/app/api/user.hh"

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"
#include "cs/www/app/context/context.hh"
#include "cs/www/app/protos/auth.proto.hh"
#include "cs/www/app/protos/user.proto.hh"

namespace {
using ::cs::InvalidArgument;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::www::app::protos::CreateUserRequest;
using ::cs::www::app::protos::CreateUserResponse;
using ::cs::www::app::protos::LoginRequest;
using ::cs::www::app::protos::LogoutResponse;
using ::cs::www::app::protos::Token;
using ::cs::www::app::protos::User;

// djb2
std::string HashPassword(std::string password,
                         std::string salt) {
  unsigned long hash = 5381;
  auto str = salt + password + salt;
  for (char c : str) {
    hash = ((hash << 5) + hash) + c;  // hash * 33 + c
  }

  std::stringstream ss;
  ss << std::hex << hash;
  return ss.str();
}

}  // namespace

namespace cs::www::app::api {

IMPLEMENT_API(CreateUserAPI, CreateUserRequest,
              CreateUserResponse) {
  // Validation
  if (request.email.empty()) {
    return TRACE(
        InvalidArgument("Email must not be empty."));
  }
  if (request.password.empty()) {
    return TRACE(
        InvalidArgument("Password must not be empty."));
  }
  if (request.confirm_password.empty()) {
    return TRACE(InvalidArgument(
        "Password confirmation must not be empty."));
  }
  if (request.password != request.confirm_password) {
    return TRACE(
        InvalidArgument("Passwords do not match."));
  }

  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));
  JsonProtoDatabase<User> users_db(DATA_DIR, "users");

  // Lookup if a user with this email already exists in
  // the users database.
  SET_OR_RET(
      bool user_exists,
      users_db.query_view().then([&](auto query_view) {
        return query_view
            .where(
                LAMBDA(u, EQUALS(u.email, request.email)))
            .any();
      }));
  if (user_exists) {
    return TRACE(
        Error("User with this email already exists."));
  }

  // Save to database.
  User user;
  user.email = request.email;
  user.password.salt =
      cs::util::random::uuid::generate_uuid_v4();
  user.password.hash =
      HashPassword(request.password, user.password.salt);
  user.uuid = cs::util::random::uuid::generate_uuid_v4();
  user.admin = false;

  OK_OR_RET(users_db.Insert(user));

  // Return a view of the user.
  CreateUserResponse response;
  response.email = user.email;

  return response;
}

}  // namespace cs::www::app::api

namespace cs::www::app::rpc {
IMPLEMENT_RPC(CreateUserRPC, CreateUserRequest,
              CreateUserResponse) {
  return cs::www::app::api::CreateUserAPI().Process(
      request);
}
}  // namespace cs::www::app::rpc

// cs/apps/trycopilot.ai/api/user.cc
#include "cs/apps/trycopilot.ai/api/user.hh"

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {
using ::cs::InvalidArgument;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::EQUALS;

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

namespace cs::apps::trycopilotai::api {

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

  // Lookup if a user with this email already exists in
  // the users database.
  cs::net::proto::db::QueryView<User> users_query("users");
  SET_OR_RET(
      bool user_exists,
      users_query.where(EQUALS("email", request.email))
          .any());
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

  OK_OR_RET(cs::net::proto::db::Insert("users", user));

  // Return a view of the user with UUID for auto-login
  CreateUserResponse response;
  response.email = user.email;
  response.user_uuid =
      user.uuid;  // Include UUID so caller can create token

  return response;
}

}  // namespace cs::apps::trycopilotai::api

namespace cs::apps::trycopilotai::rpc {
IMPLEMENT_RPC(CreateUserRPC, CreateUserRequest,
              CreateUserResponse) {
  return cs::apps::trycopilotai::api::CreateUserAPI()
      .Process(request);
}
}  // namespace cs::apps::trycopilotai::rpc

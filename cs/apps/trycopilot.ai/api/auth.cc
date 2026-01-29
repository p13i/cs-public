// cs/apps/trycopilot.ai/api/auth.cc
#include "cs/apps/trycopilot.ai/api/auth.hh"

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.validate.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::ValidationError;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::validation::Validate;
using LoginRequestRules = ::cs::apps::trycopilotai::protos::
    gencode::auth::validation_generated::LoginRequestRules;

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
IMPLEMENT_API(LoginAPI, LoginRequest, LoginResponse) {
  auto validation = Validate(request, LoginRequestRules{});
  if (!validation.ok()) {
    return TRACE(validation);
  }
  // Lookup user email in users database.
  cs::net::proto::db::QueryView<User> users_query("users");
  SET_OR_RET(
      User user,
      users_query.where(EQUALS("email", request.email))
          .first());

  if (HashPassword(request.password, user.password.salt) !=
      user.password.hash) {
    return TRACE(InvalidArgument("Invalid password."));
  }

  cs::net::proto::db::QueryView<Token> tokens_query(
      "tokens");
  ResultOr<Token> existing_token =
      tokens_query.where(EQUALS("user_uuid", user.uuid))
          .where(EQUALS("active", true))
          .first();
  Token token;
  if (Result::IsNotFound(existing_token)) {
    token.user_uuid = user.uuid;
    token.uuid = cs::util::random::uuid::generate_uuid_v4();
    token.active = true;
    OK_OR_RET(cs::net::proto::db::Insert("tokens", token));
  } else if (existing_token.ok()) {
    token = existing_token.value();
  } else {
    return TRACE(existing_token);
  }
  LoginResponse response;
  response.email = user.email;
  response.token = token;
  return response;
}

IMPLEMENT_API(
    LogoutAPI,
    cs::apps::trycopilotai::protos::LogoutRequest,
    cs::apps::trycopilotai::protos::LogoutResponse) {
  if (request.token_uuid.empty()) {
    return TRACE(
        ValidationError("`token_uuid` must not be empty."));
  }
  cs::net::proto::db::QueryView<Token> tokens_query(
      "tokens");
  SET_OR_RET(
      auto token,
      tokens_query.where(EQUALS("uuid", request.token_uuid))
          .first());

  token.active = false;
  OK_OR_RET(cs::net::proto::db::Insert("tokens", token));

  cs::apps::trycopilotai::protos::LogoutResponse response;
  response.logged_out = true;
  return response;
}
}  // namespace cs::apps::trycopilotai::api
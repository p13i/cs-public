#include "cs/www/app/api/auth.hh"

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"
#include "cs/www/app/context/context.hh"
#include "cs/www/app/protos/auth.proto.hh"
#include "cs/www/app/protos/user.proto.hh"

namespace {
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::ValidationError;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::www::app::protos::LoginRequest;
using ::cs::www::app::protos::LoginResponse;
using ::cs::www::app::protos::LogoutRequest;
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
IMPLEMENT_API(LoginAPI, LoginRequest, LoginResponse) {
  if (request.email.empty()) {
    return TRACE(
        ValidationError("Email must not be empty."));
  }
  if (request.password.empty()) {
    return TRACE(
        ValidationError("Password must not be empty."));
  }
  // Lookup user email in users database.
  SET_OR_RET(auto DATA_DIR, Context::Read("DATA_DIR"));
  JsonProtoDatabase<User> users_db(DATA_DIR, "users");
  SET_OR_RET(
      User user,
      users_db.query_view().then([&](auto query_view) {
        return query_view
            .where(
                LAMBDA(u, EQUALS(u.email, request.email)))
            .first();
      }));

  if (HashPassword(request.password, user.password.salt) !=
      user.password.hash) {
    return TRACE(InvalidArgument("Invalid password."));
  }

  JsonProtoDatabase<Token> tokens_db(DATA_DIR, "tokens");
  ResultOr<Token> existing_token =
      tokens_db.query_view().then([&](auto query_view) {
        return query_view
            .where(
                LAMBDA(t, EQUALS(t.user_uuid, user.uuid)))
            .where(LAMBDA(t, EQUALS(t.active, true)))
            .first();
      });
  Token token;
  if (Result::IsNotFound(existing_token)) {
    token.user_uuid = user.uuid;
    token.uuid = cs::util::random::uuid::generate_uuid_v4();
    token.active = true;
    OK_OR_RET(tokens_db.Insert(token));
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

IMPLEMENT_API(LogoutAPI,
              cs::www::app::protos::LogoutRequest,
              cs::www::app::protos::LogoutResponse) {
  if (request.token_uuid.empty()) {
    return TRACE(
        ValidationError("`token_uuid` must not be empty."));
  }
  SET_OR_RET(auto DATA_DIR, Context::Read("DATA_DIR"));
  JsonProtoDatabase<Token> tokens_db(DATA_DIR, "tokens");
  SET_OR_RET(
      auto token,
      tokens_db.query_view().then([&](auto query_view) {
        return query_view
            .where(LAMBDA(
                t, EQUALS(t.uuid, request.token_uuid)))
            .first();
      }));

  token.active = false;
  OK_OR_RET(tokens_db.Insert(token));

  cs::www::app::protos::LogoutResponse response;
  response.logged_out = true;
  return response;
}
}  // namespace cs::www::app::api
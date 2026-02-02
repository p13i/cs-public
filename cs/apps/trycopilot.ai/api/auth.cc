// cs/apps/trycopilot.ai/api/auth.cc
#include "cs/apps/trycopilot.ai/api/auth.hh"

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.validate.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::InvalidArgument;
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
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::net::proto::validation::Validate;
using ::cs::util::di::Context;
}  // namespace

namespace {  // impl
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

using ::cs::InvalidArgument;
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
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::net::proto::validation::Validate;
using ::cs::util::random::uuid::generate_uuid_v4;
using LoginRequestRules = ::cs::apps::trycopilotai::protos::
    gencode::auth::validation_generated::LoginRequestRules;

IMPLEMENT_RPC(LoginRPC, LoginRequest, LoginResponse) {
  auto validation = Validate(request, LoginRequestRules{});
  if (!validation.ok()) {
    return TRACE(validation);
  }
  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();
  SET_OR_RET(User user,
             QueryView<User>("users", db)
                 .where(EQUALS(&User::email, request.email))
                 .first());

  if (HashPassword(request.password, user.password.salt) !=
      user.password.hash) {
    return TRACE(InvalidArgument("Invalid password."));
  }

  QueryView<Token> tokens_query("tokens", db);
  ResultOr<Token> existing_token =
      tokens_query
          .where(EQUALS(&Token::user_uuid, user.uuid))
          .where(EQUALS(&Token::active, true))
          .first();
  Token token;
  if (Result::IsNotFound(existing_token)) {
    token.user_uuid = user.uuid;
    token.uuid = generate_uuid_v4();
    token.active = true;
    OK_OR_RET(Upsert("tokens", token, *db));
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

IMPLEMENT_RPC(LogoutRPC, LogoutRequest, LogoutResponse) {
  if (request.token_uuid.empty()) {
    return TRACE(
        ValidationError("`token_uuid` must not be empty."));
  }
  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();
  SET_OR_RET(
      auto token,
      QueryView<Token>("tokens", db)
          .where(EQUALS(&Token::uuid, request.token_uuid))
          .first());

  token.active = false;
  OK_OR_RET(Upsert("tokens", token, *db));

  LogoutResponse response;
  response.logged_out = true;
  return response;
}
}  // namespace cs::apps::trycopilotai::api
// cs/apps/trycopilot.ai/api/user.cc
#include "cs/apps/trycopilot.ai/api/user.hh"

#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
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
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::ResultOr;
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
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

IMPLEMENT_RPC(CreateUserRPC, CreateUserRequest,
              CreateUserResponse) {
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

  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();
  SET_OR_RET(bool user_exists,
             QueryView<User>("users", db)
                 .where(EQUALS(&User::email, request.email))
                 .any());
  if (user_exists) {
    return TRACE(
        Error("User with this email already exists."));
  }

  User user;
  user.email = request.email;
  user.password.salt =
      ::cs::util::random::uuid::generate_uuid_v4();
  user.password.hash =
      HashPassword(request.password, user.password.salt);
  user.uuid = ::cs::util::random::uuid::generate_uuid_v4();
  user.admin = false;

  OK_OR_RET(
      Upsert("users", user, *ctx->Get<IDatabaseClient>()));

  CreateUserResponse response;
  response.email = user.email;
  response.user_uuid = user.uuid;

  return response;
}

}  // namespace cs::apps::trycopilotai::api

namespace cs::apps::trycopilotai::rpc {

ResultOr<CreateUserResponse> CreateUserRPC::Process(
    const CreateUserRequest& request) {
  SET_OR_RET(auto* ctx, GetContext());
  return CreateUserRPC(*ctx).Process(request);
}

}  // namespace cs::apps::trycopilotai::rpc

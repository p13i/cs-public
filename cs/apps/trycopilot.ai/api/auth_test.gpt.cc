// cs/apps/trycopilot.ai/api/auth_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/auth.hh"

#include <string>

#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/proto/db/in_memory_client.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::api::LoginRPC;
using ::cs::apps::trycopilotai::api::LogoutRPC;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::proto::db::DatabaseBaseUrl;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::InMemoryDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
}  // namespace

using AppContext =
    Context<DatabaseBaseUrl, IDatabaseClient>;

// Test fixture for auth tests with in-memory DB via DI
class AuthAPITest : public ::testing::Test {
 protected:
  void SetUp() override {
    app_ctx_ = ContextBuilder<AppContext>()
                   .bind<DatabaseBaseUrl>()
                   .with(std::string(""))
                   .bind<IDatabaseClient>()
                   .to<InMemoryDatabaseClient>()
                   .build();
  }

  CreateUserRequest BuildCreateUserRequest(
      const std::string& email, const std::string& password,
      const std::string& confirm_password) {
    CreateUserRequest req;
    req.email = email;
    req.password = password;
    req.confirm_password = confirm_password;
    return req;
  }

  LoginRequest BuildLoginRequest(
      const std::string& email,
      const std::string& password) {
    LoginRequest req;
    req.email = email;
    req.password = password;
    return req;
  }

  AppContext app_ctx_;
  std::string test_email_ =
      "test_" + generate_uuid_v4() + "@example.com";
  std::string test_password_ = "SecureP@ssw0rd!";
};

TEST_F(AuthAPITest, CreateUserSuccess) {
  CreateUserRPC api(app_ctx_);
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto result = api.Process(request);
  ASSERT_TRUE(result.ok()) << result.message();
  EXPECT_EQ(result.value().email, test_email_);
  EXPECT_FALSE(result.value().user_uuid.empty());
}

TEST_F(AuthAPITest, CreateUserRejectsEmptyEmail) {
  CreateUserRPC api(app_ctx_);
  auto request = BuildCreateUserRequest("", test_password_,
                                        test_password_);

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(
      result.message(),
      ::testing::HasSubstr("Email must not be empty"));
}

TEST_F(AuthAPITest, CreateUserRejectsEmptyPassword) {
  CreateUserRPC api(app_ctx_);
  auto request =
      BuildCreateUserRequest(test_email_, "", "");

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(
      result.message(),
      ::testing::HasSubstr("Password must not be empty"));
}

TEST_F(AuthAPITest, CreateUserRejectsPasswordMismatch) {
  CreateUserRPC api(app_ctx_);
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, "DifferentPassword");

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(
      result.message(),
      ::testing::HasSubstr("Passwords do not match"));
}

TEST_F(AuthAPITest, CreateUserRejectsDuplicateEmail) {
  CreateUserRPC api(app_ctx_);
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto first_result = api.Process(request);
  ASSERT_TRUE(first_result.ok()) << first_result.message();

  auto duplicate_result = api.Process(request);

  ASSERT_FALSE(duplicate_result.ok());
  EXPECT_THAT(duplicate_result.message(),
              ::testing::HasSubstr(
                  "User with this email already exists"));
}

TEST_F(AuthAPITest, LoginSuccessAfterRegistration) {
  CreateUserRPC create_api(app_ctx_);
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_request);
  ASSERT_TRUE(create_result.ok())
      << create_result.message();

  LoginRPC login_rpc(app_ctx_);
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);
  auto login_result = login_rpc.Process(login_request);

  ASSERT_TRUE(login_result.ok())
      << "Login failed: " << login_result.message();
  EXPECT_EQ(login_result.value().email, test_email_);
  EXPECT_FALSE(login_result.value().token.uuid.empty());
  EXPECT_EQ(login_result.value().token.user_uuid,
            create_result.value().user_uuid);
  EXPECT_TRUE(login_result.value().token.active);
}

TEST_F(AuthAPITest, LoginRejectsWrongPassword) {
  CreateUserRPC create_api(app_ctx_);
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_request);
  ASSERT_TRUE(create_result.ok())
      << create_result.message();

  LoginRPC login_rpc(app_ctx_);
  auto login_request =
      BuildLoginRequest(test_email_, "WrongPassword");
  auto login_result = login_rpc.Process(login_request);

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              ::testing::HasSubstr("Invalid password"));
}

TEST_F(AuthAPITest, LoginRejectsNonexistentUser) {
  LoginRPC login_rpc(app_ctx_);
  auto login_request = BuildLoginRequest(
      "nonexistent_" + generate_uuid_v4() + "@example.com",
      test_password_);

  auto login_result = login_rpc.Process(login_request);

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              ::testing::HasSubstr("No values found"));
}

TEST_F(AuthAPITest, LoginReusesExistingActiveToken) {
  CreateUserRPC create_api(app_ctx_);
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_request);
  ASSERT_TRUE(create_result.ok())
      << create_result.message();

  LoginRPC login_rpc(app_ctx_);
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);
  auto first_login = login_rpc.Process(login_request);
  ASSERT_TRUE(first_login.ok()) << first_login.message();
  std::string first_token = first_login.value().token.uuid;

  auto second_login = login_rpc.Process(login_request);
  ASSERT_TRUE(second_login.ok()) << second_login.message();
  std::string second_token =
      second_login.value().token.uuid;

  EXPECT_EQ(first_token, second_token)
      << "Should reuse existing active token";
}

TEST_F(AuthAPITest, LogoutDeactivatesToken) {
  CreateUserRPC create_api(app_ctx_);
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_request);
  ASSERT_TRUE(create_result.ok())
      << create_result.message();

  LoginRPC login_rpc(app_ctx_);
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);
  auto login_result = login_rpc.Process(login_request);
  ASSERT_TRUE(login_result.ok()) << login_result.message();

  LogoutRPC logout_rpc(app_ctx_);
  LogoutRequest logout_request;
  logout_request.token_uuid =
      login_result.value().token.uuid;
  auto logout_result = logout_rpc.Process(logout_request);
  ASSERT_TRUE(logout_result.ok());
  EXPECT_TRUE(logout_result.value().logged_out);

  auto db = app_ctx_.Get<IDatabaseClient>();
  auto token_result =
      QueryView<Token>("tokens", db)
          .where(EQUALS(&Token::uuid,
                        login_result.value().token.uuid))
          .first();
  ASSERT_TRUE(token_result.ok());
  EXPECT_FALSE(token_result.value().active)
      << "Token should be inactive after logout";
}

TEST_F(AuthAPITest, LogoutRejectsEmptyTokenUuid) {
  LogoutRPC logout_rpc(app_ctx_);
  LogoutRequest logout_request;
  logout_request.token_uuid = "";

  auto result = logout_rpc.Process(logout_request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              ::testing::HasSubstr(
                  "token_uuid` must not be empty"));
}

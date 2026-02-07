// cs/apps/trycopilot.ai/api/auth_flow_test.gpt.cc
#include <string>

#include "cs/apps/trycopilot.ai/api/auth.hh"
#include "cs/apps/trycopilot.ai/api/test_db_store.gpt.hh"
#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
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
using ::cs::apps::trycopilotai::api::testing::TestDbStore;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::testing::Invoke;
using ::testing::Return;
}  // namespace

using AppContext = Context<IDatabaseClient>;

namespace {  // impl
class AuthFlowTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_store_ =
        std::make_shared<cs::apps::trycopilotai::api::
                             testing::TestDbStore>();
    mock_db_ = std::make_shared<MockDatabaseClient>();
    ON_CALL(*mock_db_, GetBaseUrl())
        .WillByDefault(Return("mock://"));
    ON_CALL(*mock_db_, Upsert(::testing::_))
        .WillByDefault(
            Invoke([this](const cs::net::proto::database::
                              UpsertRequest& r) {
              return cs::apps::trycopilotai::api::testing::
                  UpsertToStore(r, db_store_.get());
            }));
    ON_CALL(*mock_db_, Query(::testing::_))
        .WillByDefault(
            Invoke([this](const cs::net::proto::database::
                              QueryRequest& r) {
              return cs::apps::trycopilotai::api::testing::
                  QueryFromStore(r, *db_store_);
            }));
    app_ctx_ =
        ContextBuilder<AppContext>()
            .bind<IDatabaseClient>()
            .from([this](AppContext&) { return mock_db_; })
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

  std::shared_ptr<TestDbStore> db_store_;
  std::shared_ptr<MockDatabaseClient> mock_db_;
  AppContext app_ctx_;
  std::string test_email_ =
      "test_" + generate_uuid_v4() + "@example.com";
  std::string test_password_ = "SecureP@ssw0rd!";
};

// Full register -> login -> logout flow. Verifies token is
// marked inactive in the DB; GetAuthenticatedUser (ui)
// rejects inactive tokens so the user does not appear
// logged in after logout.
TEST_F(AuthFlowTest, RegisterLoginLogoutFullFlow) {
  CreateUserRPC create_api(app_ctx_);
  auto create_req = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_req);
  ASSERT_TRUE(create_result.ok())
      << "CreateUser failed: " << create_result.message();
  EXPECT_EQ(create_result.value().email, test_email_);
  EXPECT_FALSE(create_result.value().user_uuid.empty());

  LoginRPC login_rpc(app_ctx_);
  auto login_req =
      BuildLoginRequest(test_email_, test_password_);
  auto login_result = login_rpc.Process(login_req);
  ASSERT_TRUE(login_result.ok())
      << "Login failed: " << login_result.message();
  EXPECT_EQ(login_result.value().email, test_email_);
  EXPECT_FALSE(login_result.value().token.uuid.empty());
  EXPECT_EQ(login_result.value().token.user_uuid,
            create_result.value().user_uuid);
  EXPECT_TRUE(login_result.value().token.active);

  LogoutRPC logout_rpc(app_ctx_);
  LogoutRequest logout_req;
  logout_req.token_uuid = login_result.value().token.uuid;
  auto logout_result = logout_rpc.Process(logout_req);
  ASSERT_TRUE(logout_result.ok());
  EXPECT_TRUE(logout_result.value().logged_out);

  auto db = app_ctx_.Get<IDatabaseClient>();
  auto tokens_query =
      QueryView<Token>("tokens", db)
          .where(EQUALS(&Token::uuid,
                        login_result.value().token.uuid));
  auto token_after = tokens_query.first();
  ASSERT_TRUE(token_after.ok());
  EXPECT_THAT(token_after.value().active,
              ::testing::IsFalse());
}

// Verifies that after logout the old token is not reused: a
// subsequent login returns a new token, and the UI layer
// (GetAuthenticatedUser) rejects inactive tokens so the
// user does not appear logged in.
TEST_F(AuthFlowTest, AfterLogoutLoginReturnsNewToken) {
  CreateUserRPC create_api(app_ctx_);
  auto create_req = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_req);
  ASSERT_TRUE(create_result.ok())
      << create_result.message();

  LoginRPC login_rpc(app_ctx_);
  auto login_req =
      BuildLoginRequest(test_email_, test_password_);
  auto first_login = login_rpc.Process(login_req);
  ASSERT_TRUE(first_login.ok()) << first_login.message();
  std::string logged_out_token_uuid =
      first_login.value().token.uuid;

  LogoutRPC logout_rpc(app_ctx_);
  LogoutRequest logout_req;
  logout_req.token_uuid = logged_out_token_uuid;
  auto logout_result = logout_rpc.Process(logout_req);
  ASSERT_TRUE(logout_result.ok());
  EXPECT_TRUE(logout_result.value().logged_out);

  // Login again with same credentials: backend must issue a
  // new token (inactive tokens are not reused), so user is
  // not "still logged in" with the old token.
  auto second_login = login_rpc.Process(login_req);
  ASSERT_TRUE(second_login.ok()) << second_login.message();
  std::string new_token_uuid =
      second_login.value().token.uuid;

  EXPECT_NE(logged_out_token_uuid, new_token_uuid)
      << "After logout, next login must return a new "
         "token; "
      << "old token must not be reused.";
}

TEST_F(AuthFlowTest, LoginRejectsWrongPassword) {
  CreateUserRPC create_api(app_ctx_);
  auto create_req = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_req);
  ASSERT_TRUE(create_result.ok());

  LoginRPC login_rpc(app_ctx_);
  auto login_req =
      BuildLoginRequest(test_email_, "WrongPassword");
  auto login_result = login_rpc.Process(login_req);

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              ::testing::HasSubstr("Invalid password"));
}

TEST_F(AuthFlowTest, LoginRejectsNonexistentUser) {
  LoginRPC login_rpc(app_ctx_);
  auto login_req = BuildLoginRequest(
      "nonexistent_" + generate_uuid_v4() + "@example.com",
      test_password_);
  auto login_result = login_rpc.Process(login_req);

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              ::testing::HasSubstr("No values found"));
}

TEST_F(AuthFlowTest, LoginReusesExistingActiveToken) {
  CreateUserRPC create_api(app_ctx_);
  auto create_req = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_req);
  ASSERT_TRUE(create_result.ok());

  LoginRPC login_rpc(app_ctx_);
  auto login_req =
      BuildLoginRequest(test_email_, test_password_);
  auto first_login = login_rpc.Process(login_req);
  ASSERT_TRUE(first_login.ok());
  std::string first_token = first_login.value().token.uuid;

  auto second_login = login_rpc.Process(login_req);
  ASSERT_TRUE(second_login.ok());
  EXPECT_EQ(first_login.value().token.uuid,
            second_login.value().token.uuid);
}

TEST_F(AuthFlowTest, CreateUserRejectsDuplicateEmail) {
  CreateUserRPC create_api(app_ctx_);
  auto create_req = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto first = create_api.Process(create_req);
  ASSERT_TRUE(first.ok());

  auto duplicate = create_api.Process(create_req);
  ASSERT_FALSE(duplicate.ok());
  EXPECT_THAT(duplicate.message(),
              ::testing::HasSubstr(
                  "User with this email already exists"));
}

TEST_F(AuthFlowTest, InMemoryClientPersistsAcrossRpcs) {
  auto db = app_ctx_.Get<IDatabaseClient>();
  auto any_before =
      QueryView<User>("users", db)
          .where(EQUALS(&User::email, test_email_))
          .any();
  ASSERT_TRUE(any_before.ok());
  EXPECT_FALSE(any_before.value());

  CreateUserRPC create_api(app_ctx_);
  auto create_result =
      create_api.Process(BuildCreateUserRequest(
          test_email_, test_password_, test_password_));
  ASSERT_TRUE(create_result.ok());

  auto any_after =
      QueryView<User>("users", db)
          .where(EQUALS(&User::email, test_email_))
          .any();
  ASSERT_TRUE(any_after.ok());
  EXPECT_TRUE(any_after.value());
}

TEST_F(AuthFlowTest, LogoutRejectsEmptyTokenUuid) {
  LogoutRPC logout_rpc(app_ctx_);
  LogoutRequest req;
  req.token_uuid = "";
  auto result = logout_rpc.Process(req);
  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              ::testing::HasSubstr("must not be empty"));
}

}  // namespace

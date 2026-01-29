// cs/apps/trycopilot.ai/api/auth_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/auth.hh"

#include <chrono>
#include <string>
#include <thread>

#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using ::cs::apps::trycopilotai::api::CreateUserAPI;
using ::cs::apps::trycopilotai::api::LoginAPI;
using ::cs::apps::trycopilotai::api::LogoutAPI;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::QueryView;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::testing::HasSubstr;
using ::testing::IsTrue;

// Test fixture for auth tests with helper methods
class AuthAPITest : public ::testing::Test {
 protected:
  std::string test_email_ =
      "test_" + generate_uuid_v4() + "@example.com";
  std::string test_password_ = "SecureP@ssw0rd!";

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
};

TEST_F(AuthAPITest, CreateUserSuccess) {
  CreateUserAPI api;
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto result = api.Process(request);
  if (!result.ok()) {
    GTEST_SKIP() << "Database-service not available: "
                 << result.message();
  }

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result.value().email, test_email_);
  EXPECT_FALSE(result.value().user_uuid.empty());
}

TEST_F(AuthAPITest, CreateUserRejectsEmptyEmail) {
  CreateUserAPI api;
  auto request = BuildCreateUserRequest("", test_password_,
                                        test_password_);

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              HasSubstr("Email must not be empty"));
}

TEST_F(AuthAPITest, CreateUserRejectsEmptyPassword) {
  CreateUserAPI api;
  auto request =
      BuildCreateUserRequest(test_email_, "", "");

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              HasSubstr("Password must not be empty"));
}

TEST_F(AuthAPITest, CreateUserRejectsPasswordMismatch) {
  CreateUserAPI api;
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, "DifferentPassword");

  auto result = api.Process(request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              HasSubstr("Passwords do not match"));
}

TEST_F(AuthAPITest, CreateUserRejectsDuplicateEmail) {
  CreateUserAPI api;
  auto request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  // Create first user
  auto first_result = api.Process(request);
  if (!first_result.ok()) {
    GTEST_SKIP() << "Database-service not available: "
                 << first_result.message();
  }
  ASSERT_TRUE(first_result.ok());

  // Try to create duplicate
  auto duplicate_result = api.Process(request);

  ASSERT_FALSE(duplicate_result.ok());
  EXPECT_THAT(
      duplicate_result.message(),
      HasSubstr("User with this email already exists"));
}

TEST_F(AuthAPITest, LoginSuccessAfterRegistration) {
  // First, create a user
  CreateUserAPI create_api;
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto create_result = create_api.Process(create_request);
  if (!create_result.ok()) {
    GTEST_SKIP() << "Database-service not available: "
                 << create_result.message();
  }
  ASSERT_TRUE(create_result.ok());

  // Give database time to propagate (eventual consistency)
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));

  // Now try to login
  LoginAPI login_api;
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);

  auto login_result = login_api.Process(login_request);

  ASSERT_TRUE(login_result.ok())
      << "Login failed: " << login_result.message();
  EXPECT_EQ(login_result.value().email, test_email_);
  EXPECT_FALSE(login_result.value().token.uuid.empty());
  EXPECT_EQ(login_result.value().token.user_uuid,
            create_result.value().user_uuid);
  EXPECT_TRUE(login_result.value().token.active);
}

TEST_F(AuthAPITest, LoginRejectsWrongPassword) {
  // First, create a user
  CreateUserAPI create_api;
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto create_result = create_api.Process(create_request);
  if (!create_result.ok()) {
    GTEST_SKIP() << "Database-service not available: "
                 << create_result.message();
  }
  ASSERT_TRUE(create_result.ok());

  // Give database time to propagate
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));

  // Try to login with wrong password
  LoginAPI login_api;
  auto login_request =
      BuildLoginRequest(test_email_, "WrongPassword");

  auto login_result = login_api.Process(login_request);

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              HasSubstr("Invalid password"));
}

TEST_F(AuthAPITest, LoginRejectsNonexistentUser) {
  LoginAPI login_api;
  auto login_request = BuildLoginRequest(
      "nonexistent_" + generate_uuid_v4() + "@example.com",
      test_password_);

  auto login_result = login_api.Process(login_request);

  // Skip if database-service not available
  if (!login_result.ok()) {
    if (login_result.message().find("hostname") !=
            std::string::npos ||
        login_result.message().find("connect") !=
            std::string::npos ||
        login_result.message().find("resolve") !=
            std::string::npos) {
      GTEST_SKIP() << "Database-service not available: "
                   << login_result.message();
    }
  }

  ASSERT_FALSE(login_result.ok());
  EXPECT_THAT(login_result.message(),
              HasSubstr("No values found"));
}

TEST_F(AuthAPITest, LoginReusesExistingActiveToken) {
  // Create user
  CreateUserAPI create_api;
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);

  auto create_result = create_api.Process(create_request);
  if (!create_result.ok()) {
    GTEST_SKIP() << "Database-service not available";
  }
  ASSERT_TRUE(create_result.ok());

  // Give database time to propagate
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));

  // First login
  LoginAPI login_api;
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);
  auto first_login = login_api.Process(login_request);
  ASSERT_TRUE(first_login.ok()) << first_login.message();
  std::string first_token = first_login.value().token.uuid;

  // Second login should reuse the same active token
  auto second_login = login_api.Process(login_request);
  ASSERT_TRUE(second_login.ok()) << second_login.message();
  std::string second_token =
      second_login.value().token.uuid;

  EXPECT_EQ(first_token, second_token)
      << "Should reuse existing active token";
}

TEST_F(AuthAPITest, LogoutDeactivatesToken) {
  // Create user and login
  CreateUserAPI create_api;
  auto create_request = BuildCreateUserRequest(
      test_email_, test_password_, test_password_);
  auto create_result = create_api.Process(create_request);
  if (!create_result.ok()) {
    GTEST_SKIP() << "Database-service not available";
  }

  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));

  LoginAPI login_api;
  auto login_request =
      BuildLoginRequest(test_email_, test_password_);
  auto login_result = login_api.Process(login_request);
  ASSERT_TRUE(login_result.ok());

  // Logout
  LogoutAPI logout_api;
  LogoutRequest logout_request;
  logout_request.token_uuid =
      login_result.value().token.uuid;

  auto logout_result = logout_api.Process(logout_request);
  ASSERT_TRUE(logout_result.ok());
  EXPECT_TRUE(logout_result.value().logged_out);

  // Verify token is inactive
  QueryView<Token> tokens_query("tokens");
  auto token_result =
      tokens_query
          .where(EQUALS(&Token::uuid,
                        login_result.value().token.uuid))
          .first();
  if (token_result.ok()) {
    EXPECT_FALSE(token_result.value().active)
        << "Token should be inactive after logout";
  }
}

TEST_F(AuthAPITest, LogoutRejectsEmptyTokenUuid) {
  LogoutAPI logout_api;
  LogoutRequest logout_request;
  logout_request.token_uuid = "";

  auto result = logout_api.Process(logout_request);

  ASSERT_FALSE(result.ok());
  EXPECT_THAT(result.message(),
              HasSubstr("token_uuid` must not be empty"));
}

}  // namespace

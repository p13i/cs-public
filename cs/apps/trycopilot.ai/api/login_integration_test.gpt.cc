// cs/apps/trycopilot.ai/api/login_integration_test.gpt.cc
#include <sstream>
#include <string>

#include "cs/apps/trycopilot.ai/api/auth.hh"
#include "cs/apps/trycopilot.ai/api/test_db_store.gpt.hh"
#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/apps/trycopilot.ai/ui/ui.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::api::testing::TestDbStore;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::ui::GetAuthenticatedUser;
using ::cs::apps::trycopilotai::ui::GetHomePage;
using ::cs::apps::trycopilotai::ui::GetIndexPage;
using ::cs::apps::trycopilotai::ui::GetToken;
using ::cs::apps::trycopilotai::ui::PostLoginPage;
using ::cs::apps::trycopilotai::ui::PostLogoutPage;
using ::cs::apps::trycopilotai::ui::PostRegisterPage;
using ::cs::net::http::kContentTypeXWwwFormUrlEncoded;
using ::cs::net::http::Request;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::Return;
}  // namespace

using AppContext = Context<IDatabaseClient>;

namespace {  // impl

std::string UrlEncode(const std::string& value) {
  std::ostringstream encoded;
  for (unsigned char c : value) {
    if (std::isalnum(c) != 0 || c == '-' || c == '_' ||
        c == '.' || c == '~') {
      encoded << static_cast<char>(c);
    } else {
      encoded << '%' << std::hex << std::uppercase
              << static_cast<unsigned>(c);
    }
  }
  return encoded.str();
}

std::string ExtractAuthTokenFromSetCookie(
    const std::string& set_cookie) {
  std::string key = "auth_token=";
  size_t start = set_cookie.find(key);
  if (start == std::string::npos) {
    return "";
  }
  start += key.length();
  size_t end = set_cookie.find(';', start);
  return (end == std::string::npos)
             ? set_cookie.substr(start)
             : set_cookie.substr(start, end - start);
}

class LoginIntegrationTest : public ::testing::Test {
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

  void CreateTestUser(const std::string& email,
                      const std::string& password) {
    CreateUserRPC create_api(app_ctx_);
    CreateUserRequest req;
    req.email = email;
    req.password = password;
    req.confirm_password = password;
    auto result = create_api.Process(req);
    ASSERT_TRUE(result.ok()) << result.message();
  }

  Request MakePostLoginRequest(
      const std::string& email,
      const std::string& password) {
    Request req;
    req._method = "POST";
    req._path = "/login/";
    req._headers["Content-Type"] =
        kContentTypeXWwwFormUrlEncoded;
    req._headers["Host"] = "www.trycopilot.ai";
    req._body = "email=" + UrlEncode(email) +
                "&password=" + UrlEncode(password);
    return req;
  }

  Request MakePostRegisterRequest(
      const std::string& email,
      const std::string& password) {
    Request req;
    req._method = "POST";
    req._path = "/register/";
    req._headers["Content-Type"] =
        kContentTypeXWwwFormUrlEncoded;
    req._headers["Host"] = "www.trycopilot.ai";
    req._body = "email=" + UrlEncode(email) +
                "&password=" + UrlEncode(password) +
                "&confirm_password=" + UrlEncode(password);
    return req;
  }

  Request MakePostLogoutRequest(
      const std::string& cookie_value) {
    Request req;
    req._method = "POST";
    req._path = "/logout/";
    req._headers["Host"] = "www.trycopilot.ai";
    if (!cookie_value.empty()) {
      req._headers["Cookie"] = "auth_token=" + cookie_value;
    }
    return req;
  }

  Request MakeGetRequest(const std::string& path,
                         const std::string& cookie_value) {
    Request req;
    req._method = "GET";
    req._path = path;
    req._headers["Host"] = "www.trycopilot.ai";
    if (!cookie_value.empty()) {
      req._headers["Cookie"] = "auth_token=" + cookie_value;
    }
    return req;
  }

  std::shared_ptr<TestDbStore> db_store_;
  std::shared_ptr<MockDatabaseClient> mock_db_;
  AppContext app_ctx_;
  std::string test_email_ =
      "test_" + generate_uuid_v4() + "@example.com";
  std::string test_password_ = "SecureP@ssw0rd!";
};

}  // namespace

TEST_F(LoginIntegrationTest,
       PostLoginWithEmptyBodyReturnsValidationError) {
  Request req;
  req._method = "POST";
  req._path = "/login/";
  req._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  req._headers["Host"] = "www.trycopilot.ai";
  req._body = "";

  auto response = PostLoginPage(req, app_ctx_);

  EXPECT_EQ(response._status.code, 400u)
      << "Expected 400 BAD REQUEST for empty form, got "
      << response._status.code;
  EXPECT_THAT(response.body(), HasSubstr("required"));
}

TEST_F(
    LoginIntegrationTest,
    PostLoginWithValidFormReturns302WithSetCookieAndLocation) {
  CreateTestUser(test_email_, test_password_);

  auto req =
      MakePostLoginRequest(test_email_, test_password_);
  auto response = PostLoginPage(req, app_ctx_);

  ASSERT_EQ(response._status.code, 302u)
      << "Expected 302 FOUND on successful login, got "
      << response._status.code;
  ASSERT_TRUE(response._headers.count("Set-Cookie") > 0)
      << "Expected Set-Cookie header in login response";
  EXPECT_THAT(response._headers.at("Set-Cookie"),
              HasSubstr("auth_token="));
  EXPECT_EQ(response._headers["Location"], "/home/");
}

TEST_F(LoginIntegrationTest, GetTokenFindsCookieInRequest) {
  Request req = MakeGetRequest("/home/", "test-token-uuid");
  ASSERT_GT(req._headers.count("Cookie"), 0u);

  auto token_or = GetToken(req);
  ASSERT_TRUE(token_or.ok())
      << "GetToken should find Cookie: "
      << token_or.message();
  EXPECT_EQ(token_or.value(), "test-token-uuid");
}

TEST_F(LoginIntegrationTest,
       GetAuthenticatedUserWithCookieSucceeds) {
  CreateTestUser(test_email_, test_password_);

  auto login_req =
      MakePostLoginRequest(test_email_, test_password_);
  auto login_response = PostLoginPage(login_req, app_ctx_);
  ASSERT_EQ(login_response._status.code, 302u);

  std::string token = ExtractAuthTokenFromSetCookie(
      login_response._headers.at("Set-Cookie"));
  ASSERT_FALSE(token.empty());

  auto home_req = MakeGetRequest("/home/", token);
  ASSERT_GT(home_req._headers.count("Cookie"), 0u);

  auto user_or = GetAuthenticatedUser(home_req, app_ctx_);
  ASSERT_TRUE(user_or.ok())
      << "GetAuthenticatedUser should succeed with cookie: "
      << user_or.message();
  EXPECT_EQ(user_or.value().email, test_email_);
}

TEST_F(LoginIntegrationTest,
       GetHomeWithoutCookieReturns403) {
  Request req = MakeGetRequest("/home/", "");

  auto response = GetHomePage(req, app_ctx_);

  EXPECT_EQ(response._status.code, 403u)
      << "Expected 403 for /home/ without auth cookie";
}

TEST_F(LoginIntegrationTest,
       GetHomeWithAuthCookieReturns200) {
  CreateTestUser(test_email_, test_password_);

  auto login_req =
      MakePostLoginRequest(test_email_, test_password_);
  auto login_response = PostLoginPage(login_req, app_ctx_);
  ASSERT_EQ(login_response._status.code, 302u);

  std::string token = ExtractAuthTokenFromSetCookie(
      login_response._headers.at("Set-Cookie"));
  ASSERT_FALSE(token.empty())
      << "Failed to extract auth_token from Set-Cookie";

  auto home_req = MakeGetRequest("/home/", token);
  ASSERT_GT(home_req._headers.count("Cookie"), 0u)
      << "Test setup: Cookie header must be set before "
         "GetHomePage";
  auto home_response = GetHomePage(home_req, app_ctx_);

  EXPECT_EQ(home_response._status.code, 200u)
      << "Expected 200 OK with valid auth cookie, got 403 "
         "- "
         "cookie not recognized";
}

TEST_F(LoginIntegrationTest, LoginFlowFullRoundtrip) {
  CreateTestUser(test_email_, test_password_);

  auto login_req =
      MakePostLoginRequest(test_email_, test_password_);
  auto login_response = PostLoginPage(login_req, app_ctx_);
  ASSERT_EQ(login_response._status.code, 302u)
      << "Login failed: " << login_response.body();

  std::string token = ExtractAuthTokenFromSetCookie(
      login_response._headers.at("Set-Cookie"));
  ASSERT_FALSE(token.empty());

  auto home_req = MakeGetRequest("/home/", token);
  auto home_response = GetHomePage(home_req, app_ctx_);
  EXPECT_EQ(home_response._status.code, 200u)
      << "GET /home/ with cookie returned "
      << home_response._status.code;

  auto index_req = MakeGetRequest("/", token);
  auto index_response = GetIndexPage(index_req, app_ctx_);
  EXPECT_EQ(index_response._status.code, 200u);
  EXPECT_THAT(index_response.body(), HasSubstr("Logout"))
      << "Index page should show Logout when logged in";
}

TEST_F(LoginIntegrationTest,
       RegisterHomeLogoutIndexFlow_Home403AfterLogout) {
  auto register_req =
      MakePostRegisterRequest(test_email_, test_password_);
  auto register_response =
      PostRegisterPage(register_req, app_ctx_);
  ASSERT_EQ(register_response._status.code, 200u)
      << "Registration failed: "
      << register_response.body();
  ASSERT_TRUE(
      register_response._headers.count("Set-Cookie") > 0)
      << "Expected Set-Cookie after registration";

  std::string token = ExtractAuthTokenFromSetCookie(
      register_response._headers.at("Set-Cookie"));
  ASSERT_FALSE(token.empty());

  auto home_req = MakeGetRequest("/home/", token);
  auto home_response = GetHomePage(home_req, app_ctx_);
  EXPECT_EQ(home_response._status.code, 200u)
      << "GET /home/ after register should succeed";

  auto logout_req = MakePostLogoutRequest(token);
  auto logout_response =
      PostLogoutPage(logout_req, app_ctx_);
  ASSERT_EQ(logout_response._status.code, 302u)
      << "POST /logout/ should redirect";

  auto home_after_logout = MakeGetRequest("/home/", "");
  auto home_after_response =
      GetHomePage(home_after_logout, app_ctx_);
  EXPECT_TRUE(home_after_response._status.code == 403u ||
              home_after_response._status.code == 404u)
      << "GET /home/ after logout must return 403 or 404, "
         "got "
      << home_after_response._status.code;

  auto index_req = MakeGetRequest("/", "");
  auto index_response = GetIndexPage(index_req, app_ctx_);
  EXPECT_EQ(index_response._status.code, 200u)
      << "GET / (index) after logout should succeed";
}

TEST_F(LoginIntegrationTest,
       LoginHomeLogoutIndexFlow_Home403AfterLogout) {
  CreateTestUser(test_email_, test_password_);

  auto login_req =
      MakePostLoginRequest(test_email_, test_password_);
  auto login_response = PostLoginPage(login_req, app_ctx_);
  ASSERT_EQ(login_response._status.code, 302u)
      << "Login failed: " << login_response.body();

  std::string token = ExtractAuthTokenFromSetCookie(
      login_response._headers.at("Set-Cookie"));
  ASSERT_FALSE(token.empty());

  auto home_req = MakeGetRequest("/home/", token);
  auto home_response = GetHomePage(home_req, app_ctx_);
  EXPECT_EQ(home_response._status.code, 200u)
      << "GET /home/ after login should succeed";

  auto logout_req = MakePostLogoutRequest(token);
  auto logout_response =
      PostLogoutPage(logout_req, app_ctx_);
  ASSERT_EQ(logout_response._status.code, 302u)
      << "POST /logout/ should redirect";

  auto home_after_logout = MakeGetRequest("/home/", "");
  auto home_after_response =
      GetHomePage(home_after_logout, app_ctx_);
  EXPECT_TRUE(home_after_response._status.code == 403u ||
              home_after_response._status.code == 404u)
      << "GET /home/ after logout must return 403 or 404, "
         "got "
      << home_after_response._status.code;

  auto index_req = MakeGetRequest("/", "");
  auto index_response = GetIndexPage(index_req, app_ctx_);
  EXPECT_EQ(index_response._status.code, 200u)
      << "GET / (index) after logout should succeed";
}

TEST_F(LoginIntegrationTest,
       LoginWithCharsetInContentType) {
  CreateTestUser(test_email_, test_password_);

  auto login_req =
      MakePostLoginRequest(test_email_, test_password_);
  login_req._headers["Content-Type"] =
      "application/x-www-form-urlencoded; charset=utf-8";

  auto login_response = PostLoginPage(login_req, app_ctx_);
  ASSERT_EQ(login_response._status.code, 302u)
      << "Login with charset in Content-Type should "
         "succeed, got "
      << login_response._status.code << ": "
      << login_response.body();

  ASSERT_TRUE(login_response._headers.count("Set-Cookie") >
              0)
      << "Expected Set-Cookie header in login response";
  EXPECT_THAT(login_response._headers.at("Set-Cookie"),
              HasSubstr("auth_token="));
  EXPECT_EQ(login_response._headers["Location"], "/home/");
}

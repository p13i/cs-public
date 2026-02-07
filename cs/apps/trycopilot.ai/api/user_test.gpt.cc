// cs/apps/trycopilot.ai/api/user_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/user.hh"

#include <string>

#include "cs/apps/trycopilot.ai/api/test_db_store.gpt.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::Return;
}  // namespace

using AppContext = Context<IDatabaseClient>;
using TestDbStore =
    cs::apps::trycopilotai::api::testing::TestDbStore;

class CreateUserTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_store_ = std::make_shared<TestDbStore>();
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
    ctx_ =
        ContextBuilder<AppContext>()
            .bind<IDatabaseClient>()
            .from([this](AppContext&) { return mock_db_; })
            .build();
  }

  std::shared_ptr<TestDbStore> db_store_;
  std::shared_ptr<MockDatabaseClient> mock_db_;
  AppContext ctx_ = AppContext{};
};

TEST_F(CreateUserTest, EmptyEmailFails) {
  CreateUserRequest req;
  req.password = "pass123";
  req.confirm_password = "pass123";
  auto rpc = CreateUserRPC(ctx_);
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.message(), HasSubstr("Email"));
}

TEST_F(CreateUserTest, EmptyPasswordFails) {
  CreateUserRequest req;
  req.email = "test@example.com";
  auto rpc = CreateUserRPC(ctx_);
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.message(), HasSubstr("Password"));
}

TEST_F(CreateUserTest, MismatchedPasswordFails) {
  CreateUserRequest req;
  req.email = "test@example.com";
  req.password = "pass123";
  req.confirm_password = "pass456";
  auto rpc = CreateUserRPC(ctx_);
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.message(), HasSubstr("do not match"));
}

TEST_F(CreateUserTest, SuccessfulCreate) {
  CreateUserRequest req;
  req.email = "alice@example.com";
  req.password = "secret";
  req.confirm_password = "secret";
  auto rpc = CreateUserRPC(ctx_);
  auto result = rpc.Process(req);
  ASSERT_TRUE(result.ok()) << result.message();
  EXPECT_EQ(result.value().email, "alice@example.com");
  EXPECT_FALSE(result.value().user_uuid.empty());
}

TEST_F(CreateUserTest, DuplicateEmailFails) {
  CreateUserRequest req;
  req.email = "dup@example.com";
  req.password = "secret";
  req.confirm_password = "secret";
  {
    auto rpc = CreateUserRPC(ctx_);
    auto result = rpc.Process(req);
    ASSERT_TRUE(result.ok()) << result.message();
  }
  {
    auto rpc = CreateUserRPC(ctx_);
    auto result = rpc.Process(req);
    EXPECT_FALSE(result.ok());
    EXPECT_THAT(result.message(),
                HasSubstr("already exists"));
  }
}

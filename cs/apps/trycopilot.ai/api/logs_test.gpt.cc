// cs/apps/trycopilot.ai/api/logs_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/logs.hh"

#include <set>
#include <string>
#include <vector>

#include "cs/apps/trycopilot.ai/api/test_db_store.gpt.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/log.proto.hh"
#include "cs/apps/trycopilot.ai/protos/log.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::CreateLogRPC;
using ::cs::apps::trycopilotai::api::ListAppLogsRPC;
using ::cs::apps::trycopilotai::api::ListLogsRPC;
using ::cs::apps::trycopilotai::api::testing::TestDbStore;
using ::cs::apps::trycopilotai::protos::AppLog;
using ::cs::apps::trycopilotai::protos::CreateLogRequest;
using ::cs::apps::trycopilotai::protos::HttpLog;
using ::cs::apps::trycopilotai::protos::ListAppLogsRequest;
using ::cs::apps::trycopilotai::protos::ListLogsRequest;
using ::cs::apps::trycopilotai::protos::gencode::log::
    HttpLogBuilderImpl;
using ::cs::apps::trycopilotai::protos::gencode::log::
    HttpRequestBuilderImpl;
using ::cs::apps::trycopilotai::protos::gencode::log::
    HttpResponseBuilderImpl;
using ::cs::apps::trycopilotai::protos::gencode::log::
    HttpStatusBuilderImpl;
using ::cs::apps::trycopilotai::protos::gencode::log::
    TimestampBuilderImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::testing::Invoke;
using ::testing::Return;
}  // namespace

using AppContext = Context<IDatabaseClient>;

// Test fixture with mocked DB and shared context.
class LogsAPITest : public ::testing::Test {
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

  CreateLogRequest BuildLogRequest(
      const std::string& path, int status_code,
      const std::string& body) {
    CreateLogRequest request;
    request.request.path = path;
    request.request.method = "GET";
    request.request.receive_timestamp.iso8601 =
        "2024-01-01T00:00:00Z";
    request.response.status.code = status_code;
    request.response.status.name = "OK";
    request.response.content_type = "text/plain";
    request.response.content_length = body.size();
    request.response.body = body;
    request.response.send_timestamp.iso8601 =
        "2024-01-01T00:00:01Z";
    return request;
  }

  std::shared_ptr<TestDbStore> db_store_;
  std::shared_ptr<MockDatabaseClient> mock_db_;
  AppContext app_ctx_;
};

TEST_F(LogsAPITest, CreateLogHandlesFavicon) {
  CreateLogRequest request =
      BuildLogRequest("/favicon.ico", 200, "ignored");
  CreateLogRPC api(app_ctx_);
  auto result = api.Process(request);
  ASSERT_TRUE(result.ok()) << result.message();
  EXPECT_FALSE(result.value().http_log_uuid.empty());

  auto db = app_ctx_.Get<IDatabaseClient>();
  QueryView<HttpLog> query("http_logs", db);
  auto values = query.values();
  ASSERT_TRUE(values.ok()) << values.message();
  ASSERT_FALSE(values.value().empty());

  bool found = false;
  for (const auto& log : values.value()) {
    if (log.request.path == "/favicon.ico") {
      EXPECT_EQ(log.response.body, "favicon");
      found = true;
      break;
    }
  }
  ASSERT_TRUE(found)
      << "Expected favicon log entry not found";
}

TEST_F(LogsAPITest, ListLogsReturnsLogs) {
  auto db = app_ctx_.Get<IDatabaseClient>();
  // Populate fixtures: one log entry.
  auto normal_log =
      HttpLogBuilderImpl()
          .set_uuid(generate_uuid_v4())
          .set_request(
              HttpRequestBuilderImpl()
                  .set_path("/index")
                  .set_receive_timestamp(
                      TimestampBuilderImpl()
                          .set_iso8601(
                              "2024-01-01T00:00:01Z")
                          .Build())
                  .Build())
          .set_response(
              HttpResponseBuilderImpl()
                  .set_body("content")
                  .set_status(HttpStatusBuilderImpl()
                                  .set_code(200)
                                  .Build())
                  .Build())
          .Build();
  ASSERT_TRUE(Upsert("http_logs", normal_log, *db).ok());

  ListLogsRPC list_api(app_ctx_);
  ListLogsRequest list_request;
  auto list_result = list_api.Process(list_request);
  ASSERT_TRUE(list_result.ok()) << list_result.message();

  ASSERT_GE(list_result.value().http_logs.size(), 1u);
  EXPECT_EQ(list_result.value().http_logs[0].request.path,
            "/index");
  EXPECT_EQ(list_result.value().count,
            static_cast<int>(
                list_result.value().http_logs.size()));
}

TEST_F(LogsAPITest, ListAppLogsOrdersByTimestamp) {
  auto db = app_ctx_.Get<IDatabaseClient>();
  AppLog older;
  older.timestamp = "2024-01-01T00:00:00Z";
  older.level = "info";
  older.file = "main.cc";
  older.line = 10;
  older.message = "old";

  AppLog newer;
  newer.timestamp = "2024-01-02T00:00:00Z";
  newer.level = "info";
  newer.file = "main.cc";
  newer.line = 11;
  newer.message = "new";

  ASSERT_TRUE(Upsert("app_logs", older, *db).ok());
  ASSERT_TRUE(Upsert("app_logs", newer, *db).ok());

  ListAppLogsRPC api(app_ctx_);
  ListAppLogsRequest request;
  auto result = api.Process(request);
  ASSERT_TRUE(result.ok()) << result.message();

  ASSERT_EQ(result.value().items.size(), 2u);
  // Both fixtures should be present. Order depends on
  // field_paths for AppLog (not yet generated).
  std::set<std::string> messages = {"old", "new"};
  for (const auto& item : result.value().items) {
    messages.erase(item.message);
  }
  EXPECT_TRUE(messages.empty()) << "Expected both fixtures";
}

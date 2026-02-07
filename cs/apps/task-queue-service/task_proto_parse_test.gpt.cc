// cs/apps/task-queue-service/task_proto_parse_test.gpt.cc
#include <string>

#include "cs/apps/task-queue-service/protos/task.gpt.proto.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::task_queue_service::protos::Task;
using ::cs::net::json::parsers::ParseObject;
using ::testing::HasSubstr;
}  // namespace

namespace {

TEST(TaskParseTest, ValidTaskParseSucceeds) {
  std::string json =
      R"({"task_id":"550e8400-e29b-41d4-a716-446655440000","task_name":"x","args":{}})";
  auto result = Task().Parse(json);
  ASSERT_OK(result);
  EXPECT_EQ(result.value().task_id,
            "550e8400-e29b-41d4-a716-446655440000");
  EXPECT_EQ(result.value().task_name, "x");
  EXPECT_EQ(result.value().args.str(), "{}");
}

TEST(TaskParseTest, ParseThenSerializeRoundTrips) {
  Task original;
  original.task_id = "123e4567-e89b-12d3-a456-426614174000";
  original.task_name = "round.trip";
  {
    auto args_or = ParseObject(R"({"a":1,"b":2})");
    ASSERT_TRUE(args_or.ok()) << args_or.message();
    original.args = args_or.value();
  }

  std::string serialized = original.Serialize();
  auto parsed = Task().Parse(serialized);
  ASSERT_OK(parsed);

  EXPECT_EQ(parsed.value().task_id, original.task_id);
  EXPECT_EQ(parsed.value().task_name, original.task_name);
  EXPECT_EQ(parsed.value().args.str(), original.args.str());
}

TEST(TaskParseTest, InvalidJsonFailsParse) {
  auto result = Task().Parse("not valid json");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, EmptyStringFailsParse) {
  auto result = Task().Parse("");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, MissingFieldsParseWithDefaults) {
  std::string json = R"({"task_name":"t","args":{}})";
  auto result = Task().Parse(json);
  ASSERT_OK(result);
  EXPECT_EQ(result.value().task_id, "");
  EXPECT_EQ(result.value().task_name, "t");
  EXPECT_EQ(result.value().args.str(), "{}");
}

TEST(TaskParseTest,
     EmptyObjectParsesWithDefaultFieldValues) {
  auto result = Task().Parse("{}");
  ASSERT_OK(result);
  EXPECT_EQ(result.value().task_id, "");
  EXPECT_EQ(result.value().task_name, "");
  EXPECT_EQ(result.value().args.str(), "null");
}

TEST(TaskParseTest, NonObjectJsonFailsParse) {
  auto result = Task().Parse("[]");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, NullJsonFailsParse) {
  auto result = Task().Parse("null");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, StringJsonFailsParse) {
  auto result = Task().Parse(R"("hello")");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, NumberJsonFailsParse) {
  auto result = Task().Parse("42");
  ASSERT_NOK(result);
}

TEST(TaskParseTest, TaskWithUnicodeInArgsParses) {
  std::string json =
      R"({"task_id":"550e8400-e29b-41d4-a716-446655440000","task_name":"u","args":{"msg":"café 日本語"}})";
  auto result = Task().Parse(json);
  ASSERT_OK(result);
  EXPECT_THAT(result.value().args.str(), HasSubstr("café"));
}

TEST(TaskParseTest, TaskWithNestedJsonInArgsParses) {
  std::string json =
      R"({"task_id":"550e8400-e29b-41d4-a716-446655440000","task_name":"n","args":{"outer":{"inner":["a","b"]}}})";
  auto result = Task().Parse(json);
  ASSERT_OK(result);
  EXPECT_EQ(result.value().args.str(),
            R"({"outer":{"inner":["a","b"]}})");
}

TEST(TaskParseTest, UuidValidationAcceptsValidUuid) {
  std::string json =
      R"({"task_id":"a1b2c3d4-e5f6-7890-abcd-ef1234567890","task_name":"t","args":{}})";
  auto result = Task().Parse(json);
  ASSERT_OK(result);
  EXPECT_EQ(result.value().task_id,
            "a1b2c3d4-e5f6-7890-abcd-ef1234567890");
}

}  // namespace

// cs/apps/task-queue-service/task_contract_test.gpt.cc
// Tests the Task payload format and its use as
// message-queue payload.
#include <string>

#include "cs/apps/task-queue-service/protos/task.gpt.proto.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/testing/extensions.gpt.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::task_queue_service::protos::Task;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::testing::HasSubstr;
}  // namespace

namespace {

TEST(TaskContractTest, TaskSerializeParseRoundTrips) {
  Task task;
  task.task_id = cs::util::random::uuid::generate_uuid_v4();
  task.task_name = "my.task";
  {
    auto args_or = ParseObject(R"({"foo":42,"bar":"baz"})");
    ASSERT_TRUE(args_or.ok()) << args_or.message();
    task.args = args_or.value();
  }

  std::string serialized = task.Serialize();
  auto parsed = Task().Parse(serialized);
  ASSERT_OK(parsed);

  EXPECT_EQ(parsed.value().task_id, task.task_id);
  EXPECT_EQ(parsed.value().task_name, task.task_name);
  EXPECT_EQ(parsed.value().args.str(), task.args.str());
}

TEST(TaskContractTest,
     TaskPayloadSuitableForEnqueueRequestPayload) {
  Task task;
  task.task_id = cs::util::random::uuid::generate_uuid_v4();
  task.task_name = "contract.test";
  {
    auto args_or = ParseObject(R"({"x":1})");
    ASSERT_TRUE(args_or.ok()) << args_or.message();
    task.args = args_or.value();
  }

  std::string json_payload = task.Serialize();
  EXPECT_FALSE(json_payload.empty());
  EXPECT_THAT(json_payload, HasSubstr("task_id"));
  EXPECT_THAT(json_payload, HasSubstr("task_name"));
  EXPECT_THAT(json_payload, HasSubstr("args"));

  auto parsed = Task().Parse(json_payload);
  ASSERT_OK(parsed);
  EXPECT_EQ(parsed.value().task_name, "contract.test");
}

TEST(TaskContractTest, TaskNameAsMessageTypeConvention) {
  Task task;
  task.task_id = cs::util::random::uuid::generate_uuid_v4();
  task.task_name = "custom.type";
  task.args = Object();

  EXPECT_EQ(task.task_name, "custom.type");
}

TEST(TaskContractTest, TaskWithEmptyArgsRoundTrips) {
  Task task;
  task.task_id = cs::util::random::uuid::generate_uuid_v4();
  task.task_name = "noargs";
  task.args = Object();

  std::string serialized = task.Serialize();
  auto parsed = Task().Parse(serialized);
  ASSERT_OK(parsed);
  EXPECT_EQ(parsed.value().args.str(), task.args.str());
}

TEST(TaskContractTest,
     TaskWithSpecialCharactersInArgsRoundTrips) {
  Task task;
  task.task_id = cs::util::random::uuid::generate_uuid_v4();
  task.task_name = "special";
  {
    auto args_or = ParseObject(
        R"({"quote":"\"","backslash":"\\","newline":"\n"})");
    ASSERT_TRUE(args_or.ok()) << args_or.message();
    task.args = args_or.value();
  }

  std::string serialized = task.Serialize();
  auto parsed = Task().Parse(serialized);
  ASSERT_OK(parsed);
  EXPECT_EQ(parsed.value().args.str(), task.args.str());
}

}  // namespace

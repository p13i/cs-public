// cs/apps/task-queue-service/task_client_test.gpt.cc
// Unit tests for Task + EnqueueRequest construction
// (SendTask logic).
#include <string>

#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/apps/task-queue-service/protos/task.gpt.proto.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::message_queue::protos::EnqueueRequest;
using ::cs::apps::message_queue::protos::Message;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

namespace cs::apps::task_queue_service {
namespace {

TEST(TaskClientTest, EachTaskGetsUniqueId) {
  protos::Task t1;
  t1.task_id = cs::util::random::uuid::generate_uuid_v4();
  t1.task_name = "same";
  t1.args = Object();

  protos::Task t2;
  t2.task_id = cs::util::random::uuid::generate_uuid_v4();
  t2.task_name = "same";
  t2.args = Object();

  EXPECT_NE(t1.task_id, t2.task_id);
}

TEST(TaskClientTest, EnqueueRequestTypeMatchesTaskName) {
  protos::Task task;
  task.task_id = "550e8400-e29b-41d4-a716-446655440000";
  task.task_name = "dot.separated.name";
  task.args = Object();

  Message message;
  message.uuid = "550e8400-e29b-41d4-a716-446655440001";
  {
    auto parsed = ParseObject(task.Serialize());
    ASSERT_TRUE(parsed.ok()) << parsed.message();
    message.payload = parsed.value();
  }
  message.timestamp = NowAsISO8601TimeUTC();
  message.status = "pending";
  message.type = task.task_name;
  message.consumer_service = "";

  EnqueueRequest request;
  request.message = message;

  EXPECT_EQ(request.message.type, "dot.separated.name");
  EXPECT_THAT(request.message.payload.str(),
              ::testing::HasSubstr("dot.separated.name"));
}

TEST(TaskClientTest,
     EnqueueRequestJsonPayloadIsTaskSerialization) {
  protos::Task task;
  task.task_id = "a1b2c3d4-e5f6-7890-abcd-ef1234567890";
  task.task_name = "verify";
  {
    auto args_or = ParseObject(R"({"k":1})");
    ASSERT_TRUE(args_or.ok()) << args_or.message();
    task.args = args_or.value();
  }

  Message message;
  message.uuid = "a1b2c3d4-e5f6-7890-abcd-ef1234567891";
  {
    auto payload_parsed = ParseObject(task.Serialize());
    ASSERT_TRUE(payload_parsed.ok())
        << payload_parsed.message();
    message.payload = payload_parsed.value();
  }
  message.timestamp = NowAsISO8601TimeUTC();
  message.status = "pending";
  message.type = task.task_name;
  message.consumer_service = "";

  EnqueueRequest request;
  request.message = message;

  auto parsed =
      protos::Task().Parse(request.message.payload.str());
  ASSERT_OK(parsed);
  EXPECT_EQ(parsed.value().task_id, task.task_id);
  EXPECT_EQ(parsed.value().task_name, task.task_name);
  EXPECT_EQ(parsed.value().args.str(), task.args.str());
}

}  // namespace
}  // namespace cs::apps::task_queue_service

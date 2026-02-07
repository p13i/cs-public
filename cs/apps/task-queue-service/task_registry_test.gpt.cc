// cs/apps/task-queue-service/task_registry_test.gpt.cc
#include <string>

#include "cs/apps/task-queue-service/task_worker.gpt.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::apps::task_queue_service::TaskHandler;
using ::cs::apps::task_queue_service::TaskRegistry;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::testing::HasSubstr;
}  // namespace

namespace {

TEST(TaskRegistryTest,
     RegisterTaskWithValidHandlerSucceeds) {
  TaskRegistry registry;
  Result r = registry.RegisterTask(
      "my_task", [](const Object&) { return Ok(); });
  ASSERT_OK(r);
  EXPECT_TRUE(registry.HasTask("my_task"));
}

TEST(TaskRegistryTest, RegisterTaskWithEmptyNameSucceeds) {
  TaskRegistry registry;
  Result r = registry.RegisterTask(
      "", [](const Object&) { return Ok(); });
  ASSERT_OK(r);
  EXPECT_TRUE(registry.HasTask(""));
}

TEST(TaskRegistryTest, RegisterTaskWithNullHandlerFails) {
  TaskRegistry registry;
  TaskHandler null_handler;
  Result r = registry.RegisterTask("task",
                                   std::move(null_handler));
  ASSERT_NOK(r);
  EXPECT_THAT(r.message(),
              HasSubstr("Task handler must be callable"));
  EXPECT_FALSE(registry.HasTask("task"));
}

TEST(TaskRegistryTest, RegisterTaskTwiceWithSameNameFails) {
  TaskRegistry registry;
  ASSERT_OK(registry.RegisterTask(
      "dup", [](const Object&) { return Ok(); }));
  Result r = registry.RegisterTask(
      "dup", [](const Object&) { return Ok(); });
  ASSERT_NOK(r);
  EXPECT_THAT(r.message(),
              HasSubstr("Task already registered"));
  EXPECT_THAT(r.message(), HasSubstr("dup"));
}

TEST(TaskRegistryTest,
     HasTaskReturnsFalseForUnregisteredTask) {
  TaskRegistry registry;
  EXPECT_FALSE(registry.HasTask("nonexistent"));
  EXPECT_FALSE(registry.HasTask(""));
}

TEST(TaskRegistryTest,
     HasTaskReturnsTrueForRegisteredTask) {
  TaskRegistry registry;
  ASSERT_OK(registry.RegisterTask(
      "a", [](const Object&) { return Ok(); }));
  EXPECT_TRUE(registry.HasTask("a"));
}

TEST(TaskRegistryTest,
     DispatchCallsHandlerAndReturnsResult) {
  TaskRegistry registry;
  Object captured;
  ASSERT_OK(registry.RegisterTask(
      "capture", [&captured](const Object& args) {
        captured = args;
        return Ok();
      }));

  auto args_or = ParseObject(R"({"x":1})");
  ASSERT_TRUE(args_or.ok()) << args_or.message();
  Result r = registry.Dispatch("capture", args_or.value());
  ASSERT_OK(r);
  EXPECT_EQ(captured.str(), args_or.value().str());
}

TEST(TaskRegistryTest,
     DispatchReturnsNotFoundErrorForUnregisteredTask) {
  TaskRegistry registry;
  Result r = registry.Dispatch("missing", Object());
  ASSERT_NOK(r);
  EXPECT_THAT(r.message(),
              HasSubstr("Task handler missing"));
  EXPECT_THAT(r.message(), HasSubstr("missing"));
}

TEST(TaskRegistryTest, DispatchPropagatesHandlerFailure) {
  TaskRegistry registry;
  ASSERT_OK(
      registry.RegisterTask("failing", [](const Object&) {
        return Error("handler failed");
      }));

  Result r = registry.Dispatch("failing", Object());
  ASSERT_NOK(r);
  EXPECT_THAT(r.message(), HasSubstr("handler failed"));
}

TEST(TaskRegistryTest,
     MultipleHandlersRegisteredIndependently) {
  TaskRegistry registry;
  ASSERT_OK(registry.RegisterTask(
      "a", [](const Object&) { return Ok(); }));
  ASSERT_OK(registry.RegisterTask(
      "b", [](const Object&) { return Ok(); }));
  ASSERT_OK(registry.RegisterTask(
      "c", [](const Object&) { return Ok(); }));

  EXPECT_TRUE(registry.HasTask("a"));
  EXPECT_TRUE(registry.HasTask("b"));
  EXPECT_TRUE(registry.HasTask("c"));
  EXPECT_FALSE(registry.HasTask("d"));

  EXPECT_OK(registry.Dispatch("a", Object()));
  EXPECT_OK(registry.Dispatch("b", Object()));
  EXPECT_OK(registry.Dispatch("c", Object()));
  EXPECT_NOK(registry.Dispatch("d", Object()));
}

TEST(TaskRegistryTest, DispatchWithEmptyArgs) {
  TaskRegistry registry;
  Object received;
  ASSERT_OK(registry.RegisterTask(
      "empty", [&received](const Object& args) {
        received = args;
        return Ok();
      }));

  Result r = registry.Dispatch("empty", Object());
  ASSERT_OK(r);
  EXPECT_EQ(received.str(), "null");
}

TEST(TaskRegistryTest, DispatchWithComplexJsonArgs) {
  TaskRegistry registry;
  Object received;
  ASSERT_OK(registry.RegisterTask(
      "complex", [&received](const Object& args) {
        received = args;
        return Ok();
      }));

  auto args_or = ParseObject(
      R"({"nested":{"a":1,"b":[2,3]},"utf8":"café"})");
  ASSERT_TRUE(args_or.ok()) << args_or.message();
  Result r = registry.Dispatch("complex", args_or.value());
  ASSERT_OK(r);
  EXPECT_EQ(received.str(), args_or.value().str());
}

TEST(TaskRegistryTest, DispatchIsConst) {
  TaskRegistry registry;
  ASSERT_OK(registry.RegisterTask(
      "t", [](const Object&) { return Ok(); }));

  const TaskRegistry& cref = registry;
  Result r = cref.Dispatch("t", Object());
  ASSERT_OK(r);
}

TEST(TaskRegistryTest, HasTaskIsConst) {
  TaskRegistry registry;
  ASSERT_OK(registry.RegisterTask(
      "t", [](const Object&) { return Ok(); }));

  const TaskRegistry& cref = registry;
  EXPECT_TRUE(cref.HasTask("t"));
  EXPECT_FALSE(cref.HasTask("other"));
}

TEST(TaskRegistryTest, DispatchWithVeryLongArgs) {
  TaskRegistry registry;
  Object long_args;
  long_args.set("x", Object(std::string(10000, 'x')));
  Object received;
  ASSERT_OK(registry.RegisterTask(
      "long", [&received](const Object& args) {
        received = args;
        return Ok();
      }));

  Result r = registry.Dispatch("long", long_args);
  ASSERT_OK(r);
  EXPECT_EQ(received.str(), long_args.str());
}

TEST(TaskRegistryTest, DispatchWithNewlinesInArgs) {
  TaskRegistry registry;
  auto args_or = ParseObject("{\n  \"a\": 1\n}\n");
  ASSERT_TRUE(args_or.ok()) << args_or.message();
  Object received;
  ASSERT_OK(registry.RegisterTask(
      "nl", [&received](const Object& a) {
        received = a;
        return Ok();
      }));

  Result r = registry.Dispatch("nl", args_or.value());
  ASSERT_OK(r);
  EXPECT_EQ(received.str(), args_or.value().str());
}

}  // namespace

// cs/apps/task-queue-service/task_worker.gpt.cc
#include "cs/apps/task-queue-service/task_worker.gpt.hh"

#include <chrono>
#include <thread>
#include <utility>

#include "cs/apps/message-queue/protos/gencode/queue.proto.hh"
#include "cs/apps/message-queue/protos/gencode/queue.validate.hh"
#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/apps/message-queue/rpc.gpt.hh"
#include "cs/apps/task-queue-service/protos/gencode/task.gpt.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/task.gpt.validate.hh"
#include "cs/apps/task-queue-service/protos/task.gpt.proto.hh"
#include "cs/log.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/net/rpc/client.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::message_queue::protos::DequeueRequest;
using ::cs::apps::message_queue::protos::DequeueResponse;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::DequeueResponseRules;
using ::cs::apps::message_queue::rpc::DequeueRPC;
using ::cs::apps::task_queue_service::protos::Task;
using ::cs::apps::task_queue_service::protos::gencode::
    task_gpt::validation_generated::TaskRules;
using ::cs::net::json::Object;
using ::cs::net::proto::validation::Validate;
using ::cs::net::rpc::RPCClient;
}  // namespace

namespace cs::apps::task_queue_service {

Result RegisterBuiltInTasks(TaskRegistry& registry) {
  return registry.RegisterTask(
      "debug.echo", [](const Object& args) -> Result {
        LOG(INFO) << "[debug.echo] args=" << args.str()
                  << ENDL;
        return Ok();
      });
}

Result TaskRegistry::RegisterTask(std::string task_name,
                                  TaskHandler handler) {
  if (!handler) {
    return InvalidArgument("Task handler must be callable");
  }
  auto [it, inserted] = handlers_.emplace(
      std::move(task_name), std::move(handler));
  if (!inserted) {
    return InvalidArgument("Task already registered: " +
                           it->first);
  }
  return Ok();
}

bool TaskRegistry::HasTask(
    const std::string& task_name) const {
  return handlers_.find(task_name) != handlers_.end();
}

Result TaskRegistry::Dispatch(const std::string& task_name,
                              const Object& args) const {
  auto it = handlers_.find(task_name);
  if (it == handlers_.end()) {
    return NotFoundError("Task handler missing: " +
                         task_name);
  }
  return it->second(args);
}

Result RunWorker(const std::string& base_url,
                 TaskRegistry& registry,
                 const WorkerOptions& options) {
  RPCClient<DequeueRPC> client(base_url);
  DequeueRequest request;
  request.consumer_service = options.consumer_service;
  int iterations = 0;

  while (true) {
    if (options.max_iterations.has_value() &&
        iterations >= options.max_iterations.value()) {
      return Ok();
    }
    ++iterations;

    SET_OR_RET(
        auto response,
        client.Call(options.dequeue_endpoint, request));
    if (response.has_message) {
      OK_OR_RET(Validate(response, DequeueResponseRules{}));
    }

    if (!response.has_message) {
      if (options.empty_poll_sleep_ms > 0) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                options.empty_poll_sleep_ms));
      }
      continue;
    }

    SET_OR_RET(
        Task task,
        Task().Parse(response.message.payload.str()));
    OK_OR_RET(Validate(task, TaskRules{}));

    Result dispatch =
        registry.Dispatch(task.task_name, task.args);
    if (!dispatch.ok()) {
      LOG(ERROR) << FMT("Task %s failed: %s",
                        task.task_name.c_str(),
                        dispatch.message().c_str())
                 << ENDL;
      continue;
    }
  }
}

}  // namespace cs::apps::task_queue_service

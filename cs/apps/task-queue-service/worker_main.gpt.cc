// cs/apps/task-queue-service/worker_main.gpt.cc
#include <string>
#include <vector>

#include "cs/apps/task-queue-service/protos/config.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/config.validate.hh"
#include "cs/apps/task-queue-service/task_worker.gpt.hh"
#include "cs/log.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Ok;
using ::cs::Result;
using ::cs::apps::task_queue_service::RunWorker;
using ::cs::apps::task_queue_service::TaskRegistry;
using ::cs::apps::task_queue_service::WorkerOptions;
using ::cs::apps::task_queue_service::protos::WorkerConfig;
using ::cs::apps::task_queue_service::protos::gencode::
    config::validation_generated::WorkerConfigRules;
using ::cs::net::json::Object;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
}  // namespace

namespace {  // helpers
Result RegisterBuiltInTasks(TaskRegistry& registry) {
  return registry.RegisterTask(
      "debug.echo", [](const Object& args) -> Result {
        LOG(INFO) << "[debug.echo] args=" << args.str()
                  << ENDL;
        return Ok();
      });
}
}  // namespace

namespace cs::apps::task_queue_service {

Result RunTaskWorker(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<WorkerConfig>(argv));
  OK_OR_RET(Validate(config, WorkerConfigRules{}));

  TaskRegistry registry;
  OK_OR_RET(RegisterBuiltInTasks(registry));

  WorkerOptions options;
  options.consumer_service = config.consumer_service;
  options.dequeue_endpoint = config.dequeue_endpoint.empty()
                                 ? "/rpc/dequeue/"
                                 : config.dequeue_endpoint;

  if (config.empty_poll_sleep_ms > 0) {
    options.empty_poll_sleep_ms =
        config.empty_poll_sleep_ms;
  }
  if (config.max_iterations > 0) {
    options.max_iterations = config.max_iterations;
  }

  return RunWorker(config.broker_base_url, registry,
                   options);
}

}  // namespace cs::apps::task_queue_service

int main(int argc, char** argv) {
  return cs::Result::Main(
      argc, argv,
      cs::apps::task_queue_service::RunTaskWorker);
}

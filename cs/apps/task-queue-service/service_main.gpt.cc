// cs/apps/task-queue-service/service_main.gpt.cc
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/task-queue-service/protos/config.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/config.validate.hh"
#include "cs/apps/task-queue-service/rpc.gpt.hh"
#include "cs/apps/task-queue-service/task_worker.gpt.hh"
#include "cs/log.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::Ok;
using ::cs::Result;
using ::cs::apps::common::GetHealth;
using ::cs::apps::task_queue_service::RegisterBuiltInTasks;
using ::cs::apps::task_queue_service::RunWorker;
using ::cs::apps::task_queue_service::TaskRegistry;
using ::cs::apps::task_queue_service::WorkerOptions;
using ::cs::apps::task_queue_service::protos::WorkerConfig;
using ::cs::apps::task_queue_service::protos::gencode::
    config::validation_generated::WorkerConfigRules;
using ::cs::apps::task_queue_service::rpc::CreateTaskRPC;
using ::cs::apps::task_queue_service::rpc::GetTaskStatusRPC;
using ::cs::apps::task_queue_service::rpc::SystemStatusRPC;
using ::cs::apps::task_queue_service::rpc::TaskAppContext;
using ::cs::apps::task_queue_service::rpc::TaskRpcContext;
using ::cs::net::http::WebApp;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::ContextBuilder;
}  // namespace

Result RunTaskQueueService(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<WorkerConfig>(argv));
  OK_OR_RET(Validate(config, WorkerConfigRules{}));

  auto tctx = std::make_shared<TaskRpcContext>();
  tctx->broker_base_url = config.broker_base_url;
  if (!config.database_base_url.empty()) {
    tctx->db = std::make_shared<DatabaseClientImpl>(
        config.database_base_url);
  }

  auto app_ctx =
      ContextBuilder<TaskAppContext>()
          .bind<TaskRpcContext>()
          .from([tctx](TaskAppContext&) { return tctx; })
          .build();

  if (!config.host.empty() && config.port > 0) {
    WebApp<TaskAppContext> app(app_ctx);

    ADD_ROUTE(app, "POST", "/rpc/task/create/",
              CreateTaskRPC::HttpHandler<TaskAppContext>);
    ADD_ROUTE(
        app, "GET", "/rpc/task/status/",
        GetTaskStatusRPC::HttpHandler<TaskAppContext>);
    ADD_ROUTE(app, "GET", "/rpc/system/status/",
              SystemStatusRPC::HttpHandler<TaskAppContext>);
    ADD_ROUTE(app, "GET", "/health/", GetHealth);

    std::thread worker_thread([&config]() {
      TaskRegistry registry;
      Result reg_result = RegisterBuiltInTasks(registry);
      if (!reg_result.ok()) {
        LOG(ERROR)
            << "[worker] RegisterBuiltInTasks failed: "
            << reg_result << ENDL;
        return;
      }
      WorkerOptions options;
      options.consumer_service = config.consumer_service;
      options.dequeue_endpoint =
          config.dequeue_endpoint.empty()
              ? "/rpc/dequeue/"
              : config.dequeue_endpoint;
      if (config.empty_poll_sleep_ms > 0) {
        options.empty_poll_sleep_ms =
            config.empty_poll_sleep_ms;
      }
      if (config.max_iterations > 0) {
        options.max_iterations = config.max_iterations;
      }
      Result r = RunWorker(config.broker_base_url, registry,
                           options);
      if (!r.ok()) {
        LOG(ERROR) << "[worker] RunWorker failed: " << r
                   << ENDL;
      }
    });

    worker_thread.detach();
    return app.RunServer(config.host, config.port);
  }

  SET_OR_RET(auto parsed, ParseArgs<WorkerConfig>(argv));
  TaskRegistry registry;
  OK_OR_RET(RegisterBuiltInTasks(registry));
  WorkerOptions options;
  options.consumer_service = parsed.consumer_service;
  options.dequeue_endpoint = parsed.dequeue_endpoint.empty()
                                 ? "/rpc/dequeue/"
                                 : parsed.dequeue_endpoint;
  if (parsed.empty_poll_sleep_ms > 0) {
    options.empty_poll_sleep_ms =
        parsed.empty_poll_sleep_ms;
  }
  if (parsed.max_iterations > 0) {
    options.max_iterations = parsed.max_iterations;
  }
  return RunWorker(parsed.broker_base_url, registry,
                   options);
}

int main(int argc, char** argv) {
  std::vector<std::string> args(argv, argv + argc);
  return cs::Result::Main(
      argc, argv, [](std::vector<std::string> a) {
        return RunTaskQueueService(std::move(a));
      });
}

// cs/apps/scribe-worker/main.gpt.cc
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cs/apps/scribe-worker/protos/config.proto.hh"
#include "cs/apps/scribe-worker/whisper_task.gpt.hh"
#include "cs/apps/task-queue-service/task_worker.gpt.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::scribe_worker::RunTranscribeAudio;
using ::cs::apps::scribe_worker::WhisperTaskConfig;
using ::cs::apps::scribe_worker::protos::Config;
using ::cs::apps::task_queue_service::RunWorker;
using ::cs::apps::task_queue_service::TaskRegistry;
using ::cs::apps::task_queue_service::WorkerOptions;
using ::cs::net::json::Object;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::parsers::ParseArgs;
}  // namespace

Result RunScribeWorker(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  std::string database_base_url =
      config.database_base_url.empty()
          ? "http://database-service:8080"
          : config.database_base_url;

  WhisperTaskConfig task_config;
  task_config.blob_base_url = config.blob_base_url;
  task_config.database =
      std::make_shared<DatabaseClientImpl>(
          database_base_url);
  task_config.workspace_dir = config.workspace_dir;
  task_config.hf_token = config.hf_token;
  task_config.whisper_image = config.whisper_image;
  task_config.batch_size =
      config.batch_size > 0 ? config.batch_size : 4;

  TaskRegistry registry;
  OK_OR_RET(registry.RegisterTask(
      "transcribe_audio",
      [task_config](const Object& args) -> Result {
        auto r = RunTranscribeAudio(args, task_config);
        if (!r.ok()) {
          return Result(false, r.message(), r.code());
        }
        return Result(true, "", 0);
      }));

  return RunWorker(config.broker_base_url, registry,
                   WorkerOptions());
}

int main(int argc, char** argv) {
  return cs::Result::Main(argc, argv, RunScribeWorker);
}

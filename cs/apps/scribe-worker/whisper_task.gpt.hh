// cs/apps/scribe-worker/whisper_task.gpt.hh
#ifndef CS_APPS_SCRIBE_WORKER_WHISPER_TASK_GPT_HH
#define CS_APPS_SCRIBE_WORKER_WHISPER_TASK_GPT_HH

#include <memory>
#include <string>

#include "cs/net/json/object.hh"
#include "cs/result.hh"

namespace cs::net::proto::db {

class IDatabaseClient;

}  // namespace cs::net::proto::db

namespace cs::apps::scribe_worker {

struct WhisperTaskConfig {
  std::string blob_base_url;
  std::shared_ptr<cs::net::proto::db::IDatabaseClient>
      database;
  std::string workspace_dir;
  std::string hf_token;
  std::string whisper_image;
  int batch_size;
};

cs::ResultOr<std::string> RunTranscribeAudio(
    const cs::net::json::Object& args,
    const WhisperTaskConfig& config);

}  // namespace cs::apps::scribe_worker

#endif  // CS_APPS_SCRIBE_WORKER_WHISPER_TASK_GPT_HH

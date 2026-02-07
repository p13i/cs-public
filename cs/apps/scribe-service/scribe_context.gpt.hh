// cs/apps/scribe-service/scribe_context.gpt.hh
#ifndef CS_APPS_SCRIBE_SERVICE_SCRIBE_CONTEXT_GPT_HH
#define CS_APPS_SCRIBE_SERVICE_SCRIBE_CONTEXT_GPT_HH

#include <memory>
#include <string>

#include "cs/net/proto/db/client.gpt.hh"

namespace cs::apps::scribe_service {

class BlobClient;

}  // namespace cs::apps::scribe_service

namespace cs::apps::task_queue_service {

class TaskQueueClient;

}  // namespace cs::apps::task_queue_service

namespace cs::apps::scribe_service {

struct ScribeContext {
  std::shared_ptr<cs::net::proto::db::IDatabaseClient>
      database;
  std::string blob_base_url;
  std::string task_queue_base_url;

  cs::net::proto::db::IDatabaseClient* GetDatabase() const {
    return database.get();
  }
  std::shared_ptr<cs::net::proto::db::IDatabaseClient>
  GetDatabaseClient() const {
    return database;
  }
};

}  // namespace cs::apps::scribe_service

#endif  // CS_APPS_SCRIBE_SERVICE_SCRIBE_CONTEXT_GPT_HH

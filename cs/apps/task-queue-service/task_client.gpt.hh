// cs/apps/task-queue-service/task_client.gpt.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_TASK_CLIENT_GPT_HH
#define CS_APPS_TASK_QUEUE_SERVICE_TASK_CLIENT_GPT_HH

#include <string>

#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/net/json/object.hh"
#include "cs/result.hh"

namespace cs::apps::task_queue_service {

// Thin wrapper that turns task payloads into Enqueue RPCs.
class TaskQueueClient {
 public:
  explicit TaskQueueClient(std::string base_url);

  cs::ResultOr<
      cs::apps::message_queue::protos::EnqueueResponse>
  SendTask(const std::string& task_name,
           const cs::net::json::Object& args);

  // Same as SendTask but uses message_uuid when non-empty
  // and consumer_service for the enqueued message.
  cs::ResultOr<
      cs::apps::message_queue::protos::EnqueueResponse>
  SendTask(const std::string& task_name,
           const cs::net::json::Object& args,
           const std::string& message_uuid,
           const std::string& consumer_service);

 private:
  std::string base_url_;
};

}  // namespace cs::apps::task_queue_service

#endif  // CS_APPS_TASK_QUEUE_SERVICE_TASK_CLIENT_GPT_HH

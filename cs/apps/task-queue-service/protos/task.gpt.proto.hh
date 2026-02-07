// cs/apps/task-queue-service/protos/task.gpt.proto.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_GPT_PROTO_HH
#define CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_GPT_PROTO_HH

#include <string>

#include "cs/net/json/object.hh"
#include "cs/net/proto/proto.hh"

namespace cs::apps::task_queue_service::protos {

// Task payload stored inside message queue payloads.
DECLARE_PROTO(Task) {
  // Unique identifier for this task invocation.
  [[required]] [[uuid]]
  std::string task_id;
  // Registered task name used by worker dispatch loop.
  [[required]]
  std::string task_name;
  // Arguments consumed by the handler.
  cs::net::json::Object args;
};

}  // namespace cs::apps::task_queue_service::protos

#endif  // CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_GPT_PROTO_HH

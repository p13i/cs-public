// cs/apps/task-queue-service/protos/task_rpc.proto.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_RPC_PROTO_HH
#define CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_RPC_PROTO_HH

#include <string>

#include "cs/net/json/object.hh"
#include "cs/net/proto/proto.hh"

namespace cs::apps::task_queue_service::protos {

// Request to create a new task.
DECLARE_PROTO(CreateTaskRequest) {
  [[required]]
  std::string task_name;
  cs::net::json::Object args;
  // Optional; if provided, used as message uuid for
  // predictable probes.
  std::string uuid;

  // Consumer service name (required by message-queue).
  // Defaults to "task-queue-service" when empty.
  std::string consumer_service;
};

// Response after creating a task.
DECLARE_PROTO(CreateTaskResponse) {
  [[required]] [[uuid]]
  std::string uuid;
};

// Request to get task status by message uuid.
DECLARE_PROTO(GetTaskStatusRequest) {
  [[required]] [[uuid]]
  std::string uuid;
};

// Response with task status (mirrors Message shape).
DECLARE_PROTO(GetTaskStatusResponse) {
  [[required]] [[uuid]]
  std::string uuid;
  cs::net::json::Object payload;
  [[required]] [[iso8601]]
  std::string timestamp;
  [[required]] [[enum_in("pending", "consumed")]]
  std::string status;
  [[required]]
  std::string type;
  [[required]]
  std::string consumer_service;
};

// Request for system-wide task status counts.
DECLARE_PROTO(SystemStatusRequest){};

// Response with task counts by status.
DECLARE_PROTO(SystemStatusResponse) {
  int pending_count;
  int consumed_count;
};

}  // namespace cs::apps::task_queue_service::protos

#endif  // CS_APPS_TASK_QUEUE_SERVICE_PROTOS_TASK_RPC_PROTO_HH

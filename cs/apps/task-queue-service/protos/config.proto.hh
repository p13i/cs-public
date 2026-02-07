// cs/apps/task-queue-service/protos/config.proto.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_PROTOS_CONFIG_PROTO_HH
#define CS_APPS_TASK_QUEUE_SERVICE_PROTOS_CONFIG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::task_queue_service::protos {

// Configuration for the task worker binary.
DECLARE_PROTO(WorkerConfig) {
  // Base URL of message-queue (e.g.
  // http://localhost:8080).
  [[required]]
  std::string broker_base_url;

  // Consumer service name sent in dequeue requests
  // (required by message-queue).
  [[required]]
  std::string consumer_service;

  // RPC endpoint to call for dequeue operations.
  std::string dequeue_endpoint;

  // Sleep duration when queue is empty (ms).
  [[ge(0)]]
  int empty_poll_sleep_ms;

  // Optional iteration cap (<=0 -> run forever).
  [[ge(0)]]
  int max_iterations;

  // Host to bind HTTP server (empty -> no server).
  std::string host;

  // Port for HTTP server (when host is set). 0 = no server.
  [[ge(0)]] [[le(65535)]]
  int port;

  // Base URL of database-service for task status queries.
  std::string database_base_url;
};

}  // namespace cs::apps::task_queue_service::protos

#endif  // CS_APPS_TASK_QUEUE_SERVICE_PROTOS_CONFIG_PROTO_HH

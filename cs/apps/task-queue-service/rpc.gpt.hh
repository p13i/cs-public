// cs/apps/task-queue-service/rpc.gpt.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_RPC_GPT_HH
#define CS_APPS_TASK_QUEUE_SERVICE_RPC_GPT_HH

#include <memory>

#include "cs/apps/task-queue-service/protos/task_rpc.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::task_queue_service::rpc {

// Context for task RPCs: broker (message-queue) and
// database.
struct TaskRpcContext {
  std::string broker_base_url;
  std::shared_ptr<cs::net::proto::db::IDatabaseClient> db;
};

using TaskAppContext =
    cs::util::di::Context<TaskRpcContext>;

DECLARE_RPC(
    CreateTaskRPC,
    cs::apps::task_queue_service::protos::CreateTaskRequest,
    cs::apps::task_queue_service::protos::
        CreateTaskResponse,
    CTX(TaskRpcContext));

DECLARE_RPC(GetTaskStatusRPC,
            cs::apps::task_queue_service::protos::
                GetTaskStatusRequest,
            cs::apps::task_queue_service::protos::
                GetTaskStatusResponse,
            CTX(TaskRpcContext));

DECLARE_RPC(SystemStatusRPC,
            cs::apps::task_queue_service::protos::
                SystemStatusRequest,
            cs::apps::task_queue_service::protos::
                SystemStatusResponse,
            CTX(TaskRpcContext));

}  // namespace cs::apps::task_queue_service::rpc

#endif  // CS_APPS_TASK_QUEUE_SERVICE_RPC_GPT_HH

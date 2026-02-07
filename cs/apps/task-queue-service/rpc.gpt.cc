// cs/apps/task-queue-service/rpc.gpt.cc
#include "cs/apps/task-queue-service/rpc.gpt.hh"

#include "cs/apps/message-queue/protos/gencode/queue.proto.hh"
#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/task_rpc.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/task_rpc.validate.hh"
#include "cs/apps/task-queue-service/protos/task_rpc.proto.hh"
#include "cs/apps/task-queue-service/task_client.gpt.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Result;
using ::cs::apps::message_queue::protos::Message;
using ::cs::apps::task_queue_service::protos::
    CreateTaskRequest;
using ::cs::apps::task_queue_service::protos::
    CreateTaskResponse;
using ::cs::apps::task_queue_service::protos::
    GetTaskStatusRequest;
using ::cs::apps::task_queue_service::protos::
    GetTaskStatusResponse;
using ::cs::apps::task_queue_service::protos::
    SystemStatusRequest;
using ::cs::apps::task_queue_service::protos::
    SystemStatusResponse;
using ::cs::apps::task_queue_service::protos::gencode::
    task_rpc::validation_generated::CreateTaskRequestRules;
using ::cs::apps::task_queue_service::protos::gencode::
    task_rpc::validation_generated::CreateTaskResponseRules;
using ::cs::apps::task_queue_service::protos::gencode::
    task_rpc::validation_generated::
        GetTaskStatusRequestRules;
using ::cs::apps::task_queue_service::protos::gencode::
    task_rpc::validation_generated::
        GetTaskStatusResponseRules;
using ::cs::apps::task_queue_service::protos::gencode::
    task_rpc::validation_generated::
        SystemStatusResponseRules;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::validation::Validate;
}  // namespace

namespace cs::apps::task_queue_service::rpc {

IMPLEMENT_RPC(CreateTaskRPC, CreateTaskRequest,
              CreateTaskResponse) {
  OK_OR_RET(Validate(request, CreateTaskRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto tctx = ctx->Get<TaskRpcContext>();
  TaskQueueClient client(tctx->broker_base_url);

  std::string message_uuid;
  if (!request.uuid.empty()) {
    message_uuid = request.uuid;
  }
  std::string consumer_service = request.consumer_service;
  if (consumer_service.empty()) {
    consumer_service = "task-queue-service";
  }
  SET_OR_RET(
      auto enqueue_resp,
      client.SendTask(request.task_name, request.args,
                      message_uuid, consumer_service));

  CreateTaskResponse response;
  response.uuid = enqueue_resp.uuid;
  OK_OR_RET(Validate(response, CreateTaskResponseRules{}));
  return response;
}

IMPLEMENT_RPC(GetTaskStatusRPC, GetTaskStatusRequest,
              GetTaskStatusResponse) {
  OK_OR_RET(Validate(request, GetTaskStatusRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto tctx = ctx->Get<TaskRpcContext>();
  auto db = tctx->db;
  if (!db) {
    return TRACE(
        InvalidArgument("database client not configured"));
  }

  auto first_result =
      QueryView<Message>("message-queue", db)
          .where(EQUALS(&Message::uuid, request.uuid))
          .limit(1)
          .first();

  if (!first_result.ok()) {
    if (Result::IsNotFound(first_result)) {
      return TRACE(
          NotFoundError("task not found: " + request.uuid));
    }
    return TRACE(first_result);
  }

  Message msg = first_result.value();
  GetTaskStatusResponse response;
  response.uuid = msg.uuid;
  response.payload = msg.payload;
  response.timestamp = msg.timestamp;
  response.status = msg.status;
  response.type = msg.type;
  response.consumer_service = msg.consumer_service;
  OK_OR_RET(
      Validate(response, GetTaskStatusResponseRules{}));
  return response;
}

IMPLEMENT_RPC(SystemStatusRPC, SystemStatusRequest,
              SystemStatusResponse) {
  SET_OR_RET(auto* ctx, GetContext());
  auto tctx = ctx->Get<TaskRpcContext>();
  auto db = tctx->db;
  if (!db) {
    return TRACE(
        InvalidArgument("database client not configured"));
  }

  SET_OR_RET(auto pending_vec,
             QueryView<Message>("message-queue", db)
                 .where(EQUALS(&Message::status, "pending"))
                 .values());
  SET_OR_RET(
      auto consumed_vec,
      QueryView<Message>("message-queue", db)
          .where(EQUALS(&Message::status, "consumed"))
          .values());

  SystemStatusResponse response;
  response.pending_count =
      static_cast<int>(pending_vec.size());
  response.consumed_count =
      static_cast<int>(consumed_vec.size());
  OK_OR_RET(
      Validate(response, SystemStatusResponseRules{}));
  return response;
}

}  // namespace cs::apps::task_queue_service::rpc

// cs/apps/task-queue-service/task_client.gpt.cc
#include "cs/apps/task-queue-service/task_client.gpt.hh"

#include <utility>

#include "cs/apps/message-queue/protos/gencode/queue.proto.hh"
#include "cs/apps/message-queue/protos/gencode/queue.validate.hh"
#include "cs/apps/message-queue/rpc.gpt.hh"
#include "cs/apps/task-queue-service/protos/gencode/task.gpt.proto.hh"
#include "cs/apps/task-queue-service/protos/gencode/task.gpt.validate.hh"
#include "cs/apps/task-queue-service/protos/task.gpt.proto.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/net/rpc/client.hh"
#include "cs/result.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::ResultOr;
using ::cs::apps::message_queue::protos::EnqueueRequest;
using ::cs::apps::message_queue::protos::EnqueueResponse;
using ::cs::apps::message_queue::protos::Message;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::EnqueueRequestRules;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::MessageRules;
using ::cs::apps::message_queue::rpc::EnqueueRPC;
using ::cs::apps::task_queue_service::protos::Task;
using ::cs::apps::task_queue_service::protos::gencode::
    task_gpt::validation_generated::TaskRules;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::validation::Validate;
using ::cs::net::rpc::RPCClient;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

namespace cs::apps::task_queue_service {

TaskQueueClient::TaskQueueClient(std::string base_url)
    : base_url_(std::move(base_url)) {}

ResultOr<EnqueueResponse> TaskQueueClient::SendTask(
    const std::string& task_name, const Object& args) {
  return SendTask(task_name, args, "",
                  "task-queue-service");
}

ResultOr<EnqueueResponse> TaskQueueClient::SendTask(
    const std::string& task_name, const Object& args,
    const std::string& message_uuid,
    const std::string& consumer_service) {
  Task task;
  task.task_id = generate_uuid_v4();
  task.task_name = task_name;
  task.args = args;
  OK_OR_RET(Validate(task, TaskRules{}));

  Message message;
  if (message_uuid.empty()) {
    message.uuid = generate_uuid_v4();
  } else {
    message.uuid = message_uuid;
  }
  SET_OR_RET(message.payload,
             ParseObject(task.Serialize()));
  message.timestamp = NowAsISO8601TimeUTC();
  message.status = "pending";
  message.type = task_name;
  message.consumer_service = consumer_service.empty()
                                 ? "task-queue-service"
                                 : consumer_service;
  OK_OR_RET(Validate(message, MessageRules{}));
  EnqueueRequest request;
  request.message = message;
  OK_OR_RET(Validate(request, EnqueueRequestRules{}));

  RPCClient<EnqueueRPC> client(base_url_);
  return client.Call("/rpc/enqueue/", request);
}

}  // namespace cs::apps::task_queue_service

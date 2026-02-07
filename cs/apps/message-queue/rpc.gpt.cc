// cs/apps/message-queue/rpc.gpt.cc
#include "cs/apps/message-queue/rpc.gpt.hh"

#include "cs/apps/message-queue/protos/gencode/queue.proto.hh"
#include "cs/apps/message-queue/protos/gencode/queue.validate.hh"
#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/proto/db/protos/gencode/database.validate.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::message_queue::protos::DequeueRequest;
using ::cs::apps::message_queue::protos::DequeueResponse;
using ::cs::apps::message_queue::protos::EnqueueRequest;
using ::cs::apps::message_queue::protos::EnqueueResponse;
using ::cs::apps::message_queue::protos::Message;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::DequeueRequestRules;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::DequeueResponseRules;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::EnqueueRequestRules;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::EnqueueResponseRules;
using ::cs::apps::message_queue::protos::gencode::queue::
    validation_generated::MessageRules;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::proto::database::gencode::database::
    validation_generated::UpsertRequestRules;
using ::cs::net::proto::db::ASC;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::validation::Validate;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

namespace cs::apps::message_queue::rpc {

IMPLEMENT_RPC(EnqueueRPC, EnqueueRequest, EnqueueResponse) {
  OK_OR_RET(Validate(request, EnqueueRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();

  Message msg = request.message;
  // Generate uuid/timestamp when not supplied (empty or
  // placeholder).
  if (msg.uuid.empty()) {
    msg.uuid = generate_uuid_v4();
  }
  if (msg.timestamp.empty()) {
    msg.timestamp = NowAsISO8601TimeUTC();
  }
  msg.status = "pending";
  OK_OR_RET(Validate(msg, MessageRules{}));

  UpsertRequest upsert;
  upsert.collection = "message-queue";
  upsert.uuid = msg.uuid;
  upsert.payload_json = msg.Serialize();
  OK_OR_RET(Validate(upsert, UpsertRequestRules{}));

  OK_OR_RET(db->Upsert(upsert));

  EnqueueResponse response;
  response.uuid = msg.uuid;
  OK_OR_RET(Validate(response, EnqueueResponseRules{}));
  return response;
}

IMPLEMENT_RPC(DequeueRPC, DequeueRequest, DequeueResponse) {
  OK_OR_RET(Validate(request, DequeueRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();

  auto first_result =
      QueryView<Message>("message-queue", db)
          .where(EQUALS(&Message::status, "pending"))
          .where(EQUALS(&Message::consumer_service,
                        request.consumer_service))
          .order_by(&Message::timestamp, ASC)
          .limit(1)
          .first();

  if (!first_result.ok()) {
    if (Result::IsNotFound(first_result)) {
      DequeueResponse empty;
      empty.has_message = false;
      return empty;
    }
    return TRACE(first_result);
  }

  Message msg = first_result.value();
  msg.status = "consumed";
  msg.consumer_service = request.consumer_service;

  UpsertRequest upsert;
  upsert.collection = "message-queue";
  upsert.uuid = msg.uuid;
  upsert.payload_json = msg.Serialize();

  OK_OR_RET(db->Upsert(upsert));

  OK_OR_RET(Validate(msg, MessageRules{}));

  DequeueResponse response;
  response.has_message = true;
  response.message = msg;
  OK_OR_RET(Validate(response, DequeueResponseRules{}));
  return response;
}

}  // namespace cs::apps::message_queue::rpc

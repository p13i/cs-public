// cs/apps/message-queue/rpc.gpt.hh
#ifndef CS_APPS_MESSAGE_QUEUE_RPC_GPT_HH
#define CS_APPS_MESSAGE_QUEUE_RPC_GPT_HH

#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::message_queue::rpc {

DECLARE_RPC(
    EnqueueRPC,
    cs::apps::message_queue::protos::EnqueueRequest,
    cs::apps::message_queue::protos::EnqueueResponse,
    CTX(cs::net::proto::db::IDatabaseClient));

DECLARE_RPC(
    DequeueRPC,
    cs::apps::message_queue::protos::DequeueRequest,
    cs::apps::message_queue::protos::DequeueResponse,
    CTX(cs::net::proto::db::IDatabaseClient));

}  // namespace cs::apps::message_queue::rpc

#endif  // CS_APPS_MESSAGE_QUEUE_RPC_GPT_HH

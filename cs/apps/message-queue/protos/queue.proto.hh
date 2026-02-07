// cs/apps/message-queue/protos/queue.proto.hh
#ifndef CS_APPS_MESSAGE_QUEUE_PROTOS_QUEUE_PROTO_HH
#define CS_APPS_MESSAGE_QUEUE_PROTOS_QUEUE_PROTO_HH

#include <string>

#include "cs/net/json/object.hh"
#include "cs/net/proto/proto.hh"

namespace cs::apps::message_queue::protos {

// Message (DB payload and API).
DECLARE_PROTO(Message) {
  [[required]] [[uuid]]
  std::string uuid;
  [[required]]
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

// Enqueue request.
DECLARE_PROTO(EnqueueRequest) {
  [[required]]
  Message message;
};

// Enqueue response.
DECLARE_PROTO(EnqueueResponse) {
  [[required]] [[uuid]]
  std::string uuid;
};

// Dequeue request.
DECLARE_PROTO(DequeueRequest) {
  [[required]]
  std::string consumer_service;
};

// Dequeue response; when has_message is true, message is
// set.
DECLARE_PROTO(DequeueResponse) {
  [[required]]
  bool has_message;
  [[required]]
  Message message;
};

}  // namespace cs::apps::message_queue::protos

#endif  // CS_APPS_MESSAGE_QUEUE_PROTOS_QUEUE_PROTO_HH

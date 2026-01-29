// cs/apps/trycopilot.ai/protos/log.proto.hh
// cs/apps/trycopilot.ai/protos/log.proto.hh
// cs/apps/trycopilot.ai/protos/log.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_LOG_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_LOG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Timestamp payload.
DECLARE_PROTO(Timestamp) {
  // ISO8601 timestamp string.
  std::string iso8601;
};

// HTTP request log entry.
DECLARE_PROTO(HttpRequest) {
  // Receive timestamp.
  Timestamp receive_timestamp;
  // HTTP method.
  std::string method;
  // Request path.
  std::string path;
  // Request content type.
  std::string content_type;
  // Request content length.
  int content_length;
  // Request body.
  std::string body;
};

// HTTP status info.
DECLARE_PROTO(HttpStatus) {
  // Status code.
  int code;
  // Status name.
  std::string name;
};

// HTTP response log entry.
DECLARE_PROTO(HttpResponse) {
  // Send timestamp.
  Timestamp send_timestamp;
  // Response status.
  HttpStatus status;
  // Response content type.
  std::string content_type;
  // Response content length.
  int content_length;
  // Response body.
  std::string body;
};

// Result summary.
DECLARE_PROTO(Result) {
  // Result code.
  int code;
  // Result message.
  std::string message;
};

// HTTP log record.
DECLARE_PROTO(HttpLog) {
  // Log UUID.
  std::string uuid;
  // Logged request.
  HttpRequest request;
  // Logged response.
  HttpResponse response;
  // Result data.
  Result result;
};

// Create log request.
DECLARE_PROTO(CreateLogRequest) {
  // Request payload.
  HttpRequest request;
  // Response payload.
  HttpResponse response;
  // Result payload.
  Result result;
};

// Create log response.
DECLARE_PROTO(CreateLogResponse) {
  // Created log UUID.
  std::string http_log_uuid;
};

// Get log request.
DECLARE_PROTO(GetLogRequest) {
  // Log UUID flag.
  bool http_log_uuid;
};

// Get log response.
DECLARE_PROTO(GetLogResponse) {
  // Retrieved log.
  HttpLog http_log;
};

// List logs request.
DECLARE_PROTO(ListLogsRequest) {
  // Query string.
  std::string query;
};

// List logs response.
DECLARE_PROTO(ListLogsResponse) {
  // Result count.
  int count;
  // Retrieved logs.
  std::vector<HttpLog> http_logs;
};

// Application log entry.
DECLARE_PROTO(AppLog) {
  // Timestamp string.
  std::string timestamp;
  // Log level.
  std::string level;
  // Source file.
  std::string file;
  // Source line.
  int line;
  // Log message.
  std::string message;
};

// List app logs request.
DECLARE_PROTO(ListAppLogsRequest){};

// List app logs response.
DECLARE_PROTO(ListAppLogsResponse) {
  // App log items.
  std::vector<AppLog> items;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_LOG_PROTO_HH
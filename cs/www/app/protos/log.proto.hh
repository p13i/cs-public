#ifndef CS_WWW_APP_PROTOS_LOG_PROTO_HH
#define CS_WWW_APP_PROTOS_LOG_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Timestamp) { std::string iso8601; };

DECLARE_PROTO(HttpRequest) {
  Timestamp receive_timestamp;
  std::string method;
  std::string path;
  std::string content_type;
  int content_length;
  std::string body;
};

DECLARE_PROTO(HttpStatus) {
  int code;
  std::string name;
};

DECLARE_PROTO(HttpResponse) {
  Timestamp send_timestamp;
  HttpStatus status;
  std::string content_type;
  int content_length;
  std::string body;
};

DECLARE_PROTO(Result) {
  int code;
  std::string message;
};

DECLARE_PROTO(HttpLog) {
  std::string uuid;
  HttpRequest request;
  HttpResponse response;
  Result result;
};

DECLARE_PROTO(CreateLogRequest) {
  HttpRequest request;
  HttpResponse response;
  Result result;
};

DECLARE_PROTO(CreateLogResponse) {
  std::string http_log_uuid;
};

DECLARE_PROTO(GetLogRequest) { bool http_log_uuid; };

DECLARE_PROTO(GetLogResponse) { HttpLog http_log; };

DECLARE_PROTO(ListLogsRequest) { std::string query; };

DECLARE_PROTO(ListLogsResponse) {
  int count;
  std::vector<HttpLog> http_logs;
};

DECLARE_PROTO(AppLog) {
  std::string timestamp;
  std::string level;
  std::string file;
  int line;
  std::string message;
};

DECLARE_PROTO(ListAppLogsRequest){};

DECLARE_PROTO(ListAppLogsResponse) {
  std::vector<AppLog> items;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_LOG_PROTO_HH
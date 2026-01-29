// cs/apps/trycopilot.ai/api/logs.cc
#include "cs/apps/trycopilot.ai/api/logs.hh"

#include "cs/apps/trycopilot.ai/protos/gencode/log.proto.hh"
#include "cs/apps/trycopilot.ai/protos/log.proto.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/field_path_builder.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/util/context.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"

namespace cs::apps::trycopilotai::api {

namespace {
using ::cs::Result;
using ::cs::apps::trycopilotai::protos::AppLog;
using ::cs::apps::trycopilotai::protos::CreateLogRequest;
using ::cs::apps::trycopilotai::protos::CreateLogResponse;
using ::cs::apps::trycopilotai::protos::GetLogRequest;
using ::cs::apps::trycopilotai::protos::GetLogResponse;
using ::cs::apps::trycopilotai::protos::HttpLog;
using ::cs::apps::trycopilotai::protos::HttpRequest;
using ::cs::apps::trycopilotai::protos::ListAppLogsRequest;
using ::cs::apps::trycopilotai::protos::ListAppLogsResponse;
using ::cs::apps::trycopilotai::protos::ListLogsRequest;
using ::cs::apps::trycopilotai::protos::ListLogsResponse;
using ::cs::apps::trycopilotai::protos::Timestamp;
using ::cs::net::proto::db::CONTAINS;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::NOT_CONTAINS;
using ::cs::util::Context;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

bool IsFaviconRequest(const CreateLogRequest& request) {
  return request.request.path == "/favicon.ico" &&
         request.response.status.code == 200;
}

IMPLEMENT_API(CreateLogAPI, CreateLogRequest,
              CreateLogResponse) {
  HttpLog log =
      cs::apps::trycopilotai::protos::gencode::log::
          HttpLogBuilderImpl()
              .set_uuid(generate_uuid_v4())
              .set_request(request.request)
              .set_response(
                  cs::apps::trycopilotai::protos::gencode::
                      log::HttpResponseBuilderImpl()
                          .set_content_length(
                              request.response
                                  .content_length)
                          .set_content_type(
                              request.response.content_type)
                          .set_body(
                              IsFaviconRequest(request)
                                  ? "favicon"
                                  : request.response.body
                                            .substr(0,
                                                    128) +
                                        "...")
                          .set_send_timestamp(
                              request.response
                                  .send_timestamp)
                          .set_status(
                              request.response.status)
                          .Build())
              .set_result(request.result)
              .Build();
#if 0
  // Disabled: http_logs logging to prevent database-service RPC requests
  OK_OR_RET(cs::net::proto::db::Insert("http_logs", log));
#endif  // #if 0
  CreateLogResponse response;
  response.http_log_uuid = log.uuid;
  return response;
}

IMPLEMENT_API(ListLogsAPI, ListLogsRequest,
              ListLogsResponse) {
  ListLogsResponse response;
  TIME_IT_LOG_SET(response.http_logs, {
    cs::net::proto::db::QueryView<HttpLog> query_view(
        "http_logs");
    cs::net::proto::db::FieldPathBuilder<HttpLog> log;
    return query_view
        .where(NOT_CONTAINS(
            log >> &HttpLog::request >> &HttpRequest::path,
            "favicon.ico"))
        .order_by(log >> &HttpLog::request >>
                      &HttpRequest::receive_timestamp >>
                      &Timestamp::iso8601,
                  "desc")
        .values();
  });

  response.count = response.http_logs.size();

  return response;
}

IMPLEMENT_API(ListAppLogsAPI, ListAppLogsRequest,
              ListAppLogsResponse) {
  ListAppLogsResponse response;
  TIME_IT_LOG_SET(response.items, {
    cs::net::proto::db::QueryView<AppLog> query_view(
        "app_logs");
    return query_view
        .order_by(&AppLog::timestamp, "desc")
        // .limit(10000)
        .values();
  });

  return response;
}

}  // namespace cs::apps::trycopilotai::api
// cs/apps/trycopilot.ai/api/logs.cc
#include "cs/apps/trycopilot.ai/api/logs.hh"

#include "cs/apps/trycopilot.ai/protos/gencode/log.proto.hh"
#include "cs/apps/trycopilot.ai/protos/log.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/field_path_builder.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/context.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::apps::trycopilotai::protos::AppLog;
using ::cs::apps::trycopilotai::protos::CreateLogRequest;
using ::cs::apps::trycopilotai::protos::CreateLogResponse;
using ::cs::apps::trycopilotai::protos::HttpLog;
using ::cs::apps::trycopilotai::protos::HttpRequest;
using ::cs::apps::trycopilotai::protos::HttpResponse;
using ::cs::apps::trycopilotai::protos::ListAppLogsRequest;
using ::cs::apps::trycopilotai::protos::ListAppLogsResponse;
using ::cs::apps::trycopilotai::protos::ListLogsRequest;
using ::cs::apps::trycopilotai::protos::ListLogsResponse;
using ::cs::apps::trycopilotai::protos::Timestamp;
using ::cs::net::proto::db::FieldPathBuilder;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::NOT_CONTAINS;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
}  // namespace

namespace cs::apps::trycopilotai::api {

using ::cs::apps::trycopilotai::protos::AppLog;
using ::cs::apps::trycopilotai::protos::CreateLogRequest;
using ::cs::apps::trycopilotai::protos::CreateLogResponse;
using ::cs::apps::trycopilotai::protos::HttpLog;
using ::cs::apps::trycopilotai::protos::HttpRequest;
using ::cs::apps::trycopilotai::protos::HttpResponse;
using ::cs::apps::trycopilotai::protos::ListAppLogsRequest;
using ::cs::apps::trycopilotai::protos::ListAppLogsResponse;
using ::cs::apps::trycopilotai::protos::ListLogsRequest;
using ::cs::apps::trycopilotai::protos::ListLogsResponse;
using ::cs::apps::trycopilotai::protos::Timestamp;
using ::cs::net::proto::db::FieldPathBuilder;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::NOT_CONTAINS;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::util::random::uuid::generate_uuid_v4;

bool IsFaviconRequest(const CreateLogRequest& request) {
  return request.request.path == "/favicon.ico" &&
         request.response.status.code == 200;
}

IMPLEMENT_RPC(CreateLogRPC, CreateLogRequest,
              CreateLogResponse) {
  HttpLog log =
      ::cs::apps::trycopilotai::protos::gencode::log::
          HttpLogBuilderImpl()
              .set_uuid(generate_uuid_v4())
              .set_request(request.request)
              .set_response(
                  ::cs::apps::trycopilotai::protos::
                      gencode::log::
                          HttpResponseBuilderImpl()
                              .set_content_length(
                                  request.response
                                      .content_length)
                              .set_content_type(
                                  request.response
                                      .content_type)
                              .set_body(
                                  IsFaviconRequest(request)
                                      ? "favicon"
                                      : request.response
                                                .body
                                                .substr(
                                                    0,
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
  SET_OR_RET(auto* rpc_ctx, GetContext());
  auto db = rpc_ctx->Get<IDatabaseClient>();
  OK_OR_RET(Upsert("http_logs", log, *db));
  CreateLogResponse response;
  response.http_log_uuid = log.uuid;
  return response;
}

IMPLEMENT_RPC(ListLogsRPC, ListLogsRequest,
              ListLogsResponse) {
  SET_OR_RET(auto* rpc_ctx, GetContext());
  auto db = rpc_ctx->Get<IDatabaseClient>();
  ListLogsResponse response;
  TIME_IT_LOG_SET(response.http_logs, {
    QueryView<HttpLog> query_view("http_logs", db);
    FieldPathBuilder<HttpLog> log;
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

IMPLEMENT_RPC(ListAppLogsRPC, ListAppLogsRequest,
              ListAppLogsResponse) {
  SET_OR_RET(auto* rpc_ctx, GetContext());
  auto db = rpc_ctx->Get<IDatabaseClient>();
  ListAppLogsResponse response;
  TIME_IT_LOG_SET(response.items, {
    QueryView<AppLog> query_view("app_logs", db);
    return query_view
        .order_by(&AppLog::timestamp, "desc")
        // .limit(10000)
        .values();
  });

  return response;
}

}  // namespace cs::apps::trycopilotai::api
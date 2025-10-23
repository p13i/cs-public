#include "cs/www/app/api/logs.hh"

#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/net/proto/db/db_deferred_async.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"
#include "cs/www/app/context/context.hh"
#include "cs/www/app/protos/gencode/log.proto.hh"
#include "cs/www/app/protos/log.proto.hh"

namespace cs::www::app::api {

namespace {
using ::cs::Result;
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::util::time::NowAsISO8601TimeUTC;
using ::cs::www::app::Context;
using ::cs::www::app::protos::AppLog;
using ::cs::www::app::protos::CreateLogRequest;
using ::cs::www::app::protos::CreateLogResponse;
using ::cs::www::app::protos::GetLogRequest;
using ::cs::www::app::protos::GetLogResponse;
using ::cs::www::app::protos::HttpLog;
using ::cs::www::app::protos::ListAppLogsRequest;
using ::cs::www::app::protos::ListAppLogsResponse;
using ::cs::www::app::protos::ListLogsRequest;
using ::cs::www::app::protos::ListLogsResponse;
}  // namespace

bool IsFaviconRequest(const CreateLogRequest& request) {
  return request.request.path == "/favicon.ico" &&
         request.response.status.code == 200;
}

IMPLEMENT_API(CreateLogAPI, CreateLogRequest,
              CreateLogResponse) {
  HttpLog log =
      cs::www::app::protos::gencode::log::
          HttpLogBuilderImpl()
              .set_uuid(generate_uuid_v4())
              .set_request(request.request)
              .set_response(
                  cs::www::app::protos::gencode::log::
                      HttpResponseBuilderImpl()
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
  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));
  JsonProtoDatabase<HttpLog> db(DATA_DIR, "http_logs");
  OK_OR_RET(db.Insert(log));
  CreateLogResponse response;
  response.http_log_uuid = log.uuid;
  return response;
}

#define USE_ASYNC_DB

IMPLEMENT_API(ListLogsAPI, ListLogsRequest,
              ListLogsResponse) {
  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

#ifdef USE_ASYNC_DB
  cs::net::proto::db::AsyncJsonProtoDatabase<HttpLog> db(
      DATA_DIR, "http_logs");
#else
  cs::net::proto::db::JsonProtoDatabase<HttpLog> db(
      DATA_DIR, "http_logs");
#endif

  ListLogsResponse response;
  TIME_IT_LOG_SET(response.http_logs, {
    SET_OR_RET(auto query_view, db.query_view());
    return query_view
        .where(LAMBDA(log, !STR_CONTAINS(log.request.path,
                                         "favicon.ico")))
        .order_by(
            DESCENDING(request.receive_timestamp.iso8601))
        .values();
  });

  response.count = response.http_logs.size();

  return response;
}

IMPLEMENT_API(ListAppLogsAPI, ListAppLogsRequest,
              ListAppLogsResponse) {
  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

#ifdef USE_ASYNC_DB
  cs::net::proto::db::AsyncJsonProtoDatabase<AppLog> db(
      DATA_DIR, "app_logs");
#else
  cs::net::proto::db::JsonProtoDatabase<AppLog> db(
      DATA_DIR, "app_logs");
#endif

  ListAppLogsResponse response;
  TIME_IT_LOG_SET(response.items, {
    SET_OR_RET(auto query_view, db.query_view());
    return query_view
        .order_by(DESCENDING(timestamp))
        // .limit(10000)
        .values();
  });

  return response;
}

#undef USE_ASYNC_DB

}  // namespace cs::www::app::api
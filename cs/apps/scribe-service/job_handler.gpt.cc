// cs/apps/scribe-service/job_handler.gpt.cc
#include "cs/apps/scribe-service/job_handler.gpt.hh"

#include <string>
#include <vector>

#include "cs/apps/scribe-service/protos/job.proto.hh"
#include "cs/apps/scribe-service/scribe_context.gpt.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::NotFoundError;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::scribe_service::protos::TranscriptionJob;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::database::Condition;
using ::cs::net::proto::database::DatabaseRecord;
using ::cs::net::proto::database::QueryRequest;
using ::cs::net::proto::database::QueryResponse;
using ::cs::net::proto::database::QueryStep;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::proto::database::Value;
using ::cs::net::proto::db::IDatabaseClient;
}  // namespace

namespace {

constexpr const char kScribeJobsCollection[] =
    "scribe-jobs";

std::string ExtractJobIdFromPath(
    const std::string& path, const std::string& prefix) {
  if (path.size() <= prefix.size()) {
    return "";
  }
  std::string rest = path.substr(prefix.size());
  size_t slash = rest.find('/');
  if (slash != std::string::npos) {
    rest = rest.substr(0, slash);
  }
  return rest;
}

ResultOr<TranscriptionJob> GetJobByUuid(
    IDatabaseClient* db, const std::string& job_id) {
  QueryRequest req;
  req.collection = kScribeJobsCollection;
  QueryStep step;
  step.type = "WHERE";
  Condition cond;
  cond.field_path = "uuid";
  cond.operator_type = "eq";
  cond.value.type = "string";
  cond.value.string_value = job_id;
  step.predicate.condition = cond.Serialize(0);
  req.steps.push_back(step);
  QueryStep limit_step;
  limit_step.type = "LIMIT";
  limit_step.limit = 1;
  req.steps.push_back(limit_step);

  SET_OR_RET(QueryResponse resp, db->Query(req));
  if (resp.records.empty()) {
    return TRACE(NotFoundError("job not found"));
  }
  return TranscriptionJob().Parse(
      resp.records[0].payload_json);
}

}  // namespace

namespace cs::apps::scribe_service {

Response PostJobLog(Request request, ScribeContext& ctx) {
  std::string path = request.path();
  std::string job_id =
      ExtractJobIdFromPath(path, "/internal/job/");
  if (job_id.empty()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain,
                    "missing job_id");
  }

  std::string chunk = request.body();
  auto db = ctx.GetDatabase();
  ResultOr<TranscriptionJob> job_or =
      GetJobByUuid(db, job_id);
  if (!job_or.ok()) {
    if (Result::IsNotFound(job_or)) {
      return Response(HTTP_404_NOT_FOUND,
                      kContentTypeTextPlain,
                      "job not found");
    }
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    job_or.message());
  }

  TranscriptionJob job = std::move(job_or).value();
  job.cli_output.push_back(chunk);

  UpsertRequest upsert;
  upsert.collection = kScribeJobsCollection;
  upsert.uuid = job_id;
  upsert.payload_json = job.Serialize();
  Result upsert_result = db->Upsert(upsert);
  if (!upsert_result.ok()) {
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    upsert_result.message());
  }

  ctx.GetJobHub()->PushChunk(job_id, chunk);
  return Response(HTTP_200_OK, kContentTypeTextPlain, "OK");
}

Response PostJobComplete(Request request,
                         ScribeContext& ctx) {
  std::string path = request.path();
  std::string job_id =
      ExtractJobIdFromPath(path, "/internal/job/");
  if (job_id.empty()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain,
                    "missing job_id");
  }

  std::string body = request.body();
  std::string transcript_text = body;
  std::string status = "completed";

  auto db = ctx.GetDatabase();
  ResultOr<TranscriptionJob> job_or =
      GetJobByUuid(db, job_id);
  if (!job_or.ok()) {
    if (Result::IsNotFound(job_or)) {
      return Response(HTTP_404_NOT_FOUND,
                      kContentTypeTextPlain,
                      "job not found");
    }
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    job_or.message());
  }

  TranscriptionJob job = std::move(job_or).value();
  job.status = status;
  job.transcript_text = transcript_text;

  UpsertRequest upsert;
  upsert.collection = kScribeJobsCollection;
  upsert.uuid = job_id;
  upsert.payload_json = job.Serialize();
  Result upsert_result = db->Upsert(upsert);
  if (!upsert_result.ok()) {
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    upsert_result.message());
  }

  ctx.GetJobHub()->PushComplete(job_id, transcript_text,
                                status);
  return Response(HTTP_200_OK, kContentTypeTextPlain, "OK");
}

}  // namespace cs::apps::scribe_service

// cs/apps/scribe-service/ui.gpt.cc
#include "cs/apps/scribe-service/ui.gpt.hh"

#include <sstream>
#include <string>

#include "cs/apps/common/website.gpt.hh"
#include "cs/apps/common/website_nav.gpt.hh"
#include "cs/apps/scribe-service/blob_client.gpt.hh"
#include "cs/apps/scribe-service/protos/job.proto.hh"
#include "cs/apps/scribe-service/protos/upload.proto.hh"
#include "cs/apps/scribe-service/scribe_context.gpt.hh"
#include "cs/apps/task-queue-service/task_client.gpt.hh"
#include "cs/log.hh"
#include "cs/net/html/bootstrap/proto_form.gpt.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/html/escape.gpt.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/json/object.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/util/base64.gpt.hh"
#include "cs/util/string.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::common::website::BuildWebsiteConfig;
using ::cs::apps::common::website::MakeWebsite;
using ::cs::apps::common::website::NavItemSpec;
using ::cs::apps::common::website::PageOptions;
using ::cs::apps::common::website::WebsiteConfig;
using ::cs::apps::common::website::WebsiteUserInfo;
using ::cs::apps::message_queue::protos::EnqueueResponse;
using ::cs::apps::scribe_service::protos::TranscriptionJob;
using ::cs::apps::scribe_service::protos::UploadAudioForm;
using ::cs::apps::task_queue_service::TaskQueueClient;
using ::cs::net::html::EscapeForHtml;
using ::cs::net::html::dom::a;
using ::cs::net::html::dom::div;
using ::cs::net::html::dom::h1;
using ::cs::net::html::dom::h2;
using ::cs::net::html::dom::p;
using ::cs::net::html::dom::pre;
using ::cs::net::html::dom::strong;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_302_FOUND;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::json::Object;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::net::proto::form::GenerateBootstrapProtoForm;
using ::cs::util::Base64Encode;
using ::cs::util::random::uuid::generate_uuid_v4;
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
    std::shared_ptr<IDatabaseClient> db,
    const std::string& job_id) {
  QueryView<TranscriptionJob> query(kScribeJobsCollection,
                                    db);
  SET_OR_RET(
      auto first,
      query.where(EQUALS(&TranscriptionJob::job_id, job_id))
          .limit(1)
          .first());
  return first;
}

std::string GetFirstFilePartFromMultipart(
    const std::string& body,
    const std::string& content_type) {
  const std::string boundary_prefix = "boundary=";
  size_t pos = content_type.find(boundary_prefix);
  if (pos == std::string::npos) {
    return "";
  }
  pos += boundary_prefix.size();
  while (pos < content_type.size() &&
         (content_type[pos] == ' ' ||
          content_type[pos] == '"')) {
    pos++;
  }
  size_t end = pos;
  while (end < content_type.size() &&
         content_type[end] != '"' &&
         content_type[end] != ' ' &&
         content_type[end] != ';' &&
         content_type[end] != '\r' &&
         content_type[end] != '\n') {
    end++;
  }
  std::string boundary =
      content_type.substr(pos, end - pos);
  if (boundary.empty()) {
    return "";
  }
  std::string delim = "\r\n--" + boundary;
  size_t start = 0;
  if (body.size() >= 2 && body[0] == '-' &&
      body[1] == '-') {
    size_t first = body.find("\r\n", 0);
    if (first != std::string::npos) {
      start = first + 2;
    } else {
      first = body.find('\n', 0);
      if (first != std::string::npos) start = first + 1;
    }
  }
  size_t part_start = body.find(delim, start);
  if (part_start == std::string::npos) {
    if (start < body.size()) {
      size_t headers_end = body.find("\r\n\r\n", start);
      if (headers_end != std::string::npos) {
        return body.substr(headers_end + 4);
      }
      headers_end = body.find("\n\n", start);
      if (headers_end != std::string::npos) {
        return body.substr(headers_end + 2);
      }
    }
    return "";
  }
  part_start += delim.size();
  if (part_start < body.size() && body[part_start] == '\r')
    part_start++;
  if (part_start < body.size() && body[part_start] == '\n')
    part_start++;
  size_t headers_end = body.find("\r\n\r\n", part_start);
  if (headers_end == std::string::npos) {
    headers_end = body.find("\n\n", part_start);
    if (headers_end == std::string::npos) return "";
    headers_end += 2;
  } else {
    headers_end += 4;
  }
  size_t part_end = body.find(delim, headers_end);
  if (part_end == std::string::npos) {
    part_end = body.size();
    if (part_end >= 2 && body[part_end - 2] == '\r' &&
        body[part_end - 1] == '\n') {
      part_end -= 2;
    } else if (part_end >= 1 &&
               body[part_end - 1] == '\n') {
      part_end -= 1;
    }
  }
  return body.substr(headers_end, part_end - headers_end);
}

static std::string MakeScribePage(
    const std::string& title,
    const std::string& content_html,
    const std::string& active_path) {
  std::vector<NavItemSpec> nav_items;
  NavItemSpec item;
  item.href = "/";
  item.label = "Scribe";
  item.requires_auth = false;
  item.requires_admin = false;
  item.requires_logged_out = false;
  nav_items.push_back(item);

  PageOptions opts{};
  opts.nav_base = "";
  opts.nav_items = std::move(nav_items);
  WebsiteUserInfo user_info{};
  user_info.logged_in = false;
  user_info.email = "";
  user_info.admin = false;
  opts.user = user_info;
  opts.active_path = active_path;
  opts.title = title;
  opts.content_html = content_html;
  WebsiteConfig config = BuildWebsiteConfig(opts);
  return MakeWebsite(config);
}

}  // namespace

namespace cs::apps::scribe_service {

Response GetUploadPage(Request request, ScribeContext&) {
  std::string form_html =
      GenerateBootstrapProtoForm<UploadAudioForm>(
          "/upload/", "POST", nullptr,
          "multipart/form-data", "Transcribe");
  std::string content = div(
      h1({{"class", "mb-3"}}, "Scribe"),
      p({{"class", "mb-4"}},
        "Upload an audio file to transcribe (WhisperX)."),
      form_html);
  std::string html =
      MakeScribePage("Scribe - Upload Audio", content, "/");
  return HtmlResponse(html);
}

Response PostUpload(Request request, ScribeContext& ctx) {
  std::string content_type;
  {
    auto it = request.headers().find("Content-Type");
    if (it != request.headers().end()) {
      content_type = it->second;
    }
  }
  std::string audio_bytes;
  if (content_type.find("multipart/form-data") !=
      std::string::npos) {
    audio_bytes = GetFirstFilePartFromMultipart(
        request.body(), content_type);
  } else {
    audio_bytes = request.body();
  }
  if (audio_bytes.empty()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain,
                    "no audio file in request");
  }

  std::string job_id = generate_uuid_v4();

  BlobClient blob_client(ctx.blob_base_url);
  std::string bytes_base64 = Base64Encode(audio_bytes);
  auto put_result = blob_client.PutBlob(
      bytes_base64, "audio/mpeg", "audio.mp3");
  if (!put_result.ok()) {
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    put_result.message());
  }
  std::string blob_audio_uuid = put_result.value();

  TranscriptionJob job;
  job.job_id = job_id;
  job.status = "pending";
  job.blob_audio_uuid = blob_audio_uuid;

  OK_OR_RET(Upsert(kScribeJobsCollection, job_id, job,
                   *ctx.GetDatabase()));

  Object::KVMap args_kv;
  args_kv["job_id"] = Object::NewString(job_id);
  args_kv["blob_audio_uuid"] =
      Object::NewString(blob_audio_uuid);
  Object args(args_kv);

  TaskQueueClient task_client(ctx.task_queue_base_url);
  ResultOr<EnqueueResponse> enq_or =
      task_client.SendTask("transcribe_audio", args);
  if (!enq_or.ok()) {
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    enq_or.message());
  }

  Response redirect(HTTP_302_FOUND, "text/html", "");
  redirect.SetHeader("Location", "/job/" + job_id);
  return redirect;
}

Response GetJobPage(Request request, ScribeContext& ctx) {
  std::string path = request.path();
  std::string job_id = ExtractJobIdFromPath(path, "/job/");
  if (job_id.empty()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain,
                    "missing job_id");
  }

  ResultOr<TranscriptionJob> job_or =
      GetJobByUuid(ctx.GetDatabaseClient(), job_id);
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
  std::string escaped_job_id = EscapeForHtml(job_id);

  std::string cli_output_escaped;
  for (size_t i = 0; i < job.cli_output.size(); ++i) {
    if (i > 0) {
      cli_output_escaped += '\n';
    }
    cli_output_escaped += EscapeForHtml(job.cli_output[i]);
  }

  std::string main_content =
      h1({{"class", "mb-3"}}, "Transcription Job") +
      p({{"class", "mb-2"}},
        strong("Job ID: ") + escaped_job_id) +
      p({{"class", "mb-4"}},
        strong("Status: ") + EscapeForHtml(job.status)) +
      pre({{"id", "cli_output"},
           {"class", "bg-light border rounded p-3 mb-4"},
           {"style", "max-height: 20em; overflow: auto;"}},
          cli_output_escaped);

  std::string transcript_section;
  if (job.status == "completed" &&
      !job.transcript_text.empty()) {
    transcript_section =
        p({{"class", "mb-2"}},
          a({{"class", "btn btn-outline-primary"}},
            "/job/" + escaped_job_id + "/transcript",
            "Download transcript")) +
        h2({{"class", "mb-2 mt-4"}}, "Transcript") +
        pre({{"class", "bg-light border rounded p-3"},
             {"style", "white-space: pre-wrap;"}},
            EscapeForHtml(job.transcript_text));
  }

  // Refresh status page every 5 seconds (poll database).
  const std::string meta_refresh =
      "<meta http-equiv=\"refresh\" content=\"5\">";
  std::string content =
      meta_refresh + main_content + transcript_section;
  std::string html = MakeScribePage(
      "Job " + escaped_job_id, content, request.path());
  return HtmlResponse(html);
}

Response GetJobTranscript(Request request,
                          ScribeContext& ctx) {
  std::string path = request.path();
  const std::string prefix = "/job/";
  const std::string suffix = "/transcript";
  size_t transcript_suffix = path.rfind(suffix);
  if (!CS_STRING_CONTAINS(path, suffix) ||
      path.size() < prefix.size() + suffix.size()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain, "bad path");
  }
  std::string job_id = path.substr(
      prefix.size(), transcript_suffix - prefix.size());
  if (job_id.empty()) {
    return Response(HTTP_400_BAD_REQUEST,
                    kContentTypeTextPlain,
                    "missing job_id");
  }

  SET_OR_RET(TranscriptionJob job,
             GetJobByUuid(ctx.GetDatabaseClient(), job_id));

  return Response(HTTP_200_OK, kContentTypeTextPlain,
                  job.transcript_text)
      .SetHeader("Content-Disposition",
                 "attachment; filename=transcript-" +
                     job_id + ".txt");
}

}  // namespace cs::apps::scribe_service

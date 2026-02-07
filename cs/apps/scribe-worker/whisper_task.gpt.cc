// cs/apps/scribe-worker/whisper_task.gpt.cc
#include "cs/apps/scribe-worker/whisper_task.gpt.hh"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/apps/scribe-service/protos/job.proto.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/client.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/util/base64.gpt.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::FetchResponse;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::blob_service::protos::GetBlobRequest;
using ::cs::apps::blob_service::protos::GetBlobResponse;
using ::cs::apps::scribe_service::protos::TranscriptionJob;
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::json::Object;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::util::Base64Decode;
}  // namespace

namespace cs::apps::scribe_worker {

namespace {

constexpr const char kScribeJobsCollection[] =
    "scribe-jobs";

ResultOr<TranscriptionJob> GetJobByJobId(
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

Result AppendJobLogChunk(
    std::shared_ptr<IDatabaseClient> db,
    const std::string& job_id, const std::string& chunk) {
  SET_OR_RET(TranscriptionJob job,
             GetJobByJobId(db, job_id));
  if (job.status == "pending") {
    job.status = "running";
  }
  job.cli_output.push_back(chunk);
  OK_OR_RET(
      Upsert(kScribeJobsCollection, job_id, job, *db));
  return Ok();
}

Result PublishJobComplete(
    std::shared_ptr<IDatabaseClient> db,
    const std::string& job_id,
    const std::string& transcript_text) {
  SET_OR_RET(TranscriptionJob job,
             GetJobByJobId(db, job_id));
  job.status = "completed";
  job.transcript_text = transcript_text;
  OK_OR_RET(
      Upsert(kScribeJobsCollection, job_id, job, *db));
  return Ok();
}

ResultOr<std::string> GetBlobBytes(
    const std::string& base_url, const std::string& uuid) {
  GetBlobRequest req;
  req.uuid = uuid;
  SET_OR_RET(
      auto resp,
      FetchResponse(
          base_url + "/rpc/get_blob/", "POST",
          {{"Content-Type", kContentTypeApplicationJson}},
          req.Serialize()));
  if (resp.status().code != 200) {
    return Error("GetBlob HTTP " +
                 std::to_string(resp.status().code));
  }
  SET_OR_RET(GetBlobResponse parsed,
             GetBlobResponse().Parse(resp.body()));
  SET_OR_RET(std::string bytes,
             Base64Decode(parsed.blob.bytes_base64));
  return bytes;
}

Result RunDockerAndStreamLog(
    const WhisperTaskConfig& config,
    const std::string& job_id,
    const std::string& work_dir) {
  std::string hf =
      config.hf_token.empty() ? "" : config.hf_token;
  int batch = config.batch_size > 0 ? config.batch_size : 4;
  std::string img = config.whisper_image.empty()
                        ? "ghcr.io/jim60105/whisperx:latest"
                        : config.whisper_image;

  std::ostringstream cmd;
  cmd << "docker run --rm -v \"" << work_dir
      << "\":/app -v whisper_cache:/.cache " << img
      << " -- --model large-v3 --language en --diarize "
      << "--hf_token \"" << hf << "\" --batch_size "
      << batch << " /app/INPUT.mp3 2>&1";

  std::string cmd_str = cmd.str();
  FILE* fp = popen(cmd_str.c_str(), "r");
  if (!fp) {
    return Error("popen docker failed");
  }

  char buf[4096];
  std::string line;
  std::shared_ptr<IDatabaseClient> db = config.database;
  while (fgets(buf, sizeof(buf), fp) != nullptr) {
    line += buf;
    if (line.find('\n') != std::string::npos) {
      OK_OR_RET(AppendJobLogChunk(db, job_id, line));
      line.clear();
    }
  }
  if (!line.empty()) {
    OK_OR_RET(AppendJobLogChunk(db, job_id, line));
  }
  int status = pclose(fp);
  if (status != 0) {
    OK_OR_RET(AppendJobLogChunk(
        db, job_id,
        "\n[Docker exited with non-zero status]\n"));
  }
  return Ok();
}

ResultOr<std::string> ReadTranscript(
    const std::string& work_dir) {
  std::string path = work_dir + "/INPUT.txt";
  std::ifstream ifs(path);
  if (!ifs) {
    return Error("Failed to open INPUT.txt");
  }
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

}  // namespace

ResultOr<std::string> RunTranscribeAudio(
    const Object& args, const WhisperTaskConfig& config) {
  SET_OR_RET(Object job_id_obj, args.get("job_id"));
  std::string job_id = job_id_obj.as(std::string());
  if (job_id.empty()) {
    return Error("args missing job_id");
  }
  SET_OR_RET(Object blob_audio_uuid_obj,
             args.get("blob_audio_uuid"));
  std::string blob_audio_uuid =
      blob_audio_uuid_obj.as(std::string());
  if (blob_audio_uuid.empty()) {
    return Error("args missing blob_audio_uuid");
  }

  SET_OR_RET(
      std::string audio_bytes,
      GetBlobBytes(config.blob_base_url, blob_audio_uuid));

  std::string work_dir =
      config.workspace_dir + "/" + job_id;
  OK_OR_RET(cs::fs::mkdir(work_dir));

  std::string input_path = work_dir + "/INPUT.mp3";
  std::ofstream ofs(input_path, std::ios::binary);
  if (!ofs) {
    return Error("Failed to write INPUT.mp3");
  }
  ofs.write(audio_bytes.data(),
            (std::streamsize)audio_bytes.size());
  ofs.close();
  if (!ofs) {
    return Error("Failed to write INPUT.mp3");
  }

  OK_OR_RET(
      RunDockerAndStreamLog(config, job_id, work_dir));

  SET_OR_RET(std::string transcript,
             ReadTranscript(work_dir));
  OK_OR_RET(PublishJobComplete(config.database, job_id,
                               transcript));
  return transcript;
}

}  // namespace cs::apps::scribe_worker

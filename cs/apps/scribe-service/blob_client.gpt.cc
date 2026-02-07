// cs/apps/scribe-service/blob_client.gpt.cc
#include "cs/apps/scribe-service/blob_client.gpt.hh"

#include <string>

#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/response.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::FetchResponse;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::blob_service::protos::GetBlobRequest;
using ::cs::apps::blob_service::protos::GetBlobResponse;
using ::cs::apps::blob_service::protos::UpsertBlobRequest;
using ::cs::apps::blob_service::protos::UpsertBlobResponse;
using ::cs::net::http::kContentTypeApplicationJson;
}  // namespace

namespace cs::apps::scribe_service {

BlobClient::BlobClient(std::string base_url)
    : base_url_(std::move(base_url)) {}

ResultOr<std::string> BlobClient::PutBlob(
    const std::string& bytes_base64,
    const std::string& content_type,
    const std::string& original_filename) {
  UpsertBlobRequest req;
  req.blob.bytes_base64 = bytes_base64;
  req.blob.metadata.content_type = content_type;
  req.blob.metadata.original_filename = original_filename;
  req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  req.blob.metadata.size = 0;
  req.blob.metadata.created_at = "1970-01-01T00:00:00Z";

  SET_OR_RET(
      auto resp,
      FetchResponse(
          base_url_ + "/rpc/blob/upsert/", "POST",
          {{"Content-Type", kContentTypeApplicationJson}},
          req.Serialize()));
  if (resp.status().code != 200) {
    return cs::Error("PutBlob HTTP " +
                     std::to_string(resp.status().code));
  }
  SET_OR_RET(UpsertBlobResponse parsed,
             UpsertBlobResponse().Parse(resp.body()));
  return parsed.metadata.uuid;
}

ResultOr<std::string> BlobClient::GetBlob(
    const std::string& uuid) {
  GetBlobRequest req;
  req.uuid = uuid;

  SET_OR_RET(
      auto resp,
      FetchResponse(
          base_url_ + "/rpc/blob/get/", "POST",
          {{"Content-Type", kContentTypeApplicationJson}},
          req.Serialize()));
  if (resp.status().code != 200) {
    return cs::Error("GetBlob HTTP " +
                     std::to_string(resp.status().code));
  }
  SET_OR_RET(GetBlobResponse parsed,
             GetBlobResponse().Parse(resp.body()));
  return parsed.blob.bytes_base64;
}

}  // namespace cs::apps::scribe_service

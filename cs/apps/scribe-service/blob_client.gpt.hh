// cs/apps/scribe-service/blob_client.gpt.hh
#ifndef CS_APPS_SCRIBE_SERVICE_BLOB_CLIENT_GPT_HH
#define CS_APPS_SCRIBE_SERVICE_BLOB_CLIENT_GPT_HH

#include <string>

#include "cs/result.hh"

namespace cs::apps::scribe_service {

// HTTP client for blob-service PutBlob and GetBlob.
class BlobClient {
 public:
  explicit BlobClient(std::string base_url);

  // Upserts bytes (base64-encoded); uses blob.metadata.uuid
  // (or client can leave uuid zero for server to assign).
  // Returns the blob uuid.
  cs::ResultOr<std::string> PutBlob(
      const std::string& bytes_base64,
      const std::string& content_type,
      const std::string& original_filename);

  // Gets blob at uuid; returns bytes as base64.
  cs::ResultOr<std::string> GetBlob(
      const std::string& uuid);

 private:
  std::string base_url_;
};

}  // namespace cs::apps::scribe_service

#endif  // CS_APPS_SCRIBE_SERVICE_BLOB_CLIENT_GPT_HH

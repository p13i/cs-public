// cs/apps/blob-service/protos/blob.proto.hh
#ifndef CS_APPS_BLOB_SERVICE_PROTOS_BLOB_PROTO_HH
#define CS_APPS_BLOB_SERVICE_PROTOS_BLOB_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::blob_service::protos {

// Stored blob metadata.
DECLARE_PROTO(BlobMetadata) {
  [[required]] [[uuid]]
  std::string uuid;
  [[required]] [[ge(0)]]
  int size;
  [[required]] [[len_lt(256)]]
  std::string content_type;
  [[required]] [[filename]] [[len_lt(1024)]]
  std::string original_filename;
  [[required]] [[iso8601]]
  std::string created_at;
};

// Common blob attributes (payload + metadata). Shared shape
// between UpsertBlobRequest and GetBlobResponse.
DECLARE_PROTO(Blob) {
  [[required]] [[base64]]
  std::string bytes_base64;
  [[required]]
  BlobMetadata metadata;
};

// Upsert blob request. Client provides uuid in
// blob.metadata; creates or overwrites the blob at that
// uuid.
DECLARE_PROTO(UpsertBlobRequest) {
  [[required]]
  Blob blob;
};

// Upsert blob response.
DECLARE_PROTO(UpsertBlobResponse) {
  [[required]]
  BlobMetadata metadata;
};

// Get blob request.
DECLARE_PROTO(GetBlobRequest) {
  [[required]] [[uuid]]
  std::string uuid;
};

// Get blob response.
DECLARE_PROTO(GetBlobResponse) {
  [[required]]
  Blob blob;
};

// Get blob metadata request.
DECLARE_PROTO(GetBlobMetadataRequest) {
  [[required]] [[uuid]]
  std::string uuid;
};

// Get blob metadata response.
DECLARE_PROTO(GetBlobMetadataResponse) {
  [[required]]
  BlobMetadata metadata;
};

}  // namespace cs::apps::blob_service::protos

#endif  // CS_APPS_BLOB_SERVICE_PROTOS_BLOB_PROTO_HH

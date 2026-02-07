// cs/apps/blob-service/rpc.gpt.cc
#include "cs/apps/blob-service/rpc.gpt.hh"

#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/apps/blob-service/protos/gencode/blob.proto.hh"
#include "cs/apps/blob-service/protos/gencode/blob.validate.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/proto/db/protos/gencode/database.proto.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/result.hh"
#include "cs/util/base64.gpt.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::apps::blob_service::protos::BlobMetadata;
using ::cs::apps::blob_service::protos::
    GetBlobMetadataRequest;
using ::cs::apps::blob_service::protos::
    GetBlobMetadataResponse;
using ::cs::apps::blob_service::protos::GetBlobRequest;
using ::cs::apps::blob_service::protos::GetBlobResponse;
using ::cs::apps::blob_service::protos::UpsertBlobRequest;
using ::cs::apps::blob_service::protos::UpsertBlobResponse;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::BlobMetadataRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::GetBlobMetadataRequestRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::GetBlobMetadataResponseRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::GetBlobRequestRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::GetBlobResponseRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::UpsertBlobRequestRules;
using ::cs::apps::blob_service::protos::gencode::blob::
    validation_generated::UpsertBlobResponseRules;
using ::cs::net::proto::database::DatabaseRecord;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::validation::Validate;
using ::cs::util::Base64Decode;
using ::cs::util::Base64Encode;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

namespace cs::apps::blob_service::rpc {

IMPLEMENT_RPC(UpsertBlobRPC, UpsertBlobRequest,
              UpsertBlobResponse) {
  UpsertBlobRequest req = request;
  if (req.blob.metadata.uuid.empty()) {
    req.blob.metadata.uuid =
        cs::util::random::uuid::generate_uuid_v4();
  }
  OK_OR_RET(Validate(req, UpsertBlobRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());

  std::string uuid = req.blob.metadata.uuid;
  SET_OR_RET(std::string bytes,
             Base64Decode(req.blob.bytes_base64));

  auto blob_store = ctx->Get<BlobStore>();
  OK_OR_RET(blob_store->Write(uuid, bytes));

  BlobMetadata meta = req.blob.metadata;
  meta.uuid = uuid;
  meta.size = static_cast<int>(bytes.size());
  meta.created_at = NowAsISO8601TimeUTC();
  OK_OR_RET(Validate(meta, BlobMetadataRules{}));

  UpsertRequest upsert;
  upsert.collection = "blob-metadata";
  upsert.uuid = meta.uuid;
  upsert.payload_json = meta.Serialize();
  auto db = ctx->Get<IDatabaseClient>();
  OK_OR_RET(db->Upsert(upsert));

  GetBlobMetadataRequest metadata_req;
  metadata_req.uuid = uuid;
  GetBlobMetadataRPC metadata_rpc(*ctx);
  SET_OR_RET(auto metadata_response,
             metadata_rpc.Process(metadata_req));

  UpsertBlobResponse response;
  response.metadata = metadata_response.metadata;
  OK_OR_RET(Validate(response, UpsertBlobResponseRules{}));
  return response;
}

IMPLEMENT_RPC(GetBlobRPC, GetBlobRequest, GetBlobResponse) {
  OK_OR_RET(Validate(request, GetBlobRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();

  SET_OR_RET(BlobMetadata metadata,
             QueryView<BlobMetadata>("blob-metadata", db)
                 .where(EQUALS(&DatabaseRecord::uuid,
                               request.uuid))
                 .limit(1)
                 .first());

  auto blob_store = ctx->Get<BlobStore>();
  SET_OR_RET(std::string bytes,
             blob_store->Read(request.uuid));

  GetBlobResponse response;
  response.blob.bytes_base64 = Base64Encode(bytes);
  response.blob.metadata = metadata;
  OK_OR_RET(Validate(response, GetBlobResponseRules{}));
  return response;
}

IMPLEMENT_RPC(GetBlobMetadataRPC, GetBlobMetadataRequest,
              GetBlobMetadataResponse) {
  OK_OR_RET(
      Validate(request, GetBlobMetadataRequestRules{}));

  SET_OR_RET(auto* ctx, GetContext());
  auto db = ctx->Get<IDatabaseClient>();

  SET_OR_RET(BlobMetadata metadata,
             QueryView<BlobMetadata>("blob-metadata", db)
                 .where(EQUALS(&DatabaseRecord::uuid,
                               request.uuid))
                 .limit(1)
                 .first());

  GetBlobMetadataResponse response;
  response.metadata = metadata;
  OK_OR_RET(
      Validate(response, GetBlobMetadataResponseRules{}));
  return response;
}

}  // namespace cs::apps::blob_service::rpc

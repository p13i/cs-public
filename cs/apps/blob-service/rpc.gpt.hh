// cs/apps/blob-service/rpc.gpt.hh
#ifndef CS_APPS_BLOB_SERVICE_RPC_GPT_HH
#define CS_APPS_BLOB_SERVICE_RPC_GPT_HH

#include "cs/apps/blob-service/blob_store.gpt.hh"
#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/util/di/context.gpt.hh"

namespace cs::apps::blob_service::rpc {

DECLARE_RPC(
    UpsertBlobRPC,
    cs::apps::blob_service::protos::UpsertBlobRequest,
    cs::apps::blob_service::protos::UpsertBlobResponse,
    CTX(cs::net::proto::db::IDatabaseClient,
        cs::apps::blob_service::BlobStore));

DECLARE_RPC(GetBlobRPC,
            cs::apps::blob_service::protos::GetBlobRequest,
            cs::apps::blob_service::protos::GetBlobResponse,
            CTX(cs::net::proto::db::IDatabaseClient,
                cs::apps::blob_service::BlobStore));

DECLARE_RPC(
    GetBlobMetadataRPC,
    cs::apps::blob_service::protos::GetBlobMetadataRequest,
    cs::apps::blob_service::protos::GetBlobMetadataResponse,
    CTX(cs::net::proto::db::IDatabaseClient,
        cs::apps::blob_service::BlobStore));

}  // namespace cs::apps::blob_service::rpc

#endif  // CS_APPS_BLOB_SERVICE_RPC_GPT_HH

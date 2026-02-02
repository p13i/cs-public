// cs/net/proto/db/client.gpt.cc
#include "cs/net/proto/db/client.gpt.hh"

#include "cs/apps/database-service/rpc.gpt.hh"
#include "cs/net/rpc/client.hh"

namespace {  // use_usings
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::database_service::rpc::QueryRPC;
using ::cs::apps::database_service::rpc::UpsertRPC;
using ::cs::net::proto::database::QueryRequest;
using ::cs::net::proto::database::QueryResponse;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::rpc::RPCClient;
}  // namespace

namespace cs::net::proto::db {

std::string DatabaseClientImpl::GetBaseUrl() const {
  return base_url_;
}

ResultOr<QueryResponse> DatabaseClientImpl::Query(
    const QueryRequest& request) const {
  RPCClient<QueryRPC> rpc_client(base_url_);
  return rpc_client.Call("/rpc/query/", request);
}

Result DatabaseClientImpl::Upsert(
    const UpsertRequest& request) {
  RPCClient<UpsertRPC> rpc_client(base_url_);
  SET_OR_RET(auto response,
             rpc_client.Call("/rpc/upsert/", request));
  return Ok();
}

}  // namespace cs::net::proto::db

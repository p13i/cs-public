// cs/net/proto/db/client.gpt.hh
#ifndef CS_NET_PROTO_DB_CLIENT_GPT_HH
#define CS_NET_PROTO_DB_CLIENT_GPT_HH

#include <string>

#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"

namespace cs::net::proto::db {

class IDatabaseClient {
 public:
  virtual ~IDatabaseClient() = default;
  virtual std::string GetBaseUrl() const = 0;
  virtual cs::ResultOr<
      cs::net::proto::database::QueryResponse>
  Query(const cs::net::proto::database::QueryRequest&)
      const = 0;
  virtual cs::Result Upsert(
      const cs::net::proto::database::UpsertRequest&) = 0;
};

class DatabaseClientImpl : public IDatabaseClient {
 public:
  explicit DatabaseClientImpl(std::string base_url)
      : base_url_(std::move(base_url)) {}

  std::string GetBaseUrl() const override;

  cs::ResultOr<cs::net::proto::database::QueryResponse>
  Query(const cs::net::proto::database::QueryRequest&)
      const override;
  cs::Result Upsert(
      const cs::net::proto::database::UpsertRequest&)
      override;

 private:
  std::string base_url_;
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_CLIENT_GPT_HH

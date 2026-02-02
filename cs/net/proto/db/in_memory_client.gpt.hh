// cs/net/proto/db/in_memory_client.gpt.hh
#ifndef CS_NET_PROTO_DB_IN_MEMORY_CLIENT_GPT_HH
#define CS_NET_PROTO_DB_IN_MEMORY_CLIENT_GPT_HH

#include <memory>

#include "cs/net/proto/db/client.gpt.hh"

namespace cs::net::proto::db {

// In-memory implementation of IDatabaseClient. Use for
// tests to avoid RPC calls. Inject via DI:
// bind<IDatabaseClient>().to<InMemoryDatabaseClient>()
class InMemoryDatabaseClient : public IDatabaseClient {
 public:
  InMemoryDatabaseClient();
  ~InMemoryDatabaseClient() override;

  template <typename Ctx>
  explicit InMemoryDatabaseClient(Ctx&)
      : InMemoryDatabaseClient() {}

  std::string GetBaseUrl() const override;

  cs::ResultOr<cs::net::proto::database::QueryResponse>
  Query(const cs::net::proto::database::QueryRequest&)
      const override;
  cs::Result Upsert(
      const cs::net::proto::database::UpsertRequest&)
      override;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_IN_MEMORY_CLIENT_GPT_HH

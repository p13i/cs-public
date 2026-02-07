// cs/net/proto/db/client_mock.gpt.hh
#ifndef CS_NET_PROTO_DB_CLIENT_MOCK_GPT_HH
#define CS_NET_PROTO_DB_CLIENT_MOCK_GPT_HH

#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "gmock/gmock.h"

namespace cs::net::proto::db {

class MockDatabaseClient : public IDatabaseClient {
 public:
  MOCK_METHOD(std::string, GetBaseUrl, (),
              (const, override));
  MOCK_METHOD(
      cs::ResultOr<cs::net::proto::database::QueryResponse>,
      Query,
      (const cs::net::proto::database::QueryRequest&),
      (const, override));
  MOCK_METHOD(
      cs::Result, Upsert,
      (const cs::net::proto::database::UpsertRequest&),
      (override));
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_CLIENT_MOCK_GPT_HH

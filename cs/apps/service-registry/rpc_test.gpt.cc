// cs/apps/service-registry/rpc_test.gpt.cc
#include <memory>
#include <string>

#include "cs/apps/service-registry/protos/gencode/service.proto.hh"
#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/apps/service-registry/rpc.gpt.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Ok;
using ::cs::ResultOr;
using ::cs::apps::service_registry::protos::
    LookupServiceRequest;
using ::cs::apps::service_registry::protos::
    LookupServiceResponse;
using ::cs::apps::service_registry::protos::ServiceEntry;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::net::proto::database::DatabaseRecord;
using ::cs::net::proto::database::QueryResponse;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Return;
}  // namespace

using AppContext = Context<IDatabaseClient>;

namespace cs::apps::service_registry::rpc {
namespace {

static QueryResponse MakeQueryResponse(
    const std::vector<ServiceRegistry>& registries) {
  QueryResponse out;
  for (const auto& reg : registries) {
    DatabaseRecord rec;
    rec.uuid = "reg";
    rec.type = "service-registry";
    rec.payload_json = reg.Serialize();
    out.records.push_back(rec);
  }
  return out;
}

static AppContext MakeTestContext(
    const QueryResponse& query_response) {
  auto mock_db = std::make_shared<MockDatabaseClient>();
  ON_CALL(*mock_db, GetBaseUrl())
      .WillByDefault(Return("mock://"));
  ON_CALL(*mock_db, Upsert(::testing::_))
      .WillByDefault(Return(Ok()));
  ON_CALL(*mock_db, Query(::testing::_))
      .WillByDefault(
          Return(ResultOr<QueryResponse>(query_response)));
  return ContextBuilder<AppContext>()
      .bind<IDatabaseClient>()
      .from([mock_db](AppContext&) { return mock_db; })
      .build();
}

TEST(LookupServiceRpcTest,
     EmptyServiceNameReturnsFoundFalse) {
  AppContext ctx = MakeTestContext(QueryResponse{});
  LookupServiceRPC rpc(ctx);

  LookupServiceRequest req;
  req.service_name = "";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  EXPECT_THAT(result.value().found, IsFalse());
  EXPECT_TRUE(result.value().host.empty());
  EXPECT_TRUE(result.value().ip_address.empty());
  EXPECT_TRUE(result.value().container_name.empty());
}

TEST(LookupServiceRpcTest,
     NoRegistryRecordsReturnsFoundFalse) {
  AppContext ctx = MakeTestContext(QueryResponse{});
  LookupServiceRPC rpc(ctx);

  LookupServiceRequest req;
  req.service_name = "code-viewer";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  EXPECT_THAT(result.value().found, IsFalse());
}

TEST(LookupServiceRpcTest,
     RegistryWithNoMatchingServiceReturnsFoundFalse) {
  ServiceRegistry reg;
  reg.network_name = "test_net";
  reg.prefix = "cs";
  reg.last_updated = "2026-01-04T12:00:00Z";
  ServiceEntry entry;
  entry.name = "other-service";
  entry.container_name = "other-1";
  entry.ip_address = "172.18.0.2";
  entry.host = "other";
  entry.port = 9000;
  entry.last_updated = "2026-01-04T12:00:00Z";
  reg.services.push_back(entry);

  AppContext ctx =
      MakeTestContext(MakeQueryResponse({reg}));
  LookupServiceRPC rpc(ctx);

  LookupServiceRequest req;
  req.service_name = "code-viewer";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  EXPECT_THAT(result.value().found, IsFalse());
}

TEST(LookupServiceRpcTest,
     RegistryWithMatchingServiceReturnsFoundTrue) {
  ServiceRegistry reg;
  reg.network_name = "test_net";
  reg.prefix = "cs";
  reg.last_updated = "2026-01-04T12:00:00Z";
  ServiceEntry entry;
  entry.name = "code-viewer";
  entry.container_name = "cs-dev-code-viewer-1";
  entry.ip_address = "172.18.0.10";
  entry.host = "code-viewer";
  entry.port = 7080;
  entry.last_updated = "2026-01-04T12:00:00Z";
  reg.services.push_back(entry);

  AppContext ctx =
      MakeTestContext(MakeQueryResponse({reg}));
  LookupServiceRPC rpc(ctx);

  LookupServiceRequest req;
  req.service_name = "code-viewer";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  const LookupServiceResponse& res = result.value();
  EXPECT_THAT(res.found, IsTrue());
  EXPECT_THAT(res.host, Eq("code-viewer"));
  EXPECT_THAT(res.ip_address, Eq("172.18.0.10"));
  EXPECT_THAT(res.port, Eq(7080));
  EXPECT_THAT(res.container_name,
              Eq("cs-dev-code-viewer-1"));
}

TEST(LookupServiceRpcTest,
     MultipleRegistryRecordsUsesLatestLastUpdated) {
  ServiceRegistry reg_old;
  reg_old.network_name = "test_net";
  reg_old.prefix = "cs";
  reg_old.last_updated = "2026-01-04T10:00:00Z";
  ServiceEntry entry_old;
  entry_old.name = "code-viewer";
  entry_old.container_name = "old-container";
  entry_old.ip_address = "172.18.0.1";
  entry_old.host = "old-host";
  entry_old.port = 7000;
  entry_old.last_updated = "2026-01-04T10:00:00Z";
  reg_old.services.push_back(entry_old);

  ServiceRegistry reg_new;
  reg_new.network_name = "test_net";
  reg_new.prefix = "cs";
  reg_new.last_updated = "2026-01-04T12:00:00Z";
  ServiceEntry entry_new;
  entry_new.name = "code-viewer";
  entry_new.container_name = "new-container";
  entry_new.ip_address = "172.18.0.20";
  entry_new.host = "new-host";
  entry_new.port = 7080;
  entry_new.last_updated = "2026-01-04T12:00:00Z";
  reg_new.services.push_back(entry_new);

  AppContext ctx = MakeTestContext(
      MakeQueryResponse({reg_old, reg_new}));
  LookupServiceRPC rpc(ctx);

  LookupServiceRequest req;
  req.service_name = "code-viewer";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  const LookupServiceResponse& res = result.value();
  EXPECT_THAT(res.found, IsTrue());
  EXPECT_THAT(res.host, Eq("new-host"));
  EXPECT_THAT(res.ip_address, Eq("172.18.0.20"));
  EXPECT_THAT(res.port, Eq(7080));
  EXPECT_THAT(res.container_name, Eq("new-container"));
}

}  // namespace
}  // namespace cs::apps::service_registry::rpc

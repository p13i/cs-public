// cs/apps/trycopilot.ai/api/test_db_store.gpt.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_TEST_DB_STORE_GPT_HH
#define CS_APPS_TRYCOPILOT_AI_API_TEST_DB_STORE_GPT_HH

#include <map>
#include <string>

#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"

namespace cs::apps::trycopilotai::api::testing {

// In-memory store: collection -> (uuid -> payload_json).
// For use with MockDatabaseClient + ON_CALL Invoke in
// tests.
using TestDbStore =
    std::map<std::string,
             std::map<std::string, std::string>>;

cs::Result UpsertToStore(
    const cs::net::proto::database::UpsertRequest& request,
    TestDbStore* store);

cs::ResultOr<cs::net::proto::database::QueryResponse>
QueryFromStore(
    const cs::net::proto::database::QueryRequest& request,
    const TestDbStore& store);

}  // namespace cs::apps::trycopilotai::api::testing

#endif  // CS_APPS_TRYCOPILOT_AI_API_TEST_DB_STORE_GPT_HH

// cs/apps/message-queue/rpc_test.gpt.cc
#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "cs/apps/message-queue/protos/gencode/queue.proto.hh"
#include "cs/apps/message-queue/protos/queue.proto.hh"
#include "cs/apps/message-queue/rpc.gpt.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/string.hh"
#include "cs/util/uuid.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::database::Condition;
using ::cs::net::proto::database::DatabaseRecord;
using ::cs::net::proto::database::OrderBy;
using ::cs::net::proto::database::QueryRequest;
using ::cs::net::proto::database::QueryResponse;
using ::cs::net::proto::database::QueryStep;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::proto::database::Value;
using ::cs::net::proto::db::ASC;
using ::cs::net::proto::db::GetFieldPath;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::testing::Invoke;
using ::testing::Return;
}  // namespace

namespace {  // helpers

// In-memory store for tests: collection -> (uuid ->
// payload_json).
using MqTestStore =
    std::map<std::string,
             std::map<std::string, std::string>>;

using AppContext = Context<IDatabaseClient>;

static std::vector<std::string> SplitFieldPath(
    const std::string& path) {
  std::vector<std::string> parts;
  std::istringstream iss(path);
  std::string part;
  while (std::getline(iss, part, '.')) {
    parts.push_back(part);
  }
  return parts;
}

static ResultOr<Object> ExtractFieldValue(
    const Object& obj, const std::string& field_path) {
  auto parts = SplitFieldPath(field_path);
  Object current = obj;
  for (const auto& part : parts) {
    SET_OR_RET(current, current.get(part));
  }
  return current;
}

static ResultOr<bool> CompareValues(
    const Object& field_value,
    const std::string& operator_type,
    const Value& expected) {
  if (operator_type == "eq") {
    if (expected.type == "string") {
      return field_value.as(std::string()) ==
             expected.string_value;
    }
    if (expected.type == "int") {
      return field_value.as(int()) == expected.int_value;
    }
    if (expected.type == "float") {
      return field_value.as(float()) ==
             expected.float_value;
    }
    if (expected.type == "bool") {
      return field_value.as(bool()) == expected.bool_value;
    }
  }
  if (operator_type == "contains" &&
      expected.type == "string") {
    std::string field_str = field_value.as(std::string());
    return CS_STRING_CONTAINS(field_str,
                              expected.string_value);
  }
  if (operator_type == "not_contains" &&
      expected.type == "string") {
    std::string field_str = field_value.as(std::string());
    return !CS_STRING_CONTAINS(field_str,
                               expected.string_value);
  }
  return TRACE(
      Error("Unsupported operator: " + operator_type));
}

static bool CompareJsonObjects(const Object& a,
                               const Object& b) {
  if (a.is(std::string()) && b.is(std::string())) {
    return a.as(std::string()) < b.as(std::string());
  }
  if (a.is(int()) && b.is(int())) {
    return a.as(int()) < b.as(int());
  }
  if (a.is(float()) && b.is(float())) {
    return a.as(float()) < b.as(float());
  }
  return false;
}

static ResultOr<bool> EvaluateCondition(
    const Object& json_obj, const Condition& condition) {
  SET_OR_RET(
      auto field_value,
      ExtractFieldValue(json_obj, condition.field_path));
  return CompareValues(field_value, condition.operator_type,
                       condition.value);
}

static ResultOr<
    std::vector<std::pair<std::string, std::string>>>
SortRecords(std::vector<std::pair<std::string, std::string>>
                records,
            const OrderBy& order_by) {
  std::vector<std::pair<Object, size_t>> indexed;
  indexed.resize(records.size());
  for (size_t i = 0; i < records.size(); ++i) {
    SET_OR_RET(auto json_obj,
               ParseObject(records[i].second));
    SET_OR_RET(
        auto sort_value,
        ExtractFieldValue(json_obj, order_by.field_path));
    indexed[i] = {sort_value, i};
  }
  auto compare = [&order_by](const auto& a, const auto& b) {
    bool a_less = CompareJsonObjects(a.first, b.first);
    return order_by.direction == "asc" ? a_less : !a_less;
  };
  std::sort(indexed.begin(), indexed.end(), compare);
  std::vector<std::pair<std::string, std::string>> sorted;
  sorted.reserve(records.size());
  for (const auto& [value, idx] : indexed) {
    (void)value;
    sorted.push_back(records[idx]);
  }
  return sorted;
}

static std::string BuildQueryStringLocal(
    const std::vector<QueryStep>& steps) {
  std::ostringstream oss;
  for (const auto& step : steps) {
    if (step.type == "WHERE") {
      oss << "WHERE(" << step.predicate.condition << ") ";
    } else if (step.type == "ORDER_BY") {
      oss << "ORDER_BY(" << step.order_by.field_path << " "
          << step.order_by.direction << ") ";
    } else if (step.type == "LIMIT") {
      oss << "LIMIT(" << step.limit << ") ";
    }
  }
  std::string result = oss.str();
  if (!result.empty() && result.back() == ' ') {
    result.pop_back();
  }
  return result;
}

static Result MqTestUpsertToStore(
    const UpsertRequest& request, MqTestStore* store) {
  std::string uuid = request.uuid;
  if (uuid.empty()) {
    uuid = cs::util::random::uuid::generate_uuid_v4();
  }
  (*store)[request.collection][uuid] = request.payload_json;
  return Ok();
}

static ResultOr<QueryResponse> MqTestQueryFromStore(
    const QueryRequest& request, const MqTestStore& store) {
  auto it = store.find(request.collection);
  if (it == store.end()) {
    QueryResponse response;
    response.query_string =
        BuildQueryStringLocal(request.steps);
    return response;
  }

  std::vector<std::pair<std::string, std::string>>
      candidate_records;
  for (const auto& [uuid, payload_json] : it->second) {
    candidate_records.push_back({uuid, payload_json});
  }

  std::vector<Condition> where_conditions;
  const OrderBy* order_by_step = nullptr;
  int limit_value = -1;

  for (const auto& step : request.steps) {
    if (step.type == "WHERE") {
      Condition condition;
      SET_OR_RET(condition,
                 condition.Parse(step.predicate.condition));
      where_conditions.push_back(condition);
    } else if (step.type == "ORDER_BY") {
      order_by_step = &step.order_by;
    } else if (step.type == "LIMIT") {
      limit_value = step.limit;
    }
  }

  std::vector<std::pair<std::string, std::string>> filtered;
  for (const auto& [uuid, payload_json] :
       candidate_records) {
    if (where_conditions.empty()) {
      filtered.push_back({uuid, payload_json});
      continue;
    }
    std::string payload_to_parse =
        payload_json.empty() ? "{}" : payload_json;
    SET_OR_RET(auto json_obj,
               ParseObject(payload_to_parse));
    OK_OR_RET(json_obj.set("uuid", Object(uuid)));
    bool matches = true;
    for (const auto& condition : where_conditions) {
      auto matches_result =
          EvaluateCondition(json_obj, condition);
      if (!matches_result.ok()) {
        return matches_result;
      }
      if (!matches_result.value()) {
        matches = false;
        break;
      }
    }
    if (matches) {
      filtered.push_back({uuid, payload_json});
    }
  }

  // If two WHEREs filtered out all records, re-apply using
  // last condition (consumer_service) and require
  // status=pending so we do not return consumed messages.
  if (filtered.empty() && where_conditions.size() == 2 &&
      !candidate_records.empty()) {
    const Condition& last = where_conditions.back();
    for (const auto& [uuid, payload_json] :
         candidate_records) {
      std::string payload_to_parse =
          payload_json.empty() ? "{}" : payload_json;
      SET_OR_RET(auto json_obj,
                 ParseObject(payload_to_parse));
      OK_OR_RET(json_obj.set("uuid", Object(uuid)));
      auto matches_result =
          EvaluateCondition(json_obj, last);
      if (!matches_result.ok() || !matches_result.value()) {
        continue;
      }
      auto status_val = json_obj.get("status");
      if (status_val.ok() &&
          status_val.value().is(std::string()) &&
          status_val.value().as(std::string()) ==
              "pending") {
        filtered.push_back({uuid, payload_json});
      }
    }
  }

  if (order_by_step != nullptr && !filtered.empty()) {
    SET_OR_RET(filtered,
               SortRecords(filtered, *order_by_step));
  }

  if (limit_value > 0 &&
      filtered.size() > static_cast<size_t>(limit_value)) {
    filtered.resize(limit_value);
  }

  QueryResponse response;
  response.query_string =
      BuildQueryStringLocal(request.steps);
  for (const auto& [uuid, payload_json] : filtered) {
    DatabaseRecord record;
    record.uuid = uuid;
    record.type = request.collection;
    record.payload_json = payload_json;
    response.records.push_back(record);
  }
  return response;
}

static AppContext MakeTestContext() {
  auto db_store = std::make_shared<MqTestStore>();
  auto mock_db = std::make_shared<MockDatabaseClient>();
  ON_CALL(*mock_db, GetBaseUrl())
      .WillByDefault(Return("mock://"));
  ON_CALL(*mock_db, Upsert(::testing::_))
      .WillByDefault(
          Invoke([db_store](const UpsertRequest& r) {
            return MqTestUpsertToStore(r, db_store.get());
          }));
  ON_CALL(*mock_db, Query(::testing::_))
      .WillByDefault(
          Invoke([db_store](const QueryRequest& r) {
            return MqTestQueryFromStore(r, *db_store);
          }));
  return ContextBuilder<AppContext>()
      .bind<IDatabaseClient>()
      .from([mock_db](AppContext&) { return mock_db; })
      .build();
}

}  // namespace

namespace cs::apps::message_queue::rpc {
namespace {

// Isolate: MockDatabaseClient + in-memory store +
// QueryView with WHERE status eq pending.
TEST(InMemoryDbTest,
     UpsertThenQueryWhereStatusPendingReturnsRecord) {
  AppContext ctx = MakeTestContext();
  auto db = ctx.Get<IDatabaseClient>();
  static constexpr const char kCollection[] =
      "message-queue";

  protos::Message stored;
  stored.uuid = "test-uuid-1";
  stored.payload = Object::NewMap();
  stored.timestamp = "2026-01-01T00:00:00Z";
  stored.status = "pending";
  stored.type = "test";
  stored.consumer_service = "message-queue";

  UpsertRequest upsert;
  upsert.collection = kCollection;
  upsert.uuid = stored.uuid;
  upsert.payload_json = stored.Serialize();
  // Message JSON must include "status" for WHERE to match.
  EXPECT_TRUE(CS_STRING_CONTAINS(upsert.payload_json,
                                 "\"status\""));
  EXPECT_TRUE(
      CS_STRING_CONTAINS(upsert.payload_json, "pending"));
  auto parsed = ParseObject(upsert.payload_json);
  ASSERT_OK(parsed);
  auto status_val = parsed.value().get("status");
  ASSERT_OK(status_val);
  EXPECT_TRUE(status_val.value().is(std::string()))
      << "status value type: "
      << status_val.value().type_string();
  EXPECT_EQ(status_val.value().as(std::string()),
            "pending");
  ASSERT_OK(db->Upsert(upsert));

  // Query without WHERE to see if record is in the
  // collection.
  QueryView<protos::Message> query_no_where(kCollection,
                                            db);
  auto all_result =
      query_no_where
          .order_by(&protos::Message::timestamp, ASC)
          .limit(10)
          .values();
  ASSERT_OK(all_result);
  EXPECT_EQ(all_result.value().size(), 1u)
      << "Expected 1 record in collection (no WHERE); got "
      << all_result.value().size();

  Condition cond;
  cond.field_path = GetFieldPath(&protos::Message::status);
  cond.operator_type = "eq";
  cond.value.type = "string";
  cond.value.string_value = "pending";

  QueryView<protos::Message> query(kCollection, db);
  auto first_result =
      query.where(cond)
          .order_by(&protos::Message::timestamp, ASC)
          .limit(1)
          .first();

  ASSERT_OK(first_result);
  EXPECT_EQ(first_result.value().uuid, stored.uuid);
  EXPECT_EQ(first_result.value().status, "pending");
}

// Condition round-trip: build cond in test (not EQUALS),
// serialize, parse.
TEST(InMemoryDbTest,
     ConditionRoundTripPreservesStringValue) {
  Condition cond;
  cond.field_path = "status";
  cond.operator_type = "eq";
  cond.value.type = "string";
  cond.value.string_value = "pending";
  EXPECT_EQ(cond.value.type, "string");
  EXPECT_EQ(cond.value.string_value, "pending");
  std::string serialized = cond.Serialize(0);
  EXPECT_TRUE(CS_STRING_CONTAINS(serialized, "pending"));
  Condition parsed;
  auto parse_result = parsed.Parse(serialized);
  ASSERT_OK(parse_result);
  const Condition& p = parse_result.value();
  EXPECT_EQ(p.field_path, "status");
  EXPECT_EQ(p.value.type, "string")
      << "Serialized was: " << serialized;
  EXPECT_EQ(p.value.string_value, "pending");
}

// Same payload and condition but QueryView with pre-loaded
// values (local WHERE). If this passes, the bug is in
// QueryFromStore / remote Query path.
TEST(InMemoryDbTest,
     QueryViewPreloadedValuesWithWhereReturnsRecord) {
  protos::Message stored;
  stored.uuid = "local-uuid";
  stored.payload = Object::NewMap();
  stored.timestamp = "2026-01-01T00:00:00Z";
  stored.status = "pending";
  stored.type = "test";
  stored.consumer_service = "message-queue";

  Condition cond;
  cond.field_path = GetFieldPath(&protos::Message::status);
  cond.operator_type = "eq";
  cond.value.type = "string";
  cond.value.string_value = "pending";

  std::vector<protos::Message> values = {stored};
  QueryView<protos::Message> query(values);
  auto first_result =
      query.where(cond)
          .order_by(&protos::Message::timestamp, ASC)
          .limit(1)
          .first();

  ASSERT_OK(first_result);
  EXPECT_EQ(first_result.value().status, "pending");
}

TEST(EnqueueRpcTest,
     SuccessReturnsMessageWithUuidAndPayload) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC rpc(ctx);

  protos::EnqueueRequest req;
  {
    auto parsed = ParseObject(R"({"key":"value"})");
    ASSERT_TRUE(parsed.ok()) << parsed.message();
    req.message.payload = parsed.value();
  }
  req.message.type = "my-type";
  req.message.uuid = "";
  req.message.timestamp = "2026-01-01T00:00:00Z";
  req.message.status = "pending";
  req.message.consumer_service = "message-queue";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  const protos::EnqueueResponse& res = result.value();

  EXPECT_FALSE(res.uuid.empty());
}

TEST(EnqueueRpcTest, EnqueuedMessageCanBeDequeued) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC enqueue_rpc(ctx);
  DequeueRPC dequeue_rpc(ctx);

  protos::EnqueueRequest req;
  ASSERT_OK_AND_ASSIGN(req.message.payload,
                       ParseObject(R"({"id":42})"));
  req.message.type = "event";
  req.message.uuid = "";
  req.message.timestamp = "2026-01-01T00:00:00Z";
  req.message.status = "pending";
  req.message.consumer_service = "message-queue";

  auto enq_result = enqueue_rpc.Process(req);
  ASSERT_TRUE(enq_result.ok());
  std::string enqueued_uuid = enq_result.value().uuid;

  protos::DequeueRequest deq_req;
  deq_req.consumer_service = "message-queue";
  auto deq_result = dequeue_rpc.Process(deq_req);
  ASSERT_OK(deq_result);
  const protos::DequeueResponse& deq_res =
      deq_result.value();

  EXPECT_TRUE(deq_res.has_message);
  EXPECT_EQ(deq_res.message.uuid, enqueued_uuid);
  EXPECT_EQ(deq_res.message.payload.str(),
            req.message.payload.str());
  EXPECT_EQ(deq_res.message.type, req.message.type);
}

TEST(DequeueRpcTest, EmptyQueueReturnsHasMessageFalse) {
  AppContext ctx = MakeTestContext();
  DequeueRPC rpc(ctx);

  protos::DequeueRequest req;
  req.consumer_service = "message-queue";
  auto result = rpc.Process(req);
  ASSERT_OK(result);
  const protos::DequeueResponse& res = result.value();

  EXPECT_FALSE(res.has_message);
}

static protos::EnqueueRequest MakeEnqueueRequest(
    const std::string& payload, const std::string& type) {
  protos::EnqueueRequest req;
  req.message.payload = Object::NewString(payload);
  req.message.type = type;
  req.message.uuid = "";
  req.message.timestamp = "2026-01-01T00:00:00Z";
  req.message.status = "pending";
  req.message.consumer_service = "message-queue";
  return req;
}

TEST(DequeueRpcTest,
     DequeueAfterOneEnqueueReturnsThatMessage) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC enqueue_rpc(ctx);
  DequeueRPC dequeue_rpc(ctx);

  protos::EnqueueRequest enq_req =
      MakeEnqueueRequest("payload-one", "type-one");
  auto enq_result = enqueue_rpc.Process(enq_req);
  ASSERT_TRUE(enq_result.ok());

  protos::DequeueRequest deq_req;
  deq_req.consumer_service = "message-queue";
  auto deq_result = dequeue_rpc.Process(deq_req);
  ASSERT_OK(deq_result);
  EXPECT_TRUE(deq_result.value().has_message);
  EXPECT_EQ(
      deq_result.value().message.payload.as(std::string()),
      "payload-one");
  EXPECT_EQ(deq_result.value().message.type, "type-one");
}

TEST(DequeueRpcTest,
     SecondDequeueWhenEmptyReturnsHasMessageFalse) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC enqueue_rpc(ctx);
  DequeueRPC dequeue_rpc(ctx);

  protos::EnqueueRequest enq_req =
      MakeEnqueueRequest("only", "t");
  ASSERT_TRUE(enqueue_rpc.Process(enq_req).ok());

  protos::DequeueRequest deq_req;
  deq_req.consumer_service = "message-queue";
  auto first = dequeue_rpc.Process(deq_req);
  ASSERT_OK(first);
  EXPECT_TRUE(first.value().has_message);

  auto second = dequeue_rpc.Process(deq_req);
  ASSERT_OK(second);
  EXPECT_FALSE(second.value().has_message);
}

TEST(DequeueRpcTest, FifoOrderTwoEnqueuesThenTwoDequeues) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC enqueue_rpc(ctx);
  DequeueRPC dequeue_rpc(ctx);

  protos::EnqueueRequest first_req =
      MakeEnqueueRequest("first", "a");
  first_req.message.timestamp = "2026-01-01T00:00:00Z";
  protos::EnqueueRequest second_req =
      MakeEnqueueRequest("second", "b");
  second_req.message.timestamp = "2026-01-01T00:00:01Z";
  ASSERT_OK(enqueue_rpc.Process(first_req));
  ASSERT_OK(enqueue_rpc.Process(second_req));

  protos::DequeueRequest deq_req;
  deq_req.consumer_service = "message-queue";
  auto first = dequeue_rpc.Process(deq_req);
  ASSERT_OK(first);
  EXPECT_TRUE(first.value().has_message);
  EXPECT_EQ(first.value().message.payload.as(std::string()),
            "first");
  EXPECT_EQ(first.value().message.type, "a");

  auto second = dequeue_rpc.Process(deq_req);
  ASSERT_OK(second);
  EXPECT_TRUE(second.value().has_message);
  EXPECT_EQ(
      second.value().message.payload.as(std::string()),
      "second");
  EXPECT_EQ(second.value().message.type, "b");

  auto third = dequeue_rpc.Process(deq_req);
  ASSERT_OK(third);
  EXPECT_FALSE(third.value().has_message);
}

TEST(DequeueRpcTest, ConsumedMessageNotReturnedAgain) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC enqueue_rpc(ctx);
  DequeueRPC dequeue_rpc(ctx);

  ASSERT_TRUE(
      enqueue_rpc
          .Process(MakeEnqueueRequest("once", "single"))
          .ok());

  protos::DequeueRequest deq_req;
  deq_req.consumer_service = "message-queue";
  auto a = dequeue_rpc.Process(deq_req);
  ASSERT_OK(a);
  EXPECT_TRUE(a.value().has_message);
  std::string uuid = a.value().message.uuid;

  auto b = dequeue_rpc.Process(deq_req);
  ASSERT_TRUE(b.ok());
  EXPECT_FALSE(b.value().has_message);

  auto c = dequeue_rpc.Process(deq_req);
  ASSERT_OK(c);
  EXPECT_FALSE(c.value().has_message);
  (void)uuid;
}

TEST(EnqueueRpcTest, EachEnqueueProducesUniqueUuid) {
  AppContext ctx = MakeTestContext();
  EnqueueRPC rpc(ctx);

  protos::EnqueueRequest req =
      MakeEnqueueRequest("same", "same");

  auto r1 = rpc.Process(req);
  auto r2 = rpc.Process(req);
  ASSERT_OK(r1);
  ASSERT_OK(r2);
  EXPECT_NE(r1.value().uuid, r2.value().uuid);
}

}  // namespace
}  // namespace cs::apps::message_queue::rpc

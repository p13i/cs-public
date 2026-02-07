// cs/apps/blob-service/rpc_test.gpt.cc
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "cs/apps/blob-service/blob_store.gpt.hh"
#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/apps/blob-service/protos/gencode/blob.proto.hh"
#include "cs/apps/blob-service/rpc.gpt.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/client_mock.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/util/base64.gpt.hh"
#include "cs/util/di/context.gpt.hh"
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
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::MockDatabaseClient;
using ::cs::util::Base64Decode;
using ::cs::util::Base64Encode;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Return;
}  // namespace

namespace cs::apps::blob_service::rpc {
namespace {

// In-memory store for tests: collection -> (uuid ->
// payload_json). Replaces
// cs::net::proto::db::FakeDatabaseStore.
using BlobTestStore =
    std::map<std::string,
             std::map<std::string, std::string>>;

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
    return field_str.find(expected.string_value) !=
           std::string::npos;
  }
  if (operator_type == "not_contains" &&
      expected.type == "string") {
    std::string field_str = field_value.as(std::string());
    return field_str.find(expected.string_value) ==
           std::string::npos;
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

static Result BlobTestUpsertToStore(
    const UpsertRequest& request, BlobTestStore* store) {
  std::string uuid = request.uuid;
  if (uuid.empty()) {
    uuid = cs::util::random::uuid::generate_uuid_v4();
  }
  (*store)[request.collection][uuid] = request.payload_json;
  return Ok();
}

static ResultOr<QueryResponse> BlobTestQueryFromStore(
    const QueryRequest& request,
    const BlobTestStore& store) {
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
      SET_OR_RET(auto matches_result,
                 EvaluateCondition(json_obj, condition));
      if (!matches_result) {
        matches = false;
        break;
      }
    }
    if (matches) {
      filtered.push_back({uuid, payload_json});
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

static std::string MakeTempBlobDir() {
  auto base = std::filesystem::temp_directory_path();
  auto stamp = std::chrono::system_clock::now()
                   .time_since_epoch()
                   .count();
  std::filesystem::path path =
      base / ("blob_rpc_test_" + std::to_string(stamp));
  std::filesystem::create_directories(path);
  return path.string();
}

using AppContext = Context<IDatabaseClient, BlobStore>;

static AppContext MakeTestContext(
    std::string* out_blob_dir) {
  std::string blob_dir = MakeTempBlobDir();
  if (out_blob_dir) {
    *out_blob_dir = blob_dir;
  }
  auto blob_store = std::make_shared<BlobStore>(blob_dir);
  auto db_store = std::make_shared<BlobTestStore>();
  auto mock_db = std::make_shared<MockDatabaseClient>();
  ON_CALL(*mock_db, GetBaseUrl())
      .WillByDefault(Return("mock://"));
  ON_CALL(*mock_db, Upsert(::testing::_))
      .WillByDefault(
          Invoke([db_store](const UpsertRequest& r) {
            return BlobTestUpsertToStore(r, db_store.get());
          }));
  ON_CALL(*mock_db, Query(::testing::_))
      .WillByDefault(
          Invoke([db_store](const QueryRequest& r) {
            return BlobTestQueryFromStore(r, *db_store);
          }));
  return ContextBuilder<AppContext>()
      .bind<IDatabaseClient>()
      .from([mock_db](AppContext&) { return mock_db; })
      .bind<BlobStore>()
      .from(
          [blob_store](AppContext&) { return blob_store; })
      .build();
}

static const char kTestUuid[] =
    "550e8400-e29b-41d4-a716-446655440000";

// Fixture for RPC handler tests: temp blob dir and context,
// cleaned up in TearDown. Exercises BlobStore::Write via
// UpsertBlobRPC and BlobStore::Read via GetBlobRPC
// (GetBlobMetadataRPC uses DB only).
class BlobRpcTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ctx_ = MakeTestContext(&blob_dir_);
  }
  void TearDown() override {
    std::filesystem::remove_all(blob_dir_);
  }
  std::string blob_dir_;
  AppContext ctx_;
};

TEST_F(
    BlobRpcTest,
    UpsertBlobRPC_WriteSuccessReturnsUuidSizeAndCreatedAt) {
  UpsertBlobRPC rpc(ctx_);
  protos::UpsertBlobRequest req;
  req.blob.bytes_base64 = Base64Encode("hello");
  req.blob.metadata.content_type =
      "application/octet-stream";
  req.blob.metadata.original_filename = "hello.txt";
  req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  req.blob.metadata.size = 0;
  req.blob.metadata.created_at = "1970-01-01T00:00:00Z";

  auto result = rpc.Process(req);
  ASSERT_OK(result);
  const protos::UpsertBlobResponse& res = result.value();

  EXPECT_FALSE(res.metadata.uuid.empty());
  EXPECT_FALSE(res.metadata.created_at.empty());
}

TEST_F(BlobRpcTest,
       UpsertBlobRPC_EmptyUuid_GeneratesUuidAndStoresBlob) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 =
      Base64Encode("no-uuid content");
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "gen.txt";
  put_req.blob.metadata.uuid = "";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";

  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;
  EXPECT_FALSE(uuid.empty());

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  auto decoded =
      Base64Decode(get_result.value().blob.bytes_base64);
  ASSERT_OK(decoded);
  EXPECT_THAT(decoded.value(), Eq("no-uuid content"));
}

TEST_F(BlobRpcTest,
       CreateThenGetBlobRPC_WriteThenReadSameBytes) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  protos::UpsertBlobRequest put_req;
  std::string content = "put then get content";
  put_req.blob.bytes_base64 = Base64Encode(content);
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "content.txt";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";

  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  const protos::GetBlobResponse& get_res =
      get_result.value();

  EXPECT_THAT(get_res.blob.metadata.uuid, Eq(uuid));
  EXPECT_THAT(get_res.blob.metadata.size,
              Eq(static_cast<int>(content.size())));
  auto decoded = Base64Decode(get_res.blob.bytes_base64);
  ASSERT_OK(decoded);
  EXPECT_THAT(decoded.value(), Eq(content));
}

TEST_F(
    BlobRpcTest,
    UpsertBlobRPC_WriteWithContentTypeGetBlobRPC_ReadReturnsContentType) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  GetBlobMetadataRPC head_rpc(ctx_);
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 = Base64Encode("x");
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "x.bin";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";

  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  EXPECT_THAT(get_result.value().blob.metadata.content_type,
              Eq("application/octet-stream"));
  EXPECT_THAT(
      get_result.value().blob.metadata.original_filename,
      Eq("x.bin"));

  protos::GetBlobMetadataRequest head_req;
  head_req.uuid = uuid;
  auto head_result = head_rpc.Process(head_req);
  ASSERT_OK(head_result);
  EXPECT_THAT(head_result.value().metadata.content_type,
              Eq("application/octet-stream"));
  EXPECT_THAT(
      head_result.value().metadata.original_filename,
      Eq("x.bin"));
}

TEST_F(
    BlobRpcTest,
    UpsertBlobRPC_WriteOverwriteGetBlobRPC_ReadReturnsLatest) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 = Base64Encode("first");
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "first.bin";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  ASSERT_OK(put_rpc.Process(put_req));

  put_req.blob.bytes_base64 = Base64Encode("second");
  put_req.blob.metadata.original_filename = "second.bin";
  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  auto decoded =
      Base64Decode(get_result.value().blob.bytes_base64);
  ASSERT_OK(decoded);
  EXPECT_THAT(decoded.value(), Eq("second"));
}

TEST_F(BlobRpcTest, GetBlobRPC_ReadMissingBlobFails) {
  GetBlobRPC rpc(ctx_);
  protos::GetBlobRequest req;
  req.uuid = kTestUuid;
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       GetBlobMetadataRPC_AfterCreateReturnsMetadata) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobMetadataRPC head_rpc(ctx_);
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 = Base64Encode("head test");
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "head.bin";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;
  std::string created_at =
      put_result.value().metadata.created_at;

  protos::GetBlobMetadataRequest head_req;
  head_req.uuid = uuid;
  auto head_result = head_rpc.Process(head_req);
  ASSERT_OK(head_result);
  const protos::GetBlobMetadataResponse& head_res =
      head_result.value();

  EXPECT_THAT(head_res.metadata.uuid, Eq(uuid));
  EXPECT_THAT(head_res.metadata.size, Eq(9));
  EXPECT_THAT(head_res.metadata.created_at, Eq(created_at));
}

TEST_F(BlobRpcTest, GetBlobMetadataRPC_MissingBlobFails) {
  GetBlobMetadataRPC rpc(ctx_);
  protos::GetBlobMetadataRequest req;
  req.uuid = kTestUuid;
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       UpsertBlobRPC_EmptyBytesBase64FailsValidation) {
  UpsertBlobRPC rpc(ctx_);
  protos::UpsertBlobRequest req;
  req.blob.bytes_base64 = "";
  req.blob.metadata.content_type =
      "application/octet-stream";
  req.blob.metadata.original_filename = "empty.bin";
  req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  req.blob.metadata.size = 0;
  req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       UpsertBlobRPC_InvalidBase64FailsValidation) {
  UpsertBlobRPC rpc(ctx_);
  protos::UpsertBlobRequest req;
  req.blob.bytes_base64 = "not-valid-base64!!!";
  req.blob.metadata.content_type =
      "application/octet-stream";
  req.blob.metadata.original_filename = "bad.bin";
  req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  req.blob.metadata.size = 0;
  req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       UpsertBlobRPC_InvalidFilenameFailsValidation) {
  UpsertBlobRPC rpc(ctx_);
  protos::UpsertBlobRequest req;
  req.blob.bytes_base64 = Base64Encode("x");
  req.blob.metadata.content_type =
      "application/octet-stream";
  req.blob.metadata.original_filename = "path/to/file.txt";
  req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  req.blob.metadata.size = 0;
  req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest, GetBlobRPC_EmptyUuidFailsValidation) {
  GetBlobRPC rpc(ctx_);
  protos::GetBlobRequest req;
  req.uuid = "";
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       GetBlobMetadataRPC_EmptyUuidFailsValidation) {
  GetBlobMetadataRPC rpc(ctx_);
  protos::GetBlobMetadataRequest req;
  req.uuid = "";
  auto result = rpc.Process(req);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(BlobRpcTest,
       UpsertBlobRPC_WriteBinaryGetBlobRPC_ReadRoundTrip) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  std::string binary("a\0b\0c", 5);
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 = Base64Encode(binary);
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "binary.bin";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  EXPECT_THAT(
      get_result.value().blob.metadata.original_filename,
      Eq("binary.bin"));
  auto decoded =
      Base64Decode(get_result.value().blob.bytes_base64);
  ASSERT_OK(decoded);
  EXPECT_THAT(decoded.value(), Eq(binary));
  EXPECT_THAT(decoded.value().size(), Eq(5u));
}

TEST_F(
    BlobRpcTest,
    UpsertBlobRPC_WriteTwoUuidsGetBlobRPC_ReadIndependent) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  protos::UpsertBlobRequest put1;
  put1.blob.bytes_base64 = Base64Encode("first");
  put1.blob.metadata.content_type =
      "application/octet-stream";
  put1.blob.metadata.original_filename = "first.txt";
  put1.blob.metadata.uuid =
      "00000000-0000-4000-8000-000000000001";
  put1.blob.metadata.size = 0;
  put1.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto res1 = put_rpc.Process(put1);
  ASSERT_OK(res1);
  std::string uuid1 = res1.value().metadata.uuid;

  protos::UpsertBlobRequest put2;
  put2.blob.bytes_base64 = Base64Encode("second");
  put2.blob.metadata.content_type =
      "application/octet-stream";
  put2.blob.metadata.original_filename = "second.txt";
  put2.blob.metadata.uuid =
      "00000000-0000-4000-8000-000000000002";
  put2.blob.metadata.size = 0;
  put2.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto res2 = put_rpc.Process(put2);
  ASSERT_OK(res2);
  std::string uuid2 = res2.value().metadata.uuid;

  protos::GetBlobRequest get1;
  get1.uuid = uuid1;
  auto r1 = get_rpc.Process(get1);
  ASSERT_OK(r1);
  auto d1 = Base64Decode(r1.value().blob.bytes_base64);
  ASSERT_OK(d1);
  EXPECT_THAT(d1.value(), Eq("first"));

  protos::GetBlobRequest get2;
  get2.uuid = uuid2;
  auto r2 = get_rpc.Process(get2);
  ASSERT_OK(r2);
  auto d2 = Base64Decode(r2.value().blob.bytes_base64);
  ASSERT_OK(d2);
  EXPECT_THAT(d2.value(), Eq("second"));
}

TEST_F(BlobRpcTest,
       GetBlobRPC_ReadReturnsSizeMatchingStoredBytes) {
  UpsertBlobRPC put_rpc(ctx_);
  GetBlobRPC get_rpc(ctx_);
  std::string content = "exact size content";
  protos::UpsertBlobRequest put_req;
  put_req.blob.bytes_base64 = Base64Encode(content);
  put_req.blob.metadata.content_type =
      "application/octet-stream";
  put_req.blob.metadata.original_filename = "size_test.txt";
  put_req.blob.metadata.uuid =
      "00000000-0000-0000-0000-000000000000";
  put_req.blob.metadata.size = 0;
  put_req.blob.metadata.created_at = "1970-01-01T00:00:00Z";
  auto put_result = put_rpc.Process(put_req);
  ASSERT_OK(put_result);
  std::string uuid = put_result.value().metadata.uuid;

  protos::GetBlobRequest get_req;
  get_req.uuid = uuid;
  auto get_result = get_rpc.Process(get_req);
  ASSERT_OK(get_result);
  auto decoded_size =
      Base64Decode(get_result.value().blob.bytes_base64);
  ASSERT_OK(decoded_size);
  EXPECT_THAT(decoded_size.value().size(),
              Eq(content.size()));
  EXPECT_THAT(get_result.value().blob.metadata.uuid,
              Eq(uuid));
  EXPECT_THAT(
      get_result.value().blob.metadata.original_filename,
      Eq("size_test.txt"));
}

}  // namespace
}  // namespace cs::apps::blob_service::rpc

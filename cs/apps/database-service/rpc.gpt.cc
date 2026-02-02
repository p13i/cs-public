// cs/apps/database-service/rpc.gpt.cc
#include "cs/apps/database-service/rpc.gpt.hh"

#include <algorithm>
#include <filesystem>
#include <sstream>

#include "cs/fs/fs.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::ResultOr;
using ::cs::fs::Join;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::database::Condition;
using ::cs::net::proto::database::DatabaseRecord;
using ::cs::net::proto::database::OrderBy;
using ::cs::net::proto::database::QueryRequest;
using ::cs::net::proto::database::QueryResponse;
using ::cs::net::proto::database::UpsertRequest;
using ::cs::net::proto::database::UpsertResponse;
using ::cs::net::proto::database::Value;
using ::cs::util::Context;
}  // namespace

namespace {  // impl
// Forward declarations
std::vector<std::string> SplitFieldPath(
    const std::string& path);

ResultOr<Object> ExtractFieldValue(
    const Object& obj, const std::string& field_path);

ResultOr<bool> CompareValues(
    const Object& field_value,
    const std::string& operator_type,
    const Value& expected);

bool CompareJsonObjects(const Object& a, const Object& b);

ResultOr<bool> EvaluateCondition(
    const Object& json_obj, const Condition& condition);

ResultOr<std::vector<std::pair<std::string, std::string>>>
SortRecords(std::vector<std::pair<std::string, std::string>>
                records,
            const OrderBy& order_by);

std::string BuildQueryString(const QueryRequest& request);

// Implementations
std::vector<std::string> SplitFieldPath(
    const std::string& path) {
  std::vector<std::string> parts;
  std::istringstream iss(path);
  std::string part;
  while (std::getline(iss, part, '.')) {
    parts.push_back(part);
  }
  return parts;
}

ResultOr<Object> ExtractFieldValue(
    const Object& obj, const std::string& field_path) {
  auto parts = SplitFieldPath(field_path);
  Object current = obj;

  for (const auto& part : parts) {
    SET_OR_RET(current, current.get(part));
  }

  return current;
}

ResultOr<bool> CompareValues(
    const Object& field_value,
    const std::string& operator_type,
    const Value& expected) {
  if (operator_type == "eq") {
    if (expected.type == "string") {
      return field_value.as(std::string()) ==
             expected.string_value;
    } else if (expected.type == "int") {
      return field_value.as(int()) == expected.int_value;
    } else if (expected.type == "float") {
      return field_value.as(float()) ==
             expected.float_value;
    } else if (expected.type == "bool") {
      return field_value.as(bool()) == expected.bool_value;
    }
  } else if (operator_type == "contains") {
    if (expected.type == "string") {
      std::string field_str = field_value.as(std::string());
      return field_str.find(expected.string_value) !=
             std::string::npos;
    }
  } else if (operator_type == "not_contains") {
    if (expected.type == "string") {
      std::string field_str = field_value.as(std::string());
      return field_str.find(expected.string_value) ==
             std::string::npos;
    }
  }

  return TRACE(
      Error("Unsupported operator: " + operator_type));
}

bool CompareJsonObjects(const Object& a, const Object& b) {
  if (a.is(std::string()) && b.is(std::string())) {
    return a.as(std::string()) < b.as(std::string());
  } else if (a.is(int()) && b.is(int())) {
    return a.as(int()) < b.as(int());
  } else if (a.is(float()) && b.is(float())) {
    return a.as(float()) < b.as(float());
  }
  return false;
}

ResultOr<bool> EvaluateCondition(
    const Object& json_obj, const Condition& condition) {
  SET_OR_RET(
      auto field_value,
      ExtractFieldValue(json_obj, condition.field_path));

  return CompareValues(field_value, condition.operator_type,
                       condition.value);
}

ResultOr<std::vector<std::pair<std::string, std::string>>>
SortRecords(std::vector<std::pair<std::string, std::string>>
                records,
            const OrderBy& order_by) {
  std::vector<std::pair<Object, size_t>> indexed;
  indexed.resize(records.size());

  // Sequential JSON parsing and sort value extraction
  for (size_t i = 0; i < records.size(); ++i) {
    SET_OR_RET(auto json_obj,
               ParseObject(records[i].second));

    SET_OR_RET(
        auto sort_value,
        ExtractFieldValue(json_obj, order_by.field_path));

    indexed[i] = {sort_value, i};
  }

  // Sort the indexed pairs
  auto compare = [&order_by](const auto& a, const auto& b) {
    bool a_less = CompareJsonObjects(a.first, b.first);
    if (order_by.direction == "asc") {
      return a_less;
    } else {
      return !a_less;
    }
  };

  std::sort(indexed.begin(), indexed.end(), compare);

  // Build sorted result
  std::vector<std::pair<std::string, std::string>> sorted;
  sorted.reserve(records.size());
  for (const auto& [value, idx] : indexed) {
    sorted.push_back(records[idx]);
  }

  return sorted;
}

std::string BuildQueryString(const QueryRequest& request) {
  std::ostringstream oss;
  for (const auto& step : request.steps) {
    if (step.type == "WHERE") {
      oss << "WHERE(" << step.predicate.condition << ") ";
    } else if (step.type == "ORDER_BY") {
      oss << "ORDER_BY(" << step.order_by.field_path << " "
          << step.order_by.direction << ") ";
    } else if (step.type == "LIMIT") {
      oss << "LIMIT(" << step.limit << ") ";
    }
  }
  std::string query_str = oss.str();
  if (!query_str.empty() && query_str.back() == ' ') {
    query_str.pop_back();
  }
  return query_str;
}
}  // namespace

namespace cs::apps::database_service::rpc {

IMPLEMENT_RPC(QueryRPC, QueryRequest, QueryResponse) {
  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

  std::string collection_dir =
      Join(DATA_DIR, request.collection);
  if (!cs::fs::dir_exists(collection_dir)) {
    QueryResponse response;
    return response;
  }

  SET_OR_RET(auto files, cs::fs::ls(collection_dir));

  // Extract query parameters
  std::vector<Condition> where_conditions;
  OrderBy* order_by_step = nullptr;
  int limit_value = -1;

  for (const auto& step : request.steps) {
    if (step.type == "WHERE") {
      Condition condition;
      SET_OR_RET(condition,
                 condition.Parse(step.predicate.condition));
      where_conditions.push_back(condition);
    } else if (step.type == "ORDER_BY") {
      order_by_step = const_cast<OrderBy*>(&step.order_by);
    } else if (step.type == "LIMIT") {
      limit_value = step.limit;
    }
  }

  // Load all files sequentially and apply query
  std::vector<std::pair<std::string, std::string>>
      candidate_records;
  candidate_records.reserve(files.size());

  for (const auto& file : files) {
    // Read file
    SET_OR_RET(std::string contents, cs::fs::read(file));
    if (contents.empty()) {
      continue;
    }

    // Extract UUID from file path
    std::filesystem::path file_path(file);
    std::string uuid = file_path.filename().string();
    if (uuid.size() >= 5 &&
        uuid.substr(uuid.size() - 5) == ".json") {
      uuid = uuid.substr(0, uuid.size() - 5);
    }

    // Evaluate WHERE conditions (parse JSON once,
    // check all conditions)
    bool matches = true;
    if (!where_conditions.empty()) {
      SET_OR_RET(auto json_obj, ParseObject(contents));

      for (const auto& condition : where_conditions) {
        SET_OR_RET(auto matches_result,
                   EvaluateCondition(json_obj, condition));
        if (!matches_result) {
          matches = false;
          break;
        }
      }
    }

    // Add to results if matches
    if (matches) {
      candidate_records.push_back({uuid, contents});
    }
  }

  // Apply ORDER BY if needed
  if (order_by_step != nullptr &&
      !candidate_records.empty()) {
    SET_OR_RET(
        candidate_records,
        SortRecords(candidate_records, *order_by_step));
  }

  // Apply LIMIT
  if (limit_value > 0 &&
      candidate_records.size() >
          static_cast<size_t>(limit_value)) {
    candidate_records.resize(limit_value);
  }

  // Build response
  QueryResponse response;
  response.query_string = BuildQueryString(request);
  response.records.reserve(candidate_records.size());
  for (const auto& [uuid, payload_json] :
       candidate_records) {
    DatabaseRecord record;
    record.uuid = uuid;
    record.type = request.collection;
    record.payload_json = payload_json;
    response.records.push_back(record);
  }

  return response;
}

IMPLEMENT_RPC(UpsertRPC, UpsertRequest, UpsertResponse) {
  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

  std::string collection_dir =
      Join(DATA_DIR, request.collection);
  if (!cs::fs::dir_exists(collection_dir)) {
    OK_OR_RET(cs::fs::mkdir(collection_dir));
  }

  // Generate UUID if not provided
  std::string uuid = request.uuid;
  std::string uuid_display;
  if (uuid.empty()) {
    uuid_display = "EMPTY";
  } else {
    uuid_display = uuid;
  }
  LOG(INFO) << "[DEBUG] Upsert " << request.collection
            << " - received uuid=" << uuid_display << ENDL;
  if (uuid.empty()) {
    uuid = cs::util::random::uuid::generate_uuid_v4();
    LOG(INFO) << "[DEBUG] Generated new uuid=" << uuid
              << ENDL;
  }

  // Validate payload_json is valid JSON
  SET_OR_RET(auto json_obj,
             ParseObject(request.payload_json));

  // Write file
  std::filesystem::path file_path =
      std::filesystem::path(collection_dir) /
      (uuid + ".json");
  OK_OR_RET(cs::fs::write(file_path.string(),
                          request.payload_json));

  // Build response
  UpsertResponse response;
  DatabaseRecord record;
  record.uuid = uuid;
  record.type = request.collection;
  record.payload_json = request.payload_json;
  response.record = record;

  return response;
}

}  // namespace cs::apps::database_service::rpc

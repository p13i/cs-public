// cs/apps/trycopilot.ai/api/test_db_store.gpt.cc
#include "cs/apps/trycopilot.ai/api/test_db_store.gpt.hh"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"
#include "cs/util/uuid.hh"

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
}  // namespace

namespace {  // helpers

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

bool CompareJsonObjects(const Object& a, const Object& b) {
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

std::string BuildQueryStringLocal(
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

}  // namespace

namespace cs::apps::trycopilotai::api::testing {

Result UpsertToStore(const UpsertRequest& request,
                     TestDbStore* store) {
  std::string uuid = request.uuid;
  if (uuid.empty()) {
    uuid = cs::util::random::uuid::generate_uuid_v4();
  }
  (*store)[request.collection][uuid] = request.payload_json;
  return Ok();
}

ResultOr<QueryResponse> QueryFromStore(
    const QueryRequest& request, const TestDbStore& store) {
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

}  // namespace cs::apps::trycopilotai::api::testing

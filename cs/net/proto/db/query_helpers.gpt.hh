// cs/net/proto/db/query_helpers.gpt.hh
#ifndef CS_NET_PROTO_DB_QUERY_HELPERS_GPT_HH
#define CS_NET_PROTO_DB_QUERY_HELPERS_GPT_HH

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/field_path_builder.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/result.hh"

namespace cs::net::proto::db {

template <typename FieldPathType>
cs::net::proto::database::Condition EQUALS(
    FieldPathType field_path,
    const std::string& value_str) {
  cs::net::proto::database::Condition cond;
  cond.field_path = GetFieldPathOrConvert(
      std::forward<FieldPathType>(field_path));
  LOG(INFO) << "[DEBUG] EQUALS condition - field_path='"
            << cond.field_path << "' value='" << value_str
            << "'" << ENDL;
  cond.operator_type = "eq";
  cond.value.type = "string";
  cond.value.string_value = value_str;
  return cond;
}

template <typename FieldPathType>
cs::net::proto::database::Condition EQUALS(
    FieldPathType field_path, int value_int) {
  cs::net::proto::database::Condition cond;
  cond.field_path = GetFieldPathOrConvert(
      std::forward<FieldPathType>(field_path));
  cond.operator_type = "eq";
  cond.value.type = "int";
  cond.value.int_value = value_int;
  return cond;
}

template <typename FieldPathType>
cs::net::proto::database::Condition EQUALS(
    FieldPathType field_path, bool value_bool) {
  cs::net::proto::database::Condition cond;
  cond.field_path = GetFieldPathOrConvert(
      std::forward<FieldPathType>(field_path));
  cond.operator_type = "eq";
  cond.value.type = "bool";
  cond.value.bool_value = value_bool;
  return cond;
}

template <typename FieldPathType>
cs::net::proto::database::Condition CONTAINS(
    FieldPathType field_path,
    const std::string& value_str) {
  cs::net::proto::database::Condition cond;
  cond.field_path = GetFieldPathOrConvert(
      std::forward<FieldPathType>(field_path));
  cond.operator_type = "contains";
  cond.value.type = "string";
  cond.value.string_value = value_str;
  return cond;
}

template <typename FieldPathType>
cs::net::proto::database::Condition NOT_CONTAINS(
    FieldPathType field_path,
    const std::string& value_str) {
  cs::net::proto::database::Condition cond;
  cond.field_path = GetFieldPathOrConvert(
      std::forward<FieldPathType>(field_path));
  cond.operator_type = "not_contains";
  cond.value.type = "string";
  cond.value.string_value = value_str;
  return cond;
}

// Build a human-readable query string from QueryStep vector
inline std::string BuildQueryString(
    const std::vector<cs::net::proto::database::QueryStep>&
        steps) {
  std::ostringstream oss;
  for (const auto& step : steps) {
    if (step.type == "WHERE") {
      oss << "WHERE(";
      // Parse predicate.condition to extract readable
      // condition if needed For now, show the serialized
      // condition
      oss << step.predicate.condition << ") ";
    } else if (step.type == "ORDER_BY") {
      oss << "ORDER_BY(" << step.order_by.field_path << " "
          << step.order_by.direction << ") ";
    } else if (step.type == "LIMIT") {
      oss << "LIMIT(" << step.limit << ") ";
    }
  }
  std::string result = oss.str();
  // Remove trailing space if any
  if (!result.empty() && result.back() == ' ') {
    result.pop_back();
  }
  return result;
}

// Helper function to insert/upsert a record with explicit
// UUID via database-service RPC. Client must be DI-fed
// (compile-time: reference required).
template <typename T>
cs::Result Upsert(
    const std::string& collection, const std::string& uuid,
    const T& value,
    cs::net::proto::db::IDatabaseClient& client) {
  std::string uuid_display;
  if (uuid.empty()) {
    uuid_display = "EMPTY";
  } else {
    uuid_display = uuid;
  }
  LOG(INFO) << "[DEBUG] Upsert called - collection="
            << collection << " uuid=" << uuid_display
            << ENDL;

  cs::net::proto::database::UpsertRequest request;
  request.collection = collection;
  request.uuid = uuid;
  request.payload_json = value.Serialize();

  LOG(INFO) << "[DEBUG] Upsert request - uuid="
            << request.uuid << " payload="
            << request.payload_json.substr(0, 100) << "..."
            << ENDL;

  return client.Upsert(request);
}

// Helper to extract UUID if present (SFINAE)
template <typename T>
auto ExtractUuidIfPresent(const T& value,
                          int) -> decltype(value.uuid,
                                           std::string()) {
  return value.uuid;
}

template <typename T>
std::string ExtractUuidIfPresent(const T& /*value*/, long) {
  return "";  // No .uuid field, database will generate
}

// Helper function to insert/upsert a record via
// database-service RPC. Client must be DI-fed.
template <typename T>
cs::Result Upsert(
    const std::string& collection, const T& value,
    cs::net::proto::db::IDatabaseClient& client) {
  return Upsert(collection, ExtractUuidIfPresent(value, 0),
                value, client);
}

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_QUERY_HELPERS_GPT_HH

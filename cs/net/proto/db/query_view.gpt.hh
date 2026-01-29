// cs/net/proto/db/query_view.gpt.hh
#ifndef CS_NET_PROTO_DB_QUERY_VIEW_GPT_HH
#define CS_NET_PROTO_DB_QUERY_VIEW_GPT_HH

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "cs/apps/database-service/rpc.gpt.hh"
#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/field_path_builder.gpt.hh"
#include "cs/net/proto/db/protos/database.proto.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/rpc/client.hh"
#include "cs/result.hh"

namespace cs::net::proto::db {

template <typename T, typename FieldType>
std::string GetFieldPath(FieldType T::*member_ptr);

template <typename T>
class QueryView {
  typedef std::vector<T> Ts;

 public:
  // Default constructor for ResultOr compatibility
  QueryView() : collection_(""), materialized_(false) {}

  // Constructor for remote query (takes collection name)
  // No errors at construction time - errors occur during
  // materialize() This is the primary constructor for the
  // new database service pattern
  explicit QueryView(const std::string& collection)
      : collection_(collection), materialized_(false) {}

  // Constructor for in-memory query (takes pre-loaded
  // values) Useful for testing or when you already have
  // values in memory. Values are pre-loaded but steps need
  // to be applied during materialize()
  explicit QueryView(const Ts& values)
      : collection_(""),
        materialized_(false),
        materialized_values_(values) {}

  std::string query_string() {
    return BuildQueryString(proto_steps_);
  }

  ResultOr<Ts> values() {
    OK_OR_RET(this->materialize());
    return materialized_values_;
  }

  QueryView<T> where(
      const cs::net::proto::database::Condition&
          condition) {
    cs::net::proto::database::Predicate predicate;
    predicate.condition = condition.Serialize(0);

    cs::net::proto::database::QueryStep step;
    step.type = "WHERE";
    step.predicate = predicate;
    proto_steps_.push_back(step);
    return *this;
  }

  template <typename FieldPathType>
  QueryView<T> order_by(
      FieldPathType field_path,
      const std::string& direction = "desc") {
    cs::net::proto::database::OrderBy order_by;
    order_by.field_path = GetFieldPathOrConvert(
        std::forward<FieldPathType>(field_path));
    order_by.direction = direction;

    cs::net::proto::database::QueryStep step;
    step.type = "ORDER_BY";
    step.order_by = order_by;
    proto_steps_.push_back(step);
    return *this;
  }

  QueryView<T> limit(unsigned int n) {
    cs::net::proto::database::QueryStep step;
    step.type = "LIMIT";
    step.limit = static_cast<int>(n);
    proto_steps_.push_back(step);
    return *this;
  }

  cs::ResultOr<T> first() {
    limit(1);
    OK_OR_RET(this->materialize());
    if (materialized_values_.empty()) {
      return TRACE(cs::Error("No values found"));
    }
    if (materialized_values_.size() > 1) {
      return TRACE(cs::Error("More than one value found"));
    }
    return materialized_values_[0];
  }

  cs::ResultOr<bool> any() {
    OK_OR_RET(this->materialize());
    return !materialized_values_.empty();
  }

  cs::Result materialize() {
    if (materialized_) {
      return cs::Ok();
    }

    // For in-memory queries (collection_ empty), apply
    // steps locally
    if (collection_.empty()) {
      // Apply WHERE filters
      std::vector<cs::net::proto::database::Condition>
          where_conditions;
      for (const auto& step : proto_steps_) {
        if (step.type == "WHERE") {
          cs::net::proto::database::Condition condition;
          SET_OR_RET(
              condition,
              condition.Parse(step.predicate.condition));
          where_conditions.push_back(condition);
        }
      }

      // Filter records
      if (!where_conditions.empty()) {
        Ts filtered;
        for (const auto& value : materialized_values_) {
          std::string json = value.Serialize(0);
          SET_OR_RET(
              auto json_obj,
              cs::net::json::parsers::ParseObject(json));

          bool matches = true;
          for (const auto& condition : where_conditions) {
            SET_OR_RET(bool matches_condition,
                       EvaluateConditionLocal(json_obj,
                                              condition));
            if (!matches_condition) {
              matches = false;
              break;
            }
          }

          if (matches) {
            filtered.push_back(value);
          }
        }
        materialized_values_ = filtered;
      }

      // Apply ORDER BY
      cs::net::proto::database::OrderBy* order_by_step =
          nullptr;
      for (const auto& step : proto_steps_) {
        if (step.type == "ORDER_BY") {
          order_by_step = const_cast<
              cs::net::proto::database::OrderBy*>(
              &step.order_by);
          break;
        }
      }

      if (order_by_step != nullptr &&
          !materialized_values_.empty()) {
        std::sort(materialized_values_.begin(),
                  materialized_values_.end(),
                  [order_by_step](const T& a, const T& b) {
                    std::string json_a = a.Serialize(0);
                    std::string json_b = b.Serialize(0);
                    auto obj_a_or =
                        cs::net::json::parsers::ParseObject(
                            json_a);
                    auto obj_b_or =
                        cs::net::json::parsers::ParseObject(
                            json_b);
                    if (!obj_a_or.ok() || !obj_b_or.ok()) {
                      return false;
                    }

                    auto val_a_or = ExtractFieldValueLocal(
                        obj_a_or.value(),
                        order_by_step->field_path);
                    auto val_b_or = ExtractFieldValueLocal(
                        obj_b_or.value(),
                        order_by_step->field_path);
                    if (!val_a_or.ok() || !val_b_or.ok()) {
                      return false;
                    }

                    auto val_a = val_a_or.value();
                    auto val_b = val_b_or.value();

                    bool a_less = false;
                    if (val_a.is(std::string()) &&
                        val_b.is(std::string())) {
                      a_less = val_a.as(std::string()) <
                               val_b.as(std::string());
                    } else if (val_a.is(int()) &&
                               val_b.is(int())) {
                      a_less =
                          val_a.as(int()) < val_b.as(int());
                    } else if (val_a.is(float()) &&
                               val_b.is(float())) {
                      a_less = val_a.as(float()) <
                               val_b.as(float());
                    }

                    return order_by_step->direction == "asc"
                               ? a_less
                               : !a_less;
                  });
      }

      // Apply LIMIT
      int limit_value = -1;
      for (const auto& step : proto_steps_) {
        if (step.type == "LIMIT") {
          limit_value = step.limit;
          break;
        }
      }

      if (limit_value > 0 &&
          materialized_values_.size() >
              static_cast<size_t>(limit_value)) {
        materialized_values_.resize(limit_value);
      }

      materialized_ = true;
      return cs::Ok();
    }

    // Remote query - call database service
    cs::net::proto::database::QueryRequest request;
    request.collection = collection_;
    request.steps = proto_steps_;

    // Note: Requires API base class to have
    // RequestType/ResponseType typedefs. See Critical
    // Self-Analysis Issue #11 for required fix to api.hh.
    cs::net::rpc::RPCClient<
        cs::apps::database_service::api::QueryAPI>
        client("http://database-service:8080");
    SET_OR_RET(auto response,
               client.Call("/rpc/query/", request));

    materialized_values_.clear();
    for (const auto& record : response.records) {
      SET_OR_RET(auto parsed,
                 T().Parse(record.payload_json));
      materialized_values_.push_back(parsed);
    }

    materialized_ = true;
    return cs::Ok();
  }

 private:
  static cs::ResultOr<bool> EvaluateConditionLocal(
      const cs::net::json::Object& json_obj,
      const cs::net::proto::database::Condition&
          condition) {
    SET_OR_RET(auto field_value,
               ExtractFieldValueLocal(
                   json_obj, condition.field_path));

    return CompareValuesLocal(field_value,
                              condition.operator_type,
                              condition.value);
  }

  static cs::ResultOr<cs::net::json::Object>
  ExtractFieldValueLocal(const cs::net::json::Object& obj,
                         const std::string& field_path) {
    auto parts = SplitFieldPathLocal(field_path);
    cs::net::json::Object current = obj;

    for (const auto& part : parts) {
      SET_OR_RET(current, current.get(part));
    }

    return current;
  }

  static std::vector<std::string> SplitFieldPathLocal(
      const std::string& path) {
    std::vector<std::string> parts;
    std::istringstream iss(path);
    std::string part;
    while (std::getline(iss, part, '.')) {
      parts.push_back(part);
    }
    return parts;
  }

  static cs::ResultOr<bool> CompareValuesLocal(
      const cs::net::json::Object& field_value,
      const std::string& operator_type,
      const cs::net::proto::database::Value& expected) {
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
        return field_value.as(bool()) ==
               expected.bool_value;
      }
    } else if (operator_type == "contains") {
      if (expected.type == "string") {
        std::string field_str =
            field_value.as(std::string());
        return field_str.find(expected.string_value) !=
               std::string::npos;
      }
    } else if (operator_type == "not_contains") {
      if (expected.type == "string") {
        std::string field_str =
            field_value.as(std::string());
        return field_str.find(expected.string_value) ==
               std::string::npos;
      }
    }

    return TRACE(cs::Error("Unsupported operator: " +
                           operator_type));
  }

 private:
  std::vector<cs::net::proto::database::QueryStep>
      proto_steps_;
  std::string collection_;
  bool materialized_ = false;
  Ts materialized_values_;
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_QUERY_VIEW_GPT_HH

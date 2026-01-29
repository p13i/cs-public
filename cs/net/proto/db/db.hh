// cs/net/proto/db/db.hh
#ifndef CS_NET_PROTO_DB_DB_HH
#define CS_NET_PROTO_DB_DB_HH

#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/field_path_builder.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"

// Legacy macros removed - use EQUALS/CONTAINS from
// query_helpers.gpt.hh instead

// Note: F() macro syntax is not used. Use direct member
// pointers
// (&Type::Field) or FieldPathBuilder chaining (log >>
// request >> path)

// Enhanced DESCENDING macro - for new QueryView API
// Handles both member pointers and FieldPathBuilder
#define DESCENDING(field_path)                           \
  cs::net::proto::db::GetFieldPathOrConvert(field_path), \
      std::string("desc")

// Enhanced ASCENDING macro - for new QueryView API
// Handles both member pointers and FieldPathBuilder
#define ASCENDING(field_path)                            \
  cs::net::proto::db::GetFieldPathOrConvert(field_path), \
      std::string("asc")

namespace cs::net::proto::db {

// QueryView is now in query_view.gpt.hh - using it from
// there

class LocalJsonDatabase {
 public:
  LocalJsonDatabase() = default;
  LocalJsonDatabase(std::string data_abs_dir)
      : data_abs_dir_(data_abs_dir) {}

  ::cs::ResultOr<::cs::net::json::Object> get(
      std::string path);

  ::cs::Result set(std::string path,
                   ::cs::net::json::Object obj);

 private:
  std::string data_abs_dir_;
};

}  // namespace cs::net::proto::db

#endif  // CS_NET_PROTO_DB_DB_HH

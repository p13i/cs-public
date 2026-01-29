// cs/net/proto/db/db.cc
#include "cs/net/proto/db/db.hh"

#include <sstream>
#include <string>

#include "cs/fs/fs.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
}  // namespace

namespace {
struct ResourceName {
  std::string file_path;
};

ResultOr<ResourceName> ParseResourceName(std::string str) {
  if (str.empty()) {
    return TRACE(
        InvalidArgument("Empty resource name string"));
  }
  unsigned int n = str.size();
  if (n >= 64) {
    return TRACE(InvalidArgument(FMT(
        "Resource name must be less than 64 chars, not %u",
        n)));
  }
  unsigned int i;
  for (i = 0; i < n; i++) {
    char c = str[i];
    if ('a' <= c && c <= 'z') {
      continue;
    }
    if ('A' <= c && c <= 'Z') {
      continue;
    }
    if ('0' <= c && c <= '9') {
      continue;
    }
    if ('-' == c) {
      continue;
    }
    return TRACE(
        InvalidArgument(FMT("str[%u] = %c, invalid. must "
                            "be a-z, A-Z, 0-9, or -.",
                            i, c)));
  }

  ResourceName name;
  name.file_path = str + ".json";
  return name;
}
}  // namespace

namespace cs::net::proto::db {

ResultOr<Object> LocalJsonDatabase::get(
    std::string resource_name) {
  SET_OR_RET(ResourceName name,
             ParseResourceName(resource_name));
  SET_OR_RET(std::string json,
             cs::fs::read(cs::fs::Join(data_abs_dir_,
                                       name.file_path)));

  Object node;
  SET_OR_RET(node, ParseObject(json));
  return node;
}

Result LocalJsonDatabase::set(std::string path,
                              Object obj) {
  OK_OR_RET(cs::fs::mkdir(data_abs_dir_));
  std::string enclosing_folder_path =
      path.substr(0, path.find_last_of('/'));
  OK_OR_RET(cs::fs::mkdir(
      cs::fs::Join(data_abs_dir_, enclosing_folder_path)));
  return cs::fs::write(cs::fs::Join(data_abs_dir_, path),
                       obj.str());
}

}  // namespace cs::net::proto::db

// cs/parsers/arg_parser.gpt.hh
#ifndef CS_PARSERS_ARG_PARSER_GPT_HH
#define CS_PARSERS_ARG_PARSER_GPT_HH

#include <sstream>
#include <string>
#include <vector>

#include "cs/net/json/object.hh"
#include "cs/net/json/serialize.hh"
#include "cs/net/proto/proto.hh"
#include "cs/net/proto/protos/meta.proto.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"

namespace cs::parsers {

// Parses command-line arguments and constructs a proto
// instance. Arguments should be in the format
// --field=value. Boolean flags must be explicitly set:
// --flag=true or --flag=false. Fields not specified will
// use their default values.
template <typename T>
cs::ResultOr<T> ParseArgs(std::vector<std::string> argv) {
  // Skip argv[0] (program name).
  if (argv.empty()) {
    return cs::InvalidArgument("argv cannot be empty");
  }

  // Get metadata about the proto fields.
  T instance{};
  auto meta = instance.Meta();

  // Build a map of field names to values from argv.
  cs::net::json::Object::KVMap kv_map;

  for (size_t i = 1; i < argv.size(); ++i) {
    const std::string& arg = argv[i];

    // Must start with --.
    if (arg.size() < 3 || arg[0] != '-' || arg[1] != '-') {
      return cs::InvalidArgument(
          "Argument must start with --: " + arg);
    }

    // Find the = separator.
    size_t eq_pos = arg.find('=');
    if (eq_pos == std::string::npos) {
      return cs::InvalidArgument(
          "Argument must be in format --field=value: " +
          arg);
    }

    std::string field_name = arg.substr(2, eq_pos - 2);
    std::string field_value = arg.substr(eq_pos + 1);

    // Find the field in metadata.
    bool found = false;
    for (const auto& field_meta : meta.fields) {
      if (field_meta.name == field_name) {
        found = true;

        // Parse the value based on the field type.
        std::string type_str = field_meta.type;
        cs::net::json::Object value_obj;

        if (type_str == "bool") {
          if (field_value == "true") {
            value_obj =
                cs::net::json::Object::NewBool(true);
          } else if (field_value == "false") {
            value_obj =
                cs::net::json::Object::NewBool(false);
          } else {
            return cs::InvalidArgument(
                "Boolean field must be 'true' or 'false': "
                "--" +
                field_name + "=" + field_value);
          }
        } else if (type_str == "std::string") {
          value_obj =
              cs::net::json::Object::NewString(field_value);
        } else if (type_str == "int") {
          SET_OR_RET(int int_value,
                     cs::parsers::ParseInt(field_value));
          value_obj =
              cs::net::json::Object::NewInt(int_value);
        } else if (type_str == "float") {
          SET_OR_RET(float float_value,
                     cs::parsers::ParseFloat(field_value));
          value_obj =
              cs::net::json::Object::NewFloat(float_value);
        } else {
          // For other types (vectors, maps, nested protos),
          // treat as string and let the proto parser handle
          // it.
          value_obj =
              cs::net::json::Object::NewString(field_value);
        }

        kv_map[field_name] = value_obj;
        break;
      }
    }

    if (!found) {
      return cs::InvalidArgument(
          "Unknown field: --" + field_name +
          " (not found in proto metadata)");
    }
  }

  // Convert the map to a JSON string.
  cs::net::json::Object json_obj(kv_map);
  std::ostringstream oss;
  cs::net::json::SerializeObject(oss, json_obj);
  std::string json_str = oss.str();

  // Parse the JSON string to construct the proto instance.
  return instance.Parse(json_str);
}

}  // namespace cs::parsers

#endif  // CS_PARSERS_ARG_PARSER_GPT_HH

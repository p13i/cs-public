// cs/apps/service-registry/service_registry_app.gpt.cc
#include "cs/apps/service-registry/service_registry_app.gpt.hh"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "cs/apps/service-registry/protos/docker.proto.hh"
#include "cs/apps/service-registry/protos/gencode/docker.proto.hh"
#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/log.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"
#include "cs/util/random.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::service_registry::protos::ServiceEntry;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::Upsert;
}  // namespace

namespace cs::apps::service_registry {

ResultOr<std::string> RunCommand(const std::string& cmd) {
  std::array<char, 256> buffer{};
  std::string output;
  std::unique_ptr<FILE, int (*)(FILE*)> pipe(
      popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    return Error("Failed to open pipe for command.");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get())) {
    output.append(buffer.data());
  }
  int rc = pclose(pipe.release());
  if (rc != 0) {
    return Error(FMT("Command failed with exit code %d: %s",
                     rc, cmd.c_str()));
  }
  return output;
}

std::string ExtractIPAddress(
    const std::string& ip_with_cidr) {
  size_t pos = ip_with_cidr.find('/');
  if (pos != std::string::npos) {
    return ip_with_cidr.substr(0, pos);
  }
  return ip_with_cidr;
}

ResultOr<std::string> GetContainerPort(
    const std::string& container_id) {
  std::string cmd =
      FMT("docker inspect %s", container_id.c_str());
  SET_OR_RET(std::string json_output, RunCommand(cmd));
  SET_OR_RET(Object parsed, ParseObject(json_output));

  if (!parsed.has_index(0)) {
    return Error("docker inspect returned empty array");
  }

  SET_OR_RET(Object container_obj, parsed.get(0));

  if (container_obj.has_key("NetworkSettings")) {
    SET_OR_RET(Object network_settings_obj,
               container_obj.get("NetworkSettings"));
    if (network_settings_obj.has_key("Ports")) {
      SET_OR_RET(Object ports_obj,
                 network_settings_obj.get("Ports"));
      if (ports_obj.is(Object::KVMap{})) {
        Object::KVMap ports_map =
            ports_obj.as(Object::KVMap{});
        if (!ports_map.empty()) {
          const auto& first_port = *ports_map.begin();
          std::string port_key = first_port.first;
          size_t slash_pos = port_key.find('/');
          if (slash_pos != std::string::npos) {
            std::string port =
                port_key.substr(0, slash_pos);
            Object port_val = first_port.second;
            if (port_val.type() != Type::kNull &&
                port_val.is(Object::Array{})) {
              Object::Array bindings =
                  port_val.as(Object::Array{});
              if (!bindings.empty()) {
                Object binding_obj = bindings[0];
                if (binding_obj.has_key("HostPort")) {
                  SET_OR_RET(Object host_port_obj,
                             binding_obj.get("HostPort"));
                  if (host_port_obj.is(std::string{})) {
                    std::string host_port =
                        host_port_obj.as(std::string{});
                    if (!host_port.empty()) {
                      return host_port;
                    }
                  }
                }
              }
            }
            return port;
          }
        }
      }
    }
  }

  if (container_obj.has_key("Config")) {
    SET_OR_RET(Object config_obj,
               container_obj.get("Config"));
    if (config_obj.has_key("ExposedPorts")) {
      SET_OR_RET(Object exposed_ports_obj,
                 config_obj.get("ExposedPorts"));
      if (exposed_ports_obj.is(Object::KVMap{})) {
        Object::KVMap exposed_map =
            exposed_ports_obj.as(Object::KVMap{});
        if (!exposed_map.empty()) {
          const auto& first_exposed = *exposed_map.begin();
          std::string port_key = first_exposed.first;
          size_t slash_pos = port_key.find('/');
          if (slash_pos != std::string::npos) {
            return port_key.substr(0, slash_pos);
          }
        }
      }
    }
  }

  return Error(FMT("No port found for container: %s",
                   container_id.c_str()));
}

extern ResultOr<ServiceRegistry> DiscoverServices(
    const std::string& network_name,
    const std::string& prefix);

void StartDiscoveryThread(const std::string& network_name,
                          const std::string& prefix,
                          IDatabaseClient& db) {
  std::thread([network_name, prefix, &db]() {
    while (true) {
      auto discovered =
          DiscoverServices(network_name, prefix);
      if (discovered.ok()) {
        ServiceRegistry registry = discovered.value();
        ServiceRegistry registry_to_save = registry;
        registry_to_save.uuid = "services";
        auto result = Upsert("services", "services",
                             registry_to_save, db);
        if (!result.ok()) {
          LOG(WARNING) << FMT("Failed to save registry to "
                              "database: %s",
                              result.message().c_str())
                       << ENDL;
        } else {
          LOG(DEBUG) << "Saved registry to database"
                     << ENDL;
        }
      } else {
        LOG(WARNING) << FMT("Discovery failed: %s",
                            discovered.message().c_str())
                     << ENDL;
      }
      std::this_thread::sleep_for(std::chrono::seconds(61));
    }
  }).detach();
}

}  // namespace cs::apps::service_registry

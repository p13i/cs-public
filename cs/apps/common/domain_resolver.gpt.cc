// cs/apps/common/domain_resolver.gpt.cc
#include "cs/apps/common/domain_resolver.gpt.hh"

#include <map>
#include <string>

#include "cs/apps/common/routing_embedded_gen_embed.h"
#include "cs/net/http/request.hh"
#include "cs/net/load/balancer/protos/config.proto.hh"
#include "cs/result.hh"
#include "cs/util/embedded_files.hh"

namespace cs::apps::common {

namespace {  // use_usings
using ::cs::Error;
using ::cs::ResultOr;
using ::cs::net::http::Request;
using ::cs::net::load::balancer::protos::Config;
using ::cs::util::EmbeddedFileEntry;
using ::cs::util::FindEmbedded;
}  // namespace

ResultOr<std::string> DetermineEnvironment(
    const Request& request) {
  SET_OR_RET(std::string host, request.NormalizedHost());
  if (host.size() >= 4 && host.compare(0, 4, "dev.") == 0) {
    return std::string("dev");
  }
  return std::string("prod");
}

ResultOr<std::string> GetDomainForService(
    const std::string& service_name,
    const Request& request) {
  SET_OR_RET(std::string json_content,
             FindEmbedded(
                 "routing.json",
                 reinterpret_cast<const EmbeddedFileEntry*>(
                     kEmbeddedFiles_routing),
                 kEmbeddedFilesCount_routing));
  SET_OR_RET(Config config, Config().Parse(json_content));
  SET_OR_RET(std::string env,
             DetermineEnvironment(request));
  const std::map<std::string, std::string>& routes =
      [&config, &env]()
      -> const std::map<std::string, std::string>& {
    if (env == "dev") {
      return config.dev.routes;
    }
    return config.prod.routes;
  }();
  for (const auto& [domain, svc] : routes) {
    if (svc == service_name) {
      return domain;
    }
  }
  return TRACE(
      Error("Service not found in routing config: " +
            service_name));
}

}  // namespace cs::apps::common

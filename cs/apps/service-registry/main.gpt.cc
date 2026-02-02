// cs/apps/service-registry/main.gpt.cc
#include <array>
#include <cstdlib>
#include <string>
#include <vector>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/service-registry/protos/config.proto.hh"
#include "cs/apps/service-registry/protos/docker.proto.hh"
#include "cs/apps/service-registry/protos/gencode/docker.proto.hh"
#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/apps/service-registry/rpc.gpt.hh"
#include "cs/apps/service-registry/service_registry_app.gpt.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/fmt.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::common::GetHealth;
using ::cs::apps::service_registry::ExtractIPAddress;
using ::cs::apps::service_registry::RunCommand;
using ::cs::apps::service_registry::StartDiscoveryThread;
using ::cs::apps::service_registry::protos::Config;
using ::cs::apps::service_registry::protos::DockerConfig;
using ::cs::apps::service_registry::protos::ServiceEntry;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::apps::service_registry::rpc::LookupServiceRPC;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::db::DatabaseBaseUrl;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

namespace cs::apps::service_registry {

ResultOr<ServiceRegistry> DiscoverServices(
    const std::string& network_name,
    const std::string& prefix) {
  std::string cmd = FMT("docker network inspect %s",
                        network_name.c_str());
  SET_OR_RET(std::string json_output, RunCommand(cmd));
  SET_OR_RET(auto parsed, ParseObject(json_output));

  if (!parsed.has_index(0)) {
    return TRACE(Error(
        "docker network inspect returned empty array"));
  }

  SET_OR_RET(Object network_obj, parsed.get(0));

  // Extract just the Containers field as JSON string
  // (matching test code)
  SET_OR_RET(Object containers_obj,
             network_obj.get("Containers"));
  std::string containers_json = containers_obj.str();

  // Create a minimal network inspect element JSON with just
  // Containers
  std::string network_json =
      R"({"Containers":)" + containers_json + "}";

  SET_OR_RET(auto network_inspect,
             cs::apps::service_registry::protos::
                 DockerNetworkInspectElement()
                     .Parse(network_json));

  ServiceRegistry registry;
  registry.network_name = network_name;
  registry.prefix = prefix;
  registry.last_updated = NowAsISO8601TimeUTC();

  for (const auto& [container_id, container] :
       network_inspect.Containers) {
    std::string container_name = container.Name;
    std::string ip_address = container.IPv4Address;

    std::string service_name = container_name;
    std::string prefix_with_dash =
        FMT("%s-", prefix.c_str());
    if (container_name.find(prefix_with_dash) == 0) {
      service_name =
          container_name.substr(prefix.length() + 1);
    }

    std::string ip = ExtractIPAddress(ip_address);

    // auto port_result = GetContainerPort(container_id);
    // if (!port_result.ok()) {
    //   LOG(DEBUG) << FMT("Skipping container %s: %s",
    //                     container_name.c_str(),
    //                     port_result.message().c_str())
    //              << ENDL;
    //   continue;
    // }
    int port = 8080;

    ServiceEntry entry;
    entry.name = service_name;
    entry.container_name = container_name;
    entry.ip_address = ip;
    // Use container name for host to match Docker DNS
    // entries (includes compose project prefix like
    // cs-dev-...).
    entry.host = container_name;
    entry.port = port;
    entry.last_updated = NowAsISO8601TimeUTC();

    registry.services.push_back(entry);
  }

  LOG(INFO) << FMT("Discovered %zu services in network %s",
                   registry.services.size(),
                   network_name.c_str())
            << ENDL;
  return registry;
}

}  // namespace cs::apps::service_registry

Result RunServiceRegistry(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  const char* compose_project =
      std::getenv("COMPOSE_PROJECT_NAME");
  std::string prefix;
  if (compose_project) {
    prefix = compose_project;
  } else {
    prefix = "cs";
  }
  std::string network_name =
      FMT("%s_default", prefix.c_str());

  using AppContext =
      Context<DatabaseBaseUrl, IDatabaseClient>;
  auto app_ctx =
      ContextBuilder<AppContext>()
          .bind<DatabaseBaseUrl>()
          .with(std::string("http://database-service:8080"))
          .bind<IDatabaseClient>()
          .from([](AppContext& ctx) {
            return std::make_shared<DatabaseClientImpl>(
                ctx.Get<DatabaseBaseUrl>()->value());
          })
          .build();

  StartDiscoveryThread(network_name, prefix,
                       *app_ctx.Get<IDatabaseClient>());

  WebApp<AppContext> app(app_ctx);

  ADD_ROUTE(app, "POST", "/rpc/lookup-service/",
            LookupServiceRPC::HttpHandler<AppContext>);

  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunServiceRegistry);
}

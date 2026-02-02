// cs/net/load/balancer/server.gpt.cc
#include "cs/net/load/balancer/server.hh"

#include <algorithm>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/load-balancer/routing_embedded_gen_embed.h"
#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/log.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/load/balancer/balancer.hh"
#include "cs/net/load/balancer/protos/config.proto.hh"
#include "cs/net/load/balancer/protos/downstream.proto.hh"
#include "cs/net/load/balancer/proxy_environment.gpt.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/embedded_files.hh"
#include "cs/util/fmt.hh"
#include "cs/util/string.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Fetch;
using ::cs::FetchResponse;
using ::cs::InvalidArgument;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::common::GetHealth;
using ::cs::apps::service_registry::protos::ServiceEntry;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::HTTP_502_BAD_GATEWAY;
using ::cs::net::http::HTTP_503_SERVICE_UNAVAILABLE;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::load::balancer::ProxyEnvironment;
using ::cs::net::load::balancer::protos::Config;
using ::cs::net::load::balancer::protos::DownstreamClient;
using ::cs::net::load::balancer::protos::GetLoadResponse;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::util::EmbeddedFileEntry;
using ::cs::util::FindEmbedded;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::string::ToLowercase;
}  // namespace

using AppContext = ::cs::util::di::Context<
    ::cs::net::load::balancer::ProxyEnvironment,
    ::cs::net::proto::db::DatabaseBaseUrl,
    ::cs::net::proto::db::IDatabaseClient>;

namespace cs::net::load::balancer {

// In-memory cache for downstream clients
static std::vector<DownstreamClient> cached_clients_;
static std::mutex cache_mutex_;
static std::atomic<bool> should_stop_(false);
static std::thread discovery_thread_;
static std::atomic<bool> thread_started_(false);

Response FallbackHtmlResponse() {
  namespace dom = cs::net::html::dom;
  return HtmlResponse(dom::html(
      dom::head(dom::meta({{"charset", "utf-8"}}),
                dom::title("trycopilot.ai")),
      dom::body(
          dom::h1("trycopilot.ai"),
          dom::p(
              "Temporarily unavailable while discovering "
              "downstream services."))));
}

ResultOr<std::map<std::string, std::string>>
LoadRoutingConfig(const std::string& environment) {
  SET_OR_RET(std::string json_content,
             FindEmbedded(
                 "routing.json",
                 reinterpret_cast<const EmbeddedFileEntry*>(
                     kEmbeddedFiles_routing),
                 kEmbeddedFilesCount_routing));
  SET_OR_RET(Config config, Config().Parse(json_content));
  if (environment == "prod") {
    return config.prod.routes;
  } else if (environment == "dev") {
    return config.dev.routes;
  }
  return TRACE(InvalidArgument(
      "Invalid environment (expected prod or dev): " +
      environment));
}

Response ProxyHandler(Request request, AppContext& ctx) {
  const std::string& environment =
      ctx.Get<ProxyEnvironment>()->value();
  auto db = ctx.Get<IDatabaseClient>();
  SET_OR_RET(auto all_clients,
             DiscoverDownstreamClients(environment, db));

  std::string host_header;
  for (const auto& [name, value] : request.headers()) {
    if (ToLowercase(name) == "host") {
      host_header = value;
      break;
    }
  }
  std::string normalized_host = ToLowercase(host_header);
  auto colon = normalized_host.find(':');
  if (colon != std::string::npos) {
    normalized_host = normalized_host.substr(0, colon);
  }

  std::string target_service;
  SET_OR_RET(auto routes, LoadRoutingConfig(environment));
  LOG(DEBUG) << "Routing confiasdfg: "
             << STREAM_STRING_MAP(routes) << ENDL;
  auto it = routes.find(normalized_host);
  if (it == routes.end()) {
    return Response(
        HTTP_404_NOT_FOUND, kContentTypeTextPlain,
        "No routing match for host: " + normalized_host);
  }
  target_service = it->second;

  LOG(DEBUG) << "Proxyinasdfasdfsdaasdfasdfg host="
             << normalized_host
             << " to service=" << target_service << ENDL;

  std::vector<DownstreamClient> downstreams;
  for (const auto& client : all_clients) {
    if (client.service_name == target_service) {
      downstreams.push_back(client);
    }
  }

  if (downstreams.empty()) {
    return Response(
        HTTP_503_SERVICE_UNAVAILABLE, kContentTypeTextPlain,
        "No downstream clients available for service: " +
            target_service);
  }

  // Pick the least loaded client
  SET_OR_RET(auto picked, PickLeastLoaded(downstreams));

  auto result = ProxyToDownstream(request, picked);

  return result;
}

Result Server(std::string host, int port,
              std::string environment) {
  const std::string database_base_url =
      "http://database-service:8080";
  auto app_ctx =
      ContextBuilder<AppContext>()
          .bind<ProxyEnvironment>()
          .with(environment)
          .bind<::cs::net::proto::db::DatabaseBaseUrl>()
          .with(database_base_url)
          .bind<::cs::net::proto::db::IDatabaseClient>()
          .from([](AppContext& ctx) {
            return std::make_shared<
                ::cs::net::proto::db::DatabaseClientImpl>(
                ctx.Get<::cs::net::proto::db::
                            DatabaseBaseUrl>()
                    ->value());
          })
          .build();
  auto db = app_ctx.Get<IDatabaseClient>();
  StartDiscoveryThread(environment, db);

  WebApp<AppContext> app(app_ctx);

  ADD_ROUTE(app, "GET", "/health/", GetHealth);
  ADD_ROUTE(app, "GET", "/*", ProxyHandler);
  ADD_ROUTE(app, "POST", "/*", ProxyHandler);

  return app.RunServer(host, port);  // LCOV_EXCL_LINE
}  // LCOV_EXCL_LINE

namespace {}  // namespace

ResultOr<std::vector<DownstreamClient>>
DiscoverDownstreamClients(
    const std::string& environment,
    std::shared_ptr<IDatabaseClient> db_client) {
  {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (!cached_clients_.empty()) {
      return cached_clients_;
    }
  }

  std::vector<DownstreamClient> clients;

  // Load routing config to get list of service names to
  // discover
  SET_OR_RET(auto routes, LoadRoutingConfig(environment));
  std::set<std::string> target_service_names;
  for (const auto& [domain, service_name] : routes) {
    target_service_names.insert(service_name);
  }

  QueryView<ServiceRegistry> query("services", db_client);
  SET_OR_RET(auto registry, query.first());

  for (const auto& entry : registry.services) {
    LOG(DEBUG) << "Registry entry name=" << entry.name
               << " container=" << entry.container_name
               << ENDL;

    // Check if entry matches any service name from
    // routing config
    auto it = std::find_if(
        target_service_names.begin(),
        target_service_names.end(),
        [&entry](const std::string& service_name) {
          return entry.name == service_name ||
                 entry.name.find(FMT(
                     "%s-", service_name.c_str())) == 0 ||
                 entry.container_name.find(service_name) !=
                     std::string::npos;
        });
    if (it == target_service_names.end()) {
      continue;
    }
    std::string matched_service_name = *it;

    if (entry.container_name.find("run-") !=
        std::string::npos) {
      LOG(WARNING) << "Skipping transient container "
                   << entry.container_name << ENDL;
      continue;
    }

    int port = entry.port;

    auto health_response_or = FetchResponse(
        FMT("http://%s:%d/health/",
            entry.container_name.c_str(), port),
        "GET");
    if (!health_response_or.ok()) {
      LOG(WARNING)
          << FMT("Skipping %s:%d - health fetch "
                 "failed: %s",
                 entry.container_name.c_str(), port,
                 health_response_or.message().c_str())
          << ENDL;
      continue;
    }
    if (health_response_or.value().status().code !=
        HTTP_200_OK.code) {
      LOG(DEBUG)
          << FMT("Skipping %s:%d - health check failed",
                 entry.container_name.c_str(), port)
          << ENDL;
      continue;
    }

    DownstreamClient client;
    client.host = entry.container_name;
    client.port = port;
    client.load = 0.0f;

    auto load_result =
        Fetch(FMT("http://%s:%d/load/",
                  entry.container_name.c_str(), port),
              "GET");
    if (load_result.ok()) {
      auto response_proto =
          GetLoadResponse().Parse(load_result.value());
      if (response_proto.ok()) {
        client.load = response_proto.value().load;
      }
    }

    client.uuid =
        FMT("%s:%d", client.host.c_str(), client.port);
    client.service_name = matched_service_name;
    clients.push_back(client);

    LOG(DEBUG) << FMT("Discovered downstream client from "
                      "service-registry: "
                      "%s:%d (load: %.2f, service: %s, "
                      "container: %s)",
                      entry.container_name.c_str(), port,
                      client.load,
                      client.service_name.c_str(),
                      entry.container_name.c_str())
               << ENDL;
  }

  if (clients.empty()) {
    LOG(WARNING)
        << "No matching services found in service-registry"
        << ENDL;
  }

  return clients;
}

void RefreshDiscoveryLoop(
    std::string environment,
    std::shared_ptr<IDatabaseClient> db_client) {
  while (!should_stop_) {
    {
      std::lock_guard<std::mutex> lock(cache_mutex_);
      cached_clients_.clear();
    }
    auto discovered =
        DiscoverDownstreamClients(environment, db_client);
    if (discovered.ok()) {
      {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        cached_clients_ = discovered.value();
      }
      // LCOV_EXCL_START
    } else {
      LOG(WARNING)
          << "Discovery failed in background thread: "
          << discovered.message() << ENDL;
    }
    // LCOV_EXCL_STOP

    std::this_thread::sleep_for(std::chrono::seconds(59));
  }
}

void StartDiscoveryThread(
    std::string environment,
    std::shared_ptr<IDatabaseClient> db_client) {
  // Check if thread already started to avoid duplicates
  bool expected = false;
  if (!thread_started_.compare_exchange_strong(expected,
                                               true)) {
    return;  // Thread already started
  }

  should_stop_ = false;
  discovery_thread_ = std::thread(RefreshDiscoveryLoop,
                                  environment, db_client);
  discovery_thread_.detach();
}

}  // namespace cs::net::load::balancer

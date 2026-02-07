// cs/apps/cite.pub/main.cc
#include <string>
#include <vector>

#include "cs/apps/cite.pub/protos/config.proto.hh"
#include "cs/apps/cite.pub/protos/get_cached_page_request.gpt.proto.hh"
#include "cs/apps/cite.pub/protos/lookup_mapping.proto.hh"
#include "cs/apps/cite.pub/protos/save_resource_form.gpt.proto.hh"
#include "cs/apps/common/health_endpoint.hh"
#include "cs/net/html/bootstrap/dsl.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/net/proto/form/proto_form.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Fetch;
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::apps::citepub::protos::Config;
using ::cs::apps::citepub::protos::GetCachedPageRequest;
using ::cs::apps::citepub::protos::LookupMapping;
using ::cs::apps::citepub::protos::SaveResourceForm;
using ::cs::apps::common::GetHealth;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::net::proto::form::GenerateProtoForm;
using ::cs::net::rpc::ExtractProtoFromRequest;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::util::time::NowAsISO8601TimeUTC;
}  // namespace

// Generate HTML content for a URL.
ResultOr<std::string> MakeSingleFileHTML(std::string url) {
  return Fetch(url, "GET");
}

using AppContext = Context<IDatabaseClient>;

Response GetIndexPage(Request request, AppContext&) {
  using namespace cs::net::html::dom;
  namespace bs = ::cs::net::html::bootstrap;

  bs::NavbarConfig navbar_config;
  navbar_config.brand_href = "/";
  navbar_config.brand_label = "cite.pub";
  navbar_config.expand_breakpoint = "lg";
  navbar_config.collapse_id = "navbarMain";
  navbar_config.active_path = "/";

  return HtmlResponse(html(
      head(meta({{"charset", "utf-8"}}),
           meta({{"name", "viewport"},
                 {"content",
                  "width=device-width, initial-scale=1"}}),
           title("cite.pub - Save Web Pages"),
           bs::StylesheetLink()),
      body(
          bs::Navbar(navbar_config),
          bs::Container(
              h1("cite.pub") +
              p("Save any web page as a single HTML file") +
              GenerateProtoForm<SaveResourceForm>(
                  "/save/")),
          bs::ScriptTag())));
}

// POST /save/ - Save a web resource (form submission).
Response SaveResource(Request request, AppContext& ctx) {
  SET_OR_RET(
      SaveResourceForm form,
      ExtractProtoFromRequest<SaveResourceForm>(request));

  // Generate HTML content for the page
  SET_OR_RET(std::string html,
             MakeSingleFileHTML(form.url));

  // Generate UUID and short code (first 8 characters)
  std::string uuid = generate_uuid_v4();
  std::string short_code = uuid.substr(0, 8);

  // Create lookup mapping
  LookupMapping mapping;
  mapping.uuid = uuid;
  mapping.short_code = short_code;
  mapping.original_url = form.url;
  mapping.created_at = NowAsISO8601TimeUTC();
  mapping.content = html;

  OK_OR_RET(Upsert("lookups", mapping,
                   *ctx.Get<IDatabaseClient>()));

  // Return short code
  return Response(HTTP_200_OK, kContentTypeTextPlain,
                  short_code);
}

// GET /{shortcode} - Serve cached page for short code
Response GetCachedPage(Request request, AppContext& ctx) {
  GetCachedPageRequest get_cache_request;
  get_cache_request.short_code = request.path().substr(1);

  auto db = ctx.Get<IDatabaseClient>();
  QueryView<LookupMapping> query("lookups", db);
  SET_OR_RET(
      LookupMapping mapping,
      query
          .where(EQUALS(&LookupMapping::short_code,
                        get_cache_request.short_code))
          .first());

  SET_OR_RET(std::string html,
             MakeSingleFileHTML(mapping.original_url));

  return Response(HTTP_200_OK, kContentTypeTextHtml, html);
}

Result RunMyWebApp(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  if (config.host.empty()) {
    config.host = "0.0.0.0";
  }
  if (config.port == 0) {
    config.port = 8080;
  }

  auto app_ctx =
      ContextBuilder<AppContext>()
          .bind<IDatabaseClient>()
          .from([](AppContext&) {
            return std::make_shared<DatabaseClientImpl>(
                "http://database-service:8080");
          })
          .build();

  WebApp<AppContext> app(app_ctx);
  ADD_ROUTE(app, "GET", "/", GetIndexPage);
  ADD_ROUTE(app, "POST", "/save/", SaveResource);
  ADD_ROUTE(app, "GET", "/health/", GetHealth);
  ADD_ROUTE(app, "GET", "/*", GetCachedPage);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunMyWebApp);
}

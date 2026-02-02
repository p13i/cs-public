// cs/apps/trycopilot.ai/main.cc
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/trycopilot.ai/api/auth.hh"
#include "cs/apps/trycopilot.ai/api/game.hh"
// #include "cs/apps/trycopilot.ai/api/prompt.hh"
// #include "cs/apps/trycopilot.ai/api/prompt_driver.hh"
#include "cs/apps/common/load_endpoint.gpt.hh"
#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/index_embedded_gen_embed.h"
#include "cs/apps/trycopilot.ai/protos/acl.proto.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/config.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/uuid.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/web_app_meta.proto.hh"
#include "cs/apps/trycopilot.ai/protos/ui.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/uuid.proto.hh"
#include "cs/apps/trycopilot.ai/protos/web_app_meta.proto.hh"
#include "cs/apps/trycopilot.ai/scene1.hh"
#include "cs/apps/trycopilot.ai/scene_animator.hh"
#include "cs/apps/trycopilot.ai/ui/ui.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/favicon.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/json/serialize.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/database_base_url.gpt.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/renderer/film.hh"
#include "cs/renderer/pixel.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/embedded_files.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"
#include "tools/cpp/runfiles/runfiles.h"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ValidationError;
using ::cs::apps::common::GetHealth;
using ::cs::apps::common::GetLoad;
using ::cs::apps::trycopilotai::SceneAnimator;
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::api::GetAnimationRPC;
using ::cs::apps::trycopilotai::protos::Config;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::CreateUserResponse;
using ::cs::apps::trycopilotai::protos::GetUuidRequest;
using ::cs::apps::trycopilotai::protos::GetUuidResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Route;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::apps::trycopilotai::protos::WebAppMeta;
using ::cs::apps::trycopilotai::ui::GetAboutPage;
using ::cs::apps::trycopilotai::ui::GetAppLogsPage;
using ::cs::apps::trycopilotai::ui::GetCodePage;
using ::cs::apps::trycopilotai::ui::GetGame;
using ::cs::apps::trycopilotai::ui::GetHomePage;
using ::cs::apps::trycopilotai::ui::GetIndexPage;
using ::cs::apps::trycopilotai::ui::GetLoginPage;
using ::cs::apps::trycopilotai::ui::GetLogoutPage;
using ::cs::apps::trycopilotai::ui::GetLogsPage;
using ::cs::apps::trycopilotai::ui::GetNewWebsite;
using ::cs::apps::trycopilotai::ui::GetQuitPage;
using ::cs::apps::trycopilotai::ui::GetRegisterPage;
using ::cs::apps::trycopilotai::ui::PostLoginPage;
using ::cs::apps::trycopilotai::ui::PostLogoutPage;
using ::cs::apps::trycopilotai::ui::PostRegisterPage;
using ::cs::apps::trycopilotai::ui::Render;
using ::cs::net::http::GenerateWhiteIco;
using ::cs::net::http::Http301MovedPermanently;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_201_CREATED;
using ::cs::net::http::HTTP_302_FOUND;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_403_PERMISSION_DENIED;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::json::Object;
using ::cs::net::json::SerializeObjectPrettyPrintRecurse;
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::db::DatabaseBaseUrl;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::LocalJsonDatabase;
using ::cs::parsers::ConsumePrefix;
using ::cs::parsers::ParseArgs;
using ::cs::parsers::ParseInt;
using ::cs::parsers::ParseUnsignedInt;
using ::cs::renderer::Film;
using ::cs::renderer::Pixel;
using ::cs::util::EmbeddedFileEntry;
using ::cs::util::FindEmbedded;
using ::cs::util::di::ContextBuilder;
using ::cs::util::random::uuid::generate_uuid_v4;
}  // namespace

using AppContext = ::cs::util::di::Context<
    ::cs::net::proto::db::DatabaseBaseUrl,
    ::cs::net::proto::db::IDatabaseClient>;
using UtilContext = ::cs::util::Context;

namespace db {

std::string operator<<(const char* a, std::string b) {
  std::stringstream ss;
  ss << a << b;
  return ss.str();
}

std::string operator<<(std::string a, std::string b) {
  std::stringstream ss;
  ss << a << b;
  return ss.str();
}

static constexpr auto kPath = "/home/p13i/cs/data";

Response Get(Request request, AppContext&) {
  SET_OR_RET(std::string path,
             ConsumePrefix("/db/", request.path()));

  LocalJsonDatabase db(kPath);

  Object node;

  const static std::string TOKEN = "hilolzauthisjokes";
  auto found_token = request.GetQueryParam("token");
  if (found_token.ok() && found_token.value() == TOKEN &&
      path.empty()) {
    // List all contents
    SET_OR_RET(node, db.get(""));
  } else if (!path.empty()) {
    SET_OR_RET(node, db.get(path));
  } else {
    return Response(HTTP_403_PERMISSION_DENIED);
  }

  unsigned int indent = 2;
  auto found_indent = request.GetQueryParam("indent");
  if (found_indent.ok()) {
    SET_OR_RET(unsigned int i,
               ParseUnsignedInt(found_indent.value()));
    if (i < 0) {
      return TRACE(
          InvalidArgument("indent must be at least 0."));
    }
    indent = static_cast<unsigned int>(i);
  }

  std::stringstream ss;
  SerializeObjectPrettyPrintRecurse(ss, node, indent,
                                    /*initial_indent=*/0);

  return Response(HTTP_200_OK, kContentTypeApplicationJson,
                  ss.str());
}

Response Set(Request request, AppContext&) {
  SET_OR_RET(std::string path,
             ConsumePrefix("/db/", request.path()));

  LocalJsonDatabase db(kPath);

  SET_OR_RET(Object new_node, ParseObject(request.body()));
  if (path.empty()) {
    path = generate_uuid_v4();
  }
  OK_OR_RET(db.set(path, new_node));

  Object response = Object::NewMap(
      {{"url",
        Object::NewString("https://cs.p13i.io/db/" + path)},
       {"result", Object::NewString("ok")}});

  return Response(HTTP_201_CREATED,
                  kContentTypeApplicationJson,
                  response.str());
}

}  // namespace db

static constexpr char kContentTypeTextJavascript[] =
    "text/javascript";
static constexpr char kContentTypeApplicationWasm[] =
    "application/wasm";

Response GetFavicon(Request request, AppContext&) {
  std::stringstream ss;
  for (auto c : GenerateWhiteIco()) {
    ss << c;
  }
  return Response(HTTP_200_OK, "image/x-icon",
                  std::move(ss));
}

namespace api {
DECLARE_RPC(UuidRPC, GetUuidRequest, GetUuidResponse,
            CTX());
IMPLEMENT_RPC(UuidRPC, GetUuidRequest, GetUuidResponse) {
  GetUuidResponse response;
  response.uuid = generate_uuid_v4();
  return response;
}
}  // namespace api

Response GetIndexJs(Request request, AppContext&) {
  SET_OR_RET(std::string s,
             FindEmbedded(
                 "index.js",
                 reinterpret_cast<const EmbeddedFileEntry*>(
                     kEmbeddedFiles_index),
                 kEmbeddedFilesCount_index));
  return Response(kContentTypeTextJavascript, s);
}

Response GetIndexWasm(Request request, AppContext&) {
  SET_OR_RET(std::string s,
             FindEmbedded(
                 "index.wasm",
                 reinterpret_cast<const EmbeddedFileEntry*>(
                     kEmbeddedFiles_index),
                 kEmbeddedFilesCount_index));
  return Response(kContentTypeApplicationWasm, s);
}

Result RunMyWebApp(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  // Read arguments as various context variables for the
  // app.
  UtilContext::Write("VERSION", config.version);
  UtilContext::Write("COMMIT", config.commit);

  using AppContext =
      ::cs::util::di::Context<DatabaseBaseUrl,
                              IDatabaseClient>;
  auto app_ctx =
      ::ContextBuilder<AppContext>()
          .bind<DatabaseBaseUrl>()
          .with(std::string("http://database-service:8080"))
          .bind<IDatabaseClient>()
          .from([](AppContext& ctx) {
            return std::make_shared<DatabaseClientImpl>(
                ctx.Get<DatabaseBaseUrl>()->value());
          })
          .build();
  WebApp<AppContext> app(app_ctx);

  // Save routes to WebAppMeta (not AppContext).
  WebAppMeta web_app_meta;
#define ADD_ROUTE_WITH_CONTEXT(_method, _path, _function) \
  {                                                       \
    Route route;                                          \
    route.method = _method;                               \
    route.path = _path;                                   \
    route.function = #_function;                          \
    web_app_meta.routes.push_back(route);                 \
    ADD_ROUTE(app, _method, _path, _function);            \
  }
#define ADD_ROUTE_LOAD_WITH_CONTEXT()                    \
  {                                                      \
    Route route;                                         \
    route.method = "GET";                                \
    route.path = "/load/";                               \
    route.function = "GetLoad";                          \
    web_app_meta.routes.push_back(route);                \
    ADD_ROUTE_CAPTURE(app, "GET", "/load/",              \
                      [&app](Request r, AppContext& c) { \
                        return GetLoad(r, c, [&app] {    \
                          return app.GetThreadLoad();    \
                        });                              \
                      });                                \
  }

  ////////////////////////////////
  // SPECIAL APPS ////////////////
  ////////////////////////////////
  ADD_ROUTE_WITH_CONTEXT("GET", "/render/", Render);
  ADD_ROUTE_WITH_CONTEXT("GET", "/game/", GetGame);

  ////////////////////////////////
  // UI //////////////////////////
  ////////////////////////////////

  ADD_ROUTE_WITH_CONTEXT("GET", "/", GetIndexPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/home/", GetHomePage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/health/", GetHealth);
  ADD_ROUTE_LOAD_WITH_CONTEXT();
  ADD_ROUTE_WITH_CONTEXT("GET", "/code/*", GetCodePage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/new", GetNewWebsite);
  ADD_ROUTE_WITH_CONTEXT("GET", "/logs/", GetLogsPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/app_logs/",
                         GetAppLogsPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/register/",
                         GetRegisterPage);
  ADD_ROUTE_WITH_CONTEXT("POST", "/register/",
                         PostRegisterPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/login/", GetLoginPage);
  ADD_ROUTE_WITH_CONTEXT("POST", "/login/", PostLoginPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/logout/", GetLogoutPage);
  ADD_ROUTE_WITH_CONTEXT("POST", "/logout/",
                         PostLogoutPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/about/", GetAboutPage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/quit/", GetQuitPage);

  ////////////////////////////////
  // ASSETS //////////////////////
  ////////////////////////////////

  ADD_ROUTE_WITH_CONTEXT("GET", "/favicon.ico", GetFavicon);
  ADD_ROUTE_WITH_CONTEXT("GET", "/game/index.js",
                         GetIndexJs);
  ADD_ROUTE_WITH_CONTEXT("GET", "/game/index.wasm",
                         GetIndexWasm);

  ////////////////////////////////
  // API /////////////////////////
  ////////////////////////////////

  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/api/log/",
      cs::apps::trycopilotai::api::ListLogsRPC::HttpHandler<
          AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/api/uuid/",
      api::UuidRPC::HttpHandler<AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/api/login/",
      cs::apps::trycopilotai::api::LoginRPC::HttpHandler<
          AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/api/logout/",
      cs::apps::trycopilotai::api::LogoutRPC::HttpHandler<
          AppContext>);
  // ADD_ROUTE_WITH_CONTEXT("POST", "/api/prompt/drive/",
  //           cs::apps::trycopilotai::api::PromptDriverRPC::
  //               HttpHandler<AppContext>);
  // ADD_ROUTE_WITH_CONTEXT("GET", "/prompt/local/",
  //           cs::apps::trycopilotai::api::LocalPromptRPC::
  //               HttpHandler<AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/api/user/",
      cs::apps::trycopilotai::api::CreateUserRPC::
          HttpHandler<AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/api/game/animation/",
      cs::apps::trycopilotai::api::GetAnimationRPC::
          HttpHandler<AppContext>);
  // ADD_ROUTE_WITH_CONTEXT(
  //     "GET", "/prompt/",
  //     cs::apps::trycopilotai::api::PromptRPC::HttpHandler<AppContext>);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/rpc/users/",
      cs::apps::trycopilotai::rpc::CreateUserRPC::
          HttpHandler<AppContext>);
  ADD_ROUTE_WITH_CONTEXT("GET", "/db/*", db::Get);
  ADD_ROUTE_WITH_CONTEXT("POST", "/db/*", db::Set);

#undef ADD_ROUTE_LOAD_WITH_CONTEXT
#undef ADD_ROUTE_WITH_CONTEXT

  UtilContext::Write("APP_CONTEXT",
                     web_app_meta.Serialize());

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunMyWebApp);
}

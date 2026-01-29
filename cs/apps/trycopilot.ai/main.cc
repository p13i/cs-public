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
#include "cs/apps/trycopilot.ai/protos/context.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/context.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/uuid.proto.hh"
#include "cs/apps/trycopilot.ai/protos/ui.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/uuid.proto.hh"
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
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/renderer/film.hh"
#include "cs/renderer/pixel.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
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
using ::cs::apps::trycopilotai::SceneAnimator;
using ::cs::apps::trycopilotai::protos::Config;
using ::cs::apps::trycopilotai::protos::Context;
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
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::db::LocalJsonDatabase;
using ::cs::parsers::ConsumePrefix;
using ::cs::parsers::ParseArgs;
using ::cs::parsers::ParseInt;
using ::cs::parsers::ParseUnsignedInt;
using ::cs::renderer::Film;
using ::cs::renderer::Pixel;
using ::cs::util::random::uuid::generate_uuid_v4;
}  // namespace

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

Response Get(Request request) {
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
  cs::net::json::SerializeObjectPrettyPrintRecurse(
      ss, node, indent, /*initial_indent=*/0);

  return Response(cs::net::http::HTTP_200_OK,
                  kContentTypeApplicationJson, ss.str());
}

Response Set(Request request) {
  SET_OR_RET(std::string path,
             ConsumePrefix("/db/", request.path()));

  LocalJsonDatabase db(kPath);

  SET_OR_RET(Object new_node, ParseObject(request.body()));
  if (path.empty()) {
    path = ::cs::util::random::uuid::generate_uuid_v4();
  }
  OK_OR_RET(db.set(path, new_node));

  Object response = Object::NewMap(
      {{"url",
        Object::NewString("https://cs.p13i.io/db/" + path)},
       {"result", Object::NewString("ok")}});

  return Response(cs::net::http::HTTP_201_CREATED,
                  kContentTypeApplicationJson,
                  response.str());
}

}  // namespace db

static constexpr char kContentTypeTextJavascript[] =
    "text/javascript";
static constexpr char kContentTypeApplicationWasm[] =
    "application/wasm";

Response GetFavicon(Request request) {
  std::stringstream ss;
  for (auto c : cs::net::http::GenerateWhiteIco()) {
    ss << c;
  }
  return Response(HTTP_200_OK, "image/x-icon",
                  std::move(ss));
}

namespace api {
DECLARE_API(UuidAPI, GetUuidRequest, GetUuidResponse);
IMPLEMENT_API(UuidAPI, GetUuidRequest, GetUuidResponse) {
  GetUuidResponse response;
  response.uuid = generate_uuid_v4();
  return response;
}
}  // namespace api

Response GetIndexJs(Request request) {
  SET_OR_RET(std::string s,
             cs::util::FindEmbedded(
                 "index.js",
                 reinterpret_cast<
                     const cs::util::EmbeddedFileEntry*>(
                     kEmbeddedFiles_index),
                 kEmbeddedFilesCount_index));
  return Response(kContentTypeTextJavascript, s);
}

Response GetIndexWasm(Request request) {
  SET_OR_RET(std::string s,
             cs::util::FindEmbedded(
                 "index.wasm",
                 reinterpret_cast<
                     const cs::util::EmbeddedFileEntry*>(
                     kEmbeddedFiles_index),
                 kEmbeddedFilesCount_index));
  return Response(kContentTypeApplicationWasm, s);
}

Result RunMyWebApp(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  // Read arguments as various context variables for the
  // app.
  cs::util::Context::Write("VERSION", config.version);
  cs::util::Context::Write("COMMIT", config.commit);

  WebApp app;

  // Save routes to context variable
  Context context;
#define ADD_ROUTE_WITH_CONTEXT(_method, _path, _function) \
  {                                                       \
    Route route;                                          \
    route.method = _method;                               \
    route.path = _path;                                   \
    route.function = #_function;                          \
    context.routes.push_back(route);                      \
    ADD_ROUTE(app, _method, _path, _function);            \
  }

  ////////////////////////////////
  // SPECIAL APPS ////////////////
  ////////////////////////////////
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/render/",
      cs::apps::trycopilotai::ui::Render);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/game/", cs::apps::trycopilotai::ui::GetGame);

  ////////////////////////////////
  // UI //////////////////////////
  ////////////////////////////////

  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/", cs::apps::trycopilotai::ui::GetIndexPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/home/",
      cs::apps::trycopilotai::ui::GetHomePage);
  ADD_ROUTE_WITH_CONTEXT("GET", "/health/",
                         cs::apps::common::GetHealth);
  ADD_ROUTE_WITH_CONTEXT("GET", "/load/",
                         cs::apps::common::GetLoad);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/code/*",
      cs::apps::trycopilotai::ui::GetCodePage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/new",
      cs::apps::trycopilotai::ui::GetNewWebsite);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/logs/",
      cs::apps::trycopilotai::ui::GetLogsPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/app_logs/",
      cs::apps::trycopilotai::ui::GetAppLogsPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/register/",
      cs::apps::trycopilotai::ui::GetRegisterPage);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/register/",
      cs::apps::trycopilotai::ui::PostRegisterPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/login/",
      cs::apps::trycopilotai::ui::GetLoginPage);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/login/",
      cs::apps::trycopilotai::ui::PostLoginPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/logout/",
      cs::apps::trycopilotai::ui::GetLogoutPage);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/logout/",
      cs::apps::trycopilotai::ui::PostLogoutPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/about/",
      cs::apps::trycopilotai::ui::GetAboutPage);
  ADD_ROUTE_WITH_CONTEXT(
      "GET", "/quit/",
      cs::apps::trycopilotai::ui::GetQuitPage);

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

  ADD_ROUTE_WITH_CONTEXT("GET", "/api/log/",
                         cs::apps::trycopilotai::api::
                             ListLogsAPI::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT("GET", "/api/uuid/",
                         api::UuidAPI::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/api/login/",
      cs::apps::trycopilotai::api::LoginAPI::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT(
      "POST", "/api/logout/",
      cs::apps::trycopilotai::api::LogoutAPI::HttpHandler);
  // ADD_ROUTE_WITH_CONTEXT("POST", "/api/prompt/drive/",
  //           cs::apps::trycopilotai::api::PromptDriverAPI::
  //               HttpHandler);
  // ADD_ROUTE_WITH_CONTEXT("GET", "/prompt/local/",
  //           cs::apps::trycopilotai::api::LocalPromptAPI::
  //               HttpHandler);
  ADD_ROUTE_WITH_CONTEXT("POST", "/api/user/",
                         cs::apps::trycopilotai::api::
                             CreateUserAPI::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT("GET", "/api/game/animation/",
                         cs::apps::trycopilotai::api::
                             GetAnimationAPI::HttpHandler);
  // ADD_ROUTE_WITH_CONTEXT(
  //     "GET", "/prompt/",
  //     cs::apps::trycopilotai::api::PromptAPI::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT("POST", "/rpc/users/",
                         cs::apps::trycopilotai::rpc::
                             CreateUserRPC::HttpHandler);
  ADD_ROUTE_WITH_CONTEXT("GET", "/db/*", db::Get);
  ADD_ROUTE_WITH_CONTEXT("POST", "/db/*", db::Set);

#undef ADD_ROUTE_WITH_CONTEXT

  cs::util::Context::Write("APP_CONTEXT",
                           context.Serialize());

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunMyWebApp);
}

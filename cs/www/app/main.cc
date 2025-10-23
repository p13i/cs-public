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
#include <tuple>
#include <unordered_map>
#include <vector>

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
#include "cs/renderer/film.hh"
#include "cs/renderer/pixel.hh"
#include "cs/result.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"
#include "cs/www/app/api/auth.hh"
#include "cs/www/app/api/game.hh"
#include "cs/www/app/api/user.hh"
#include "cs/www/app/context/context.hh"
#include "cs/www/app/index_embedded_gen_embed.h"
#include "cs/www/app/protos/acl.proto.hh"
#include "cs/www/app/protos/auth.proto.hh"
#include "cs/www/app/protos/context.proto.hh"
#include "cs/www/app/protos/gencode/auth.proto.hh"
#include "cs/www/app/protos/gencode/context.proto.hh"
#include "cs/www/app/protos/gencode/user.proto.hh"
#include "cs/www/app/protos/gencode/uuid.proto.hh"
#include "cs/www/app/protos/user.proto.hh"
#include "cs/www/app/protos/uuid.proto.hh"
#include "cs/www/app/scene1.hh"
#include "cs/www/app/scene_animator.hh"
#include "cs/www/app/ui/ui.hh"
#include "tools/cpp/runfiles/runfiles.h"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ValidationError;
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
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::net::proto::db::LocalJsonDatabase;
using ::cs::parsers::ConsumePrefix;
using ::cs::parsers::ParseInt;
using ::cs::parsers::ParseUnsignedInt;
using ::cs::renderer::Film;
using ::cs::renderer::Pixel;
using ::cs::util::random::uuid::generate_uuid_v4;
using ::cs::www::app::SceneAnimator;
using ::cs::www::app::protos::CreateUserRequest;
using ::cs::www::app::protos::CreateUserResponse;
using ::cs::www::app::protos::GetUuidRequest;
using ::cs::www::app::protos::GetUuidResponse;
using ::cs::www::app::protos::LoginRequest;
using ::cs::www::app::protos::LoginResponse;
using ::cs::www::app::protos::LogoutRequest;
using ::cs::www::app::protos::LogoutResponse;
using ::cs::www::app::protos::Token;
using ::cs::www::app::protos::User;
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

inline cs::ResultOr<std::string> FindEmbedded(
    std::string_view name) {
  for (size_t i = 0; i < kEmbeddedFilesCount; ++i) {
    if (name == kEmbeddedFiles[i].name) {
      const auto* e = &kEmbeddedFiles[i];
      return std::string(
          reinterpret_cast<const char*>(e->data), e->size);
    }
  }
  return TRACE(cs::NotFoundError(
      FMT("Didn't find embedded file with name=%s",
          std::string(name))));
}

Response GetIndexJs(Request request) {
  SET_OR_RET(std::string s, FindEmbedded("index.js"));
  return Response(kContentTypeTextJavascript, s);
}

Response GetIndexWasm(Request request) {
  SET_OR_RET(std::string s, FindEmbedded("index.wasm"));
  return Response(kContentTypeApplicationWasm, s);
}

Result RunMyWebApp(std::vector<std::string> argv) {
  if (argv.size() != 6) {
    std::stringstream ss;
    ss << "Usage: " << argv[0]
       << " <path_to_data_directory <port> "
          "<repo_dir> <version> <commit>\n";
    ss << "argv=[";
    for (size_t i = 0; i < argv.size(); ++i) {
      ss << argv[i] << " ";
    }
    ss << "]\n" << ENDL;
    return cs::InvalidArgument(ss.str());
  }

  // Read arguments as various context variables for the
  // app.
  cs::www::app::Context::Write("DATA_DIR", argv[1]);
  cs::www::app::Context::Write("PORT", argv[2]);
  cs::www::app::Context::Write("REPO_DIR", argv[3]);
  cs::www::app::Context::Write("VERSION", argv[4]);
  cs::www::app::Context::Write("COMMIT", argv[5]);

  cs::net::http::WebApp app;

  // Save routes to context variable
  cs::www::app::protos::Context context;
#define ADD_ROUTE(_method, _path, _function) \
  {                                          \
    cs::www::app::protos::Route route;       \
    route.method = _method;                  \
    route.path = _path;                      \
    route.function = #_function;             \
    context.routes.push_back(route);         \
    app.Register(_method, _path, _function); \
  }

  ////////////////////////////////
  // SPECIAL APPS ////////////////
  ////////////////////////////////
  ADD_ROUTE("GET", "/render/", cs::www::app::ui::Render);
  ADD_ROUTE("GET", "/game/", cs::www::app::ui::GetGame);

  ////////////////////////////////
  // UI //////////////////////////
  ////////////////////////////////

  ADD_ROUTE("GET", "/", cs::www::app::ui::GetIndexPage);
  ADD_ROUTE("GET", "/home/", cs::www::app::ui::GetHomePage);
  ADD_ROUTE("GET", "/health/",
            cs::www::app::ui::GetHealthPage);
  ADD_ROUTE("GET", "/cs*", cs::www::app::ui::GetCodePage);
  ADD_ROUTE("GET", "/new", cs::www::app::ui::GetNewWebsite);
  ADD_ROUTE("GET", "/logs/", cs::www::app::ui::GetLogsPage);
  ADD_ROUTE("GET", "/app_logs/",
            cs::www::app::ui::GetAppLogsPage);
  ADD_ROUTE("GET", "/register/",
            cs::www::app::ui::GetRegisterPage);
  ADD_ROUTE("POST", "/register/",
            cs::www::app::ui::PostRegisterPage);
  ADD_ROUTE("GET", "/login/",
            cs::www::app::ui::GetLoginPage);
  ADD_ROUTE("POST", "/login/",
            cs::www::app::ui::PostLoginPage);
  ADD_ROUTE("GET", "/logout/",
            cs::www::app::ui::GetLogoutPage);
  ADD_ROUTE("POST", "/logout/",
            cs::www::app::ui::PostLogoutPage);
  ADD_ROUTE("GET", "/about/",
            cs::www::app::ui::GetAboutPage);
  ADD_ROUTE("GET", "/quit/", cs::www::app::ui::GetQuitPage);

  ////////////////////////////////
  // ASSETS //////////////////////
  ////////////////////////////////

  ADD_ROUTE("GET", "/favicon.ico", GetFavicon);
  ADD_ROUTE("GET", "/game/index.js", GetIndexJs);
  ADD_ROUTE("GET", "/game/index.wasm", GetIndexWasm);

  ////////////////////////////////
  // API /////////////////////////
  ////////////////////////////////

  ADD_ROUTE("GET", "/api/log/",
            cs::www::app::api::ListLogsAPI::HttpHandler);
  ADD_ROUTE("GET", "/api/uuid/", api::UuidAPI::HttpHandler);
  ADD_ROUTE("POST", "/api/login/",
            cs::www::app::api::LoginAPI::HttpHandler);
  ADD_ROUTE("POST", "/api/logout/",
            cs::www::app::api::LogoutAPI::HttpHandler);
  ADD_ROUTE("POST", "/api/user/",
            cs::www::app::api::CreateUserAPI::HttpHandler);
  ADD_ROUTE(
      "GET", "/api/game/animation/",
      cs::www::app::api::GetAnimationAPI::HttpHandler);
  ADD_ROUTE("POST", "/rpc/users/",
            cs::www::app::rpc::CreateUserRPC::HttpHandler);
  ADD_ROUTE("GET", "/db/*", db::Get);
  ADD_ROUTE("POST", "/db/*", db::Set);

#undef ADD_ROUTE

  cs::www::app::Context::Write("APP_CONTEXT",
                               context.Serialize());

  return app.RunServer(
      "0.0.0.0", cs::www::app::Context::Read("PORT")
                     .then(cs::parsers::ParseUnsignedInt)
                     .value_or(8080));
}

int main(int argc, char** argv) {
  return cs::Result::Main(argc, argv, RunMyWebApp);
}

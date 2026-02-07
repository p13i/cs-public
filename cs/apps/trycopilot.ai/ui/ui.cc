// cs/apps/trycopilot.ai/ui/ui.cc
#include "cs/apps/trycopilot.ai/ui/ui.hh"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "cs/apps/common/domain_resolver.gpt.hh"
#include "cs/apps/common/website.gpt.hh"
#include "cs/apps/common/website_nav.gpt.hh"
#include "cs/apps/trycopilot.ai/api/auth.hh"
#include "cs/apps/trycopilot.ai/api/game.hh"
#include "cs/apps/trycopilot.ai/api/user.hh"
#include "cs/apps/trycopilot.ai/protos/acl.proto.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/game.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/game.proto.hh"
#include "cs/apps/trycopilot.ai/protos/gencode/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/ui.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/web_app_meta.proto.hh"
#include "cs/apps/trycopilot.ai/scene_animator.hh"
#include "cs/log.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/html/minify.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/serialize.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/db/query_helpers.gpt.hh"
#include "cs/net/proto/db/query_view.gpt.hh"
#include "cs/net/proto/form/proto_form.gpt.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/di/context.gpt.hh"
#include "cs/util/string.hh"
#include "cs/util/time.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::ResultOr;
using ::cs::apps::common::GetDomainForService;
using ::cs::apps::common::website::BuildWebsiteConfig;
using ::cs::apps::common::website::Hero;
using ::cs::apps::common::website::NavItemSpec;
using ::cs::apps::common::website::PageOptions;
using ::cs::apps::common::website::WebsiteConfig;
using ::cs::apps::common::website::WebsiteUser;
using ::cs::apps::common::website::WebsiteUserInfo;
using ::cs::apps::trycopilotai::SceneAnimator;
using ::cs::apps::trycopilotai::api::CreateUserRPC;
using ::cs::apps::trycopilotai::api::ListAppLogsRPC;
using ::cs::apps::trycopilotai::api::ListLogsRPC;
using ::cs::apps::trycopilotai::api::LoginRPC;
using ::cs::apps::trycopilotai::api::LogoutRPC;
using ::cs::apps::trycopilotai::protos::CreateUserRequest;
using ::cs::apps::trycopilotai::protos::GetAnimationRequest;
using ::cs::apps::trycopilotai::protos::
    GetAnimationResponse;
using ::cs::apps::trycopilotai::protos::ListAppLogsResponse;
using ::cs::apps::trycopilotai::protos::ListLogsResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::protos::LogoutRequest;
using ::cs::apps::trycopilotai::protos::LogoutResponse;
using ::cs::apps::trycopilotai::protos::Token;
using ::cs::apps::trycopilotai::protos::User;
using ::cs::apps::trycopilotai::protos::WebAppMeta;
using ::cs::apps::trycopilotai::ui::GetAuthenticatedUser;
using ::cs::apps::trycopilotai::ui::GetToken;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_302_FOUND;
using ::cs::net::http::HTTP_403_PERMISSION_DENIED;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::EQUALS;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::db::QueryView;
using ::cs::net::proto::db::Upsert;
using ::cs::net::proto::form::GenerateProtoForm;
using ::cs::net::rpc::ExtractProtoFromRequest;
using ::cs::parsers::ParseUnsignedInt;
using ::cs::util::di::Context;
using ::cs::util::string::ToLowercase;
}  // namespace

namespace cs::apps::trycopilotai::ui {

using AppContext = Context<IDatabaseClient>;

#define ANONYMOUS_USERS_ONLY(request, ctx)       \
  if (!Result::IsNotFound(                       \
          GetAuthenticatedUser(request, ctx))) { \
    return Response(HTTP_403_PERMISSION_DENIED); \
  }

#define LOGGED_IN_USERS_ONLY(request, ctx)        \
  if (!GetAuthenticatedUser(request, ctx).ok()) { \
    return Response(HTTP_403_PERMISSION_DENIED);  \
  }

#define ADMIN_USERS_ONLY(lhs, request, ctx) \
  lhs = GetAuthenticatedUser(request, ctx); \
  if (!user.ok() || !user.value().admin) {  \
    return Response(HTTP_404_NOT_FOUND);    \
  }

static std::string AuthCookieAttributes() {
  // Cookie attributes for auth token
  // For local/docker: no Domain (cookies work with
  // hostname) For production: Domain=.trycopilot.ai (shares
  // across subdomains) Secure flag only with HTTPS
  std::string attrs = "; HttpOnly; SameSite=Lax; Path=/";

  // Check if running in production (HTTPS enabled)
  bool is_production =
      cs::util::Context::Read("HTTPS").value_or("false") ==
      "true";

  if (is_production) {
    attrs += "; Domain=.trycopilot.ai; Secure";
  }
  // For local/docker testing: no Domain attribute (more
  // permissive)

  return attrs;
}

// Source:
// https://chatgpt.com/share/67df4a37-ccf4-800e-bb0b-630599cd7766
std::string extractAuthToken(const std::string& cookie) {
  std::string key = "auth_token=";
  size_t start = cookie.find(key);
  if (start == std::string::npos) {
    return "";  // auth_token not found
  }
  start += key.length();
  size_t end = cookie.find(';', start);
  return (end == std::string::npos)
             ? cookie.substr(start)
             : cookie.substr(start, end - start);
}

ResultOr<std::string> GetToken(const Request& request) {
  std::string cookie;
  for (const auto& [name, value] : request._headers) {
    if (ToLowercase(name) == "cookie") {
      cookie = value;
      break;
    }
  }
  if (cookie.empty()) {
    return TRACE(NotFoundError("No cookie found."));
  }
  std::string token_uuid = extractAuthToken(cookie);
  if (token_uuid.empty()) {
    return TRACE(
        NotFoundError("No auth_token found in cookie."));
  }
  return token_uuid;
}

ResultOr<User> GetAuthenticatedUser(Request request,
                                    AppContext& ctx) {
  SET_OR_RET(std::string token_uuid, GetToken(request));
  LOG(INFO) << "[DEBUG] GetAuthenticatedUser - token_uuid="
            << token_uuid << ENDL;

  auto db = ctx.Get<IDatabaseClient>();
  QueryView<Token> tokens_query("tokens", db);

  SET_OR_RET(
      auto token,
      tokens_query.where(EQUALS(&Token::uuid, token_uuid))
          .first());
  LOG(INFO) << "[DEBUG] Token found - user_uuid="
            << token.user_uuid << " active=" << token.active
            << ENDL;

  if (!token.active) {
    return TRACE(
        NotFoundError("Token inactive or logged out."));
  }

  QueryView<User> users_query("users", db);
  SET_OR_RET(auto user, users_query
                            .where(EQUALS(&User::uuid,
                                          token.user_uuid))
                            .first());

  return user;
}

ResultOr<std::string> MakeWebsite(
    ResultOr<User> user, std::string active_page_path,
    std::string content, const Request& request) {
  const bool is_logged_in = user.ok();
  const bool is_admin = is_logged_in && user.value().admin;

  std::vector<NavItemSpec> nav_items;
  {
    NavItemSpec item{};  // Initialize all fields to zero
    item.href = "/";
    item.label = "Index";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/game/";
    item.label = "Game";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    SET_OR_RET(std::string code_domain,
               GetDomainForService("code-viewer", request));
    item.href = "https://" + code_domain + "/";
    item.label = "Code (cs)";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    SET_OR_RET(std::string data_domain,
               GetDomainForService("data-viewer", request));
    item.href = "https://" + data_domain + "/";
    item.label = "Data";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    SET_OR_RET(std::string blob_domain,
               GetDomainForService("blob-viewer", request));
    item.href = "https://" + blob_domain + "/";
    item.label = "Blobs";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/home/";
    item.label = "Home";
    item.requires_auth = true;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/render/";
    item.label = "Render";
    item.requires_auth = true;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/logs/";
    item.label = "HTTP Logs";
    item.requires_auth = true;
    item.requires_admin = true;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/app_logs/";
    item.label = "App Logs";
    item.requires_auth = true;
    item.requires_admin = true;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/about/";
    item.label = "About";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/logout/";
    item.label = "Logout";
    item.requires_auth = true;
    item.requires_admin = false;
    item.requires_logged_out = false;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/login/";
    item.label = "Login";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = true;
    nav_items.push_back(item);
  }
  {
    NavItemSpec item{};
    item.href = "/register/";
    item.label = "Register";
    item.requires_auth = false;
    item.requires_admin = false;
    item.requires_logged_out = true;
    nav_items.push_back(item);
  }

  PageOptions opts{};
  opts.nav_base = "";
  opts.nav_items = std::move(nav_items);
  {
    WebsiteUserInfo user_info{};
    user_info.logged_in = is_logged_in;
    user_info.email =
        is_logged_in ? user.value().email : "";
    user_info.admin = is_admin;
    opts.user = user_info;
  }
  opts.active_path = std::move(active_page_path);
  opts.content_html = std::move(content);
  opts.title = "trycopilot.ai";

  WebsiteConfig config = BuildWebsiteConfig(opts);
  return cs::apps::common::website::MakeWebsite(config);
}

static Response MakeWebsiteErrorResponse(
    const ResultOr<std::string>& html_or) {
  return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                  kContentTypeTextPlain, html_or.message());
}

Response GetIndexPage(Request request, AppContext& ctx) {
  auto html_or =
      MakeWebsite(GetAuthenticatedUser(request, ctx), "/",
                  Hero(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetGame(Request request, AppContext& ctx) {
  using namespace cs::net::html::dom;
  auto html_or = MakeWebsite(
      GetAuthenticatedUser(request, ctx), "/game/", R"html(
<canvas id="canvas"
  oncontextmenu="event.preventDefault()" tabindex=-1></canvas>
<hr/>
<div id="spinner">
</div>
<div id="status">
  Downloading...</div>
<span id="controls">
  <span><input type="checkbox" id="resize">Resize canvas</span>
  <span><input type="checkbox" id="pointerLock" checked>Lock/hide mouse
    pointer &nbsp;&nbsp;&nbsp;</span>
  <span><input type="button" value="Fullscreen"
      onclick="Module.requestFullscreen(
        document.getElementById('pointerLock').checked,
        document.getElementById('resize').checked)">
  </span>
</span>
<progress value="0" max="100" id="progress" hidden=1></progress>
<hr/>
<textarea id="output" rows="40" cols="80"></textarea>
<script type="text/javascript">
  var statusElement = document.getElementById('status');
  var progressElement = document.getElementById('progress');
  var spinnerElement = document.getElementById('spinner');

  var Module = {
    preRun: [],
    postRun: [],
    print: (function() {
      var element = document.getElementById('output');
      if (element) element.value = ''; // clear browser cache
      return function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        // These replacements are necessary if you render to raw HTML
        //text = text.replace(/&/g, "&amp;");
        //text = text.replace(/</g, "&lt;");
        //text = text.replace(/>/g, "&gt;");
        //text = text.replace('\n', '<br>', 'g');
        console.log(text);
        if (element) {
          element.value += text + "\n";
          element.scrollTop = element.scrollHeight; // focus on bottom
        }
      };
    })(),
    canvas: (() => {
      var canvas = document.getElementById('canvas');

      // As a default initial behavior, pop up an alert when webgl context is lost. To make your
      // application robust, you may want to override this behavior before shipping!
      // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
      canvas.addEventListener("webglcontextlost", (e) => {
        alert('WebGL context lost. You will need to reload the page.');
        e.preventDefault();
      }, false);

      return canvas;
    })(),
    setStatus: (text) => {
      if (!Module.setStatus.last) Module.setStatus.last = {
        time: Date.now(),
        text: ''
      };
      if (text === Module.setStatus.last.text) return;
      var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
      var now = Date.now();
      if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
      Module.setStatus.last.time = now;
      Module.setStatus.last.text = text;
      if (m) {
        text = m[1];
        progressElement.value = parseInt(m[2]) * 100;
        progressElement.max = parseInt(m[4]) * 100;
        progressElement.hidden = false;
        spinnerElement.hidden = false;
      } else {
        progressElement.value = null;
        progressElement.max = null;
        progressElement.hidden = true;
        if (!text) spinnerElement.style.display = 'none';
      }
      statusElement.innerHTML = text;
    },
    totalDependencies: 0,
    monitorRunDependencies: (left) => {
      this.totalDependencies = Math.max(this.totalDependencies, left);
      Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    }
  };
  Module.setStatus('Downloading...');
  window.onerror = (event) => {
    // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
    Module.setStatus('Exception thrown, see JavaScript console');
    spinnerElement.style.display = 'none';
    Module.setStatus = (text) => {
      if (text) console.error('[post-exception status] ' + text);
    };
  };
</script>
<script async type="text/javascript" src="index.js"></script>
)html",
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetHealthPage(Request, AppContext&) {
  return Ok();
}

inline void WriteJson(
    const std::vector<
        std::vector<std::vector<std::vector<int>>>>& a,
    std::ostream& out) {
  out.put('[');
  for (size_t i = 0, n = a.size(); i < n; ++i) {
    if (i) out.put(',');
    out.put('[');
    const auto& b = a[i];
    for (size_t j = 0, m = b.size(); j < m; ++j) {
      if (j) out.put(',');
      out.put('[');
      const auto& c = b[j];
      for (size_t k = 0, p = c.size(); k < p; ++k) {
        if (k) out.put(',');
        out.put('[');
        const auto& d = c[k];
        for (size_t t = 0, q = d.size(); t < q; ++t) {
          if (t) out.put(',');
          char buf[24];
          auto res =
              std::to_chars(buf, buf + sizeof(buf), d[t]);
          out.write(buf, res.ptr - buf);
        }
        out.put(']');
      }
      out.put(']');
    }
    out.put(']');
  }
  out.put(']');
}

Response Render(Request request, AppContext& ctx) {
  std::function<ResultOr<unsigned int>(std::string)>
      parse_unsigned_int = ParseUnsignedInt;

  LOG(DEBUG) << "Rendering page with query params: "
             << STREAM_STRING_MAP(request.query_params())
             << ENDL;

  unsigned int width = request.GetQueryParam("width")
                           .then(parse_unsigned_int)
                           .value_or(128);

  LOG(DEBUG) << "Parsed width: " << width << ENDL;

  unsigned int height = request.GetQueryParam("height")
                            .then(parse_unsigned_int)
                            .value_or(128);

  LOG(DEBUG) << "Parsed height: " << height << ENDL;

  unsigned int num_frames =
      request.GetQueryParam("num_frames")
          .then(parse_unsigned_int)
          .value_or(1);

  LOG(DEBUG) << "Parsed num_frames: " << num_frames << ENDL;

  GetAnimationRequest get_animation_request;
  get_animation_request.width = width;
  get_animation_request.height = height;
  get_animation_request.num_frames = num_frames;

  const auto [get_animation_result, render_time_ms] =
      cs::util::timeit(cs::util::make_function([&]() {
        return cs::apps::trycopilotai::api::
            GetAnimationRPC()
                .Process(get_animation_request);
      }));
  if (!get_animation_result.ok()) {
    return get_animation_result;
  }

  LOG(DEBUG) << "Constructing response..." << ENDL;

  GetAnimationResponse get_animation_response =
      get_animation_result.value();

  // clang-format off
  std::stringstream ss;
  ss << "<p>Ray-tracer rendered " << get_animation_response.request.num_frames << " frames in "
    << std::to_string(render_time_ms) << " ms at " << width << "x" << height << "px.</p>";
  ss << R"html(
<canvas id="canvas" width=")html" << width
  << R"html(" height=")html" << height 
  << R"html("></canvas>
<p id="fps"></p>
<form action="/render/" method="GET">
  <dl>
    <dt><label for="width">Width:</label></dt>
    <dd><input type="number" id="width" name="width" value=")html" << width << R"html(">px</dd>
    <dt><label for="height">Height:</label></dt>
    <dd><input type="number" id="height" name="height" value=")html" << height << R"html(">px</dd>
    <dt><label for="num_frames">Number of frames:</label></dt>
    <dd><input type="number" id="num_frames" name="num_frames" value=")html" << num_frames << R"html("></dd>
  </dl>
  <input type="submit" value="Render">
</form>
<script type="text/javascript">
  const IMAGES = )html";
  WriteJson(get_animation_response.data, ss);
  ss << R"html(;
  const FPS = )html" << 24 << R"html(;
  var i = 0;
  var oneSecondStartMs = 0;
  var oneSecondEndMs = 0;
  var drawMs = 0;
  const canvas = document.getElementById('canvas');
  var ctx = canvas.getContext('2d');
  function drawImage() {
    const startMs = Date.now();
    if (i % FPS == 0) {
      oneSecondStartMs = startMs;
    }
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    for (var y = canvas.height - 1; y >= 0; y--) {
      for (var x = 0; x < canvas.width; x++) {
        var [r, g, b, a] = IMAGES[i][x][y];
        a = 1;
        ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${a})`;
        ctx.fillRect(x, y, 1, 1);
      }
    }
    const endMs = Date.now();
    if (i % FPS == (FPS - 1)) {
      oneSecondEndMs = endMs;
      const oneSceneSecondInMs = (oneSecondEndMs - oneSecondStartMs);
      const fps = (FPS * 1000) / oneSceneSecondInMs;
      const message = `Drawing at ${fps.toFixed(2)} fps.`;
      document.getElementById("fps").innerHTML = message;
    }
    i = (i + 1) % IMAGES.length;
    drawMs = endMs - startMs;
    queueDrawImage();
  }

  function queueDrawImage() {
    const delayMs = 1000 / FPS - drawMs;
    setTimeout(drawImage, delayMs);
  }

  document.addEventListener("DOMContentLoaded", function() {
    queueDrawImage();
  });
</script>
)html";
  // clang-format on

  LOG(DEBUG) << "Constructing response... done." << ENDL;

  auto html_or =
      MakeWebsite(GetAuthenticatedUser(request, ctx),
                  "/render/", ss.str(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetLoginPage(Request request, AppContext& ctx) {
  using namespace cs::net::html::dom;
  ANONYMOUS_USERS_ONLY(request, ctx);
  const auto form_html =
      GenerateProtoForm<LoginRequest>("/login/", "POST");
  auto html_or = MakeWebsite(
      NotFoundError("ANONYMOUS_USERS_ONLY"), "/login/",
      div(h1("Login"), p("Login to your account."),
          form_html),
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response PostLoginPage(Request request, AppContext& ctx) {
  ANONYMOUS_USERS_ONLY(request, ctx);
  SET_OR_RET(
      LoginRequest login_request,
      ExtractProtoFromRequest<LoginRequest>(request));

  SET_OR_RET(LoginResponse login_response,
             LoginRPC(ctx).Process(login_request));

  auto html_or = MakeWebsite(
      GetAuthenticatedUser(request, ctx), "/home/",
      "Logged in! Redirecting to <a "
      "href=\"/home/\">home</a>...",
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return Response(HTTP_302_FOUND, kContentTypeTextHtml,
                  html_or.value())
      .SetHeader("Set-Cookie",
                 "auth_token=" + login_response.token.uuid +
                     AuthCookieAttributes())
      .SetHeader("Location", "/home/");
}

Response GetLogoutPage(Request request, AppContext& ctx) {
  LOGGED_IN_USERS_ONLY(request, ctx);
  using namespace cs::net::html::dom;
  auto html_or = MakeWebsite(
      GetAuthenticatedUser(request, ctx), "/logout/",
      div(h1("Logout"),
          p("Are you sure you want to log out?"),
          R"html(<form action="/logout/" method="POST">
  <input type="submit" value="Logout">
</form>)html"),
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response PostLogoutPage(Request request, AppContext& ctx) {
  LOGGED_IN_USERS_ONLY(request, ctx);
  auto token = GetToken(request);
  std::string email = "";
  if (!token.ok() && !Result::IsNotFound(token)) {
    LOG(WARNING) << "Token not found in request." << ENDL;
    return token;
  } else if (token.ok()) {
    LogoutRequest logout_request;
    logout_request.token_uuid = token.value();
    auto logout_response =
        LogoutRPC(ctx).Process(logout_request);
    if (!logout_response.ok() &&
        !Result::IsNotFound(logout_response.result())) {
      LOG(WARNING)
          << "Unable to log out user, token_uuid=" +
                 logout_request.token_uuid
          << ENDL;
      return logout_response;
    } else if (logout_response.ok()) {
      email = logout_response.value().email;
    }
  }

  if (email.size() > 0) {
    email = " " + email;
  }

  // Redirect to login page after logout
  return Response(HTTP_302_FOUND, kContentTypeTextHtml,
                  "Logged out successfully. Redirecting to "
                  "login...")
      .SetHeader("Set-Cookie",
                 "auth_token=; Expires=Thu, 01 Jan 1970 "
                 "00:00:00 GMT; Max-Age=0" +
                     AuthCookieAttributes())
      .SetHeader("Location", "/login/");
}

Response GetRegisterPage(Request request, AppContext& ctx) {
  using namespace cs::net::html::dom;
  ANONYMOUS_USERS_ONLY(request, ctx);
  const auto form_html =
      GenerateProtoForm<CreateUserRequest>("/register/",
                                           "POST");
  auto html_or = MakeWebsite(
      NotFoundError("ANONYMOUS_USERS_ONLY"), "/register/",
      div(h1("Register"), p("Create a new account."),
          form_html),
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response PostRegisterPage(Request request,
                          AppContext& ctx) {
  ANONYMOUS_USERS_ONLY(request, ctx);
  LOG(INFO) << "[DEBUG] PostRegister START - body_size="
            << request.body().size() << " content_type="
            << (request.headers().count("Content-Type")
                    ? request.headers().at("Content-Type")
                    : "none")
            << " body=" << request.body() << ENDL;
  SET_OR_RET(CreateUserRequest request_proto,
             ExtractProtoFromRequest<
                 cs::apps::trycopilotai::protos::
                     CreateUserRequest>(request));
  LOG(INFO) << "[DEBUG] Extracted proto - email="
            << request_proto.email << " password_len="
            << request_proto.password.size()
            << " confirm_len="
            << request_proto.confirm_password.size()
            << ENDL;
  SET_OR_RET(auto create_user_response,
             CreateUserRPC(ctx).Process(request_proto));

  LOG(INFO) << "[DEBUG] User created successfully: "
            << create_user_response.email
            << " uuid=" << create_user_response.user_uuid
            << ENDL;

  // Create token for auto-login without querying database
  // again (avoids read-after-write consistency issues in
  // distributed setup)
  Token token;
  token.user_uuid = create_user_response.user_uuid;
  token.uuid = cs::util::random::uuid::generate_uuid_v4();
  token.active = true;

  LOG(INFO) << "[DEBUG] Before token INSERT - uuid="
            << token.uuid
            << " user_uuid=" << token.user_uuid << ENDL;
  auto insert_result =
      Upsert("tokens", token, *ctx.Get<IDatabaseClient>());
  if (!insert_result.ok()) {
    LOG(ERROR) << "[DEBUG] Token INSERT FAILED: "
               << insert_result.message() << ENDL;
    return insert_result;
  }
  LOG(INFO) << "[DEBUG] Token INSERT succeeded: "
            << token.uuid << ENDL;

  // Return success page with delayed redirect (gives DB
  // replicas time to sync)
  using namespace cs::net::html::dom;
  std::stringstream success_html;
  success_html
      << h1("Welcome!")
      << p("Registration successful! Your account has been "
           "created.")
      << p("You will be redirected to your home page in 5 "
           "seconds...")
      << p(R"(<a href="/home/">Or click here to continue now</a>)")
      << R"(<meta http-equiv="refresh" content="5;url=/home/">)";

  auto html_or = MakeWebsite(
      NotFoundError("Registration success"), "/register/",
      success_html.str(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return Response(HTTP_200_OK, kContentTypeTextHtml,
                  html_or.value())
      .SetHeader("Set-Cookie", "auth_token=" + token.uuid +
                                   AuthCookieAttributes());
}

Response GetHomePage(Request request, AppContext& ctx) {
  LOGGED_IN_USERS_ONLY(request, ctx);

  auto user = GetAuthenticatedUser(request, ctx);

  std::stringstream ss;
  ss << R"html(
        <h1>Home</h1>
        <p>Welcome home, )html"
     << user.value().email << R"html(</p>)html";
  // Check if request has a valid token.
  if (user.value().admin) {
    ss << R"html(<hr/>
       <h2>Sitemap</h2>
         <ul>)html";
    SET_OR_RET(WebAppMeta web_app_meta,
               cs::util::Context::Read("APP_CONTEXT")
                   .then([&](auto s) {
                     return cs::apps::trycopilotai::protos::
                         WebAppMeta()
                             .Parse(s);
                   }));
    for (auto route : web_app_meta.routes) {
      ss << "<li><code>" << route.method
         << "&nbsp;<a href=\"" << route.path << "\">"
         << route.path << "</a></code></li>";
    }
    ss << "</ul>";
  }
  auto html_or =
      MakeWebsite(user, "/home/", ss.str(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetLogsPage(Request request, AppContext& ctx) {
  ADMIN_USERS_ONLY(auto user, request, ctx);

  SET_OR_RET(ListLogsResponse response,
             ListLogsRPC(ctx).Process(
                 cs::apps::trycopilotai::protos::
                     ListLogsRequest()));

  std::stringstream ss;
  using namespace cs::net::html::dom;
  ss << h1("HTTP logs (" +
           std::to_string(response.http_logs.size()) + ")");
  ss << R"html(<table>
  <tr>
    <th>Request UUID</th>
    <th>Receive Timestamp</th>
    <th>Method</th>
    <th>Path</th>
    <th>Request Body</th>
    <th>Response HTTP Status</th>
    <th>Response Body</th>
    <th>Result Code</th>
  </tr>)html";

  for (const auto& log : response.http_logs) {
    ss << "<tr>";
    ss << "<td><code>" << log.uuid << "</code></td>";
    ss << "<td><code>"
       << log.request.receive_timestamp.iso8601
       << "</code></td>";
    ss << "<td><code>" << log.request.method
       << "</code></td>";
    ss << "<td><code>" << log.request.path
       << "</code></td>";
    ss << "<td><code>" << log.request.body.substr(0, 32)
       << "</code></td>";
    ss << "<td><code>" << log.response.status.code << " "
       << log.response.status.name << "</code></td>";
    ss << "<td><code>" << log.response.body.substr(0, 32)
       << "</code></td>";
    ss << "<td><code>" << log.result.code << "</code></td>";
    ss << "</tr>";
  }

  ss << R"html(
      </table>)html";
  auto html_or =
      MakeWebsite(user, "/logs/", ss.str(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetAppLogsPage(Request request, AppContext& ctx) {
  ADMIN_USERS_ONLY(auto user, request, ctx);

  SET_OR_RET(ListAppLogsResponse response,
             ListAppLogsRPC(ctx).Process(
                 cs::apps::trycopilotai::protos::
                     ListAppLogsRequest()));

  std::stringstream ss;
  using namespace cs::net::html::dom;
  ss << h1("App logs (" +
           std::to_string(response.items.size()) + ")");
  ss << R"html(
  <table>
  <tr>
    <th>Timestamp</th>
    <th>Level</th>
    <th>File</th>
    <th>Line</th>
    <th>Message</th>
  </tr>)html";

  for (const auto& log : response.items) {
    ss << "<tr>";
    ss << "<td><code>" << log.timestamp << "</code></td>";
    ss << "<td><code>" << log.level << "</code></td>";
    ss << "<td><code>" << log.file << "</code></td>";
    ss << "<td><code>" << log.line << "</code></td>";
    // Show a concise preview; full message likely
    // multi-line.
    std::string preview =
        log.message.size() > 256
            ? (log.message.substr(0, 256) + "…")
            : log.message;
    ss << "<td><code>" << preview << "</code></td>";
    ss << "</tr>";
  }

  ss << R"html(</table>)html";
  auto html_or =
      MakeWebsite(user, "/app/logs/", ss.str(), request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetQuitPage(Request request, AppContext& ctx) {
  ADMIN_USERS_ONLY(auto user, request, ctx);
  // Flush buffer with endl
  LOG(FATAL) << "Quitting app." << ENDL;
  exit(-1);
}

Response GetCodePage(Request request, AppContext&) {
  auto domain_or =
      GetDomainForService("code-viewer", request);
  if (!domain_or.ok()) {
    return Response(HTTP_500_INTERNAL_SERVER_ERROR,
                    kContentTypeTextPlain,
                    domain_or.message());
  }
  std::string redirect_url =
      "https://" + domain_or.value() + "/";

  return Response(HTTP_302_FOUND, kContentTypeTextPlain,
                  "Redirecting to code viewer...")
      .SetHeader("Location", redirect_url);
}

Response GetNewWebsite(Request request, AppContext& ctx) {
  auto html_or = cs::apps::trycopilotai::ui::MakeWebsite(
      GetAuthenticatedUser(request, ctx), "/new/",
      R"html(
<h1>New Website</h1>
<p>This is a new website!</p>
)html",
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

Response GetAboutPage(Request request, AppContext& ctx) {
  using namespace ::cs::net::html::dom;
  auto html_or = cs::apps::trycopilotai::ui::MakeWebsite(
      GetAuthenticatedUser(request, ctx), "/about/",
      div(h1("About" + nbsp() + code("cs")),
          p(R"(We bring AI closer to the human experience in
                ways that have an unambiguous and measurable
                benefit to our users daily living. We create
                virtual characters and
                artificially-intelligent agents that live
                with us, learn how to work with us, and
                enable us to achieve our highest ambitions.
                Through our research and engineering work,
                we are taking the world one step closer to
                blurring the lines between the physical and
                digital worlds.)"),
          hr(),
          p(R"(This web application is handled by a C++
                framework made only using standard library
                methods, with no external includes (except
                curl because who wants to work directly with
                TLS, yikes).)"),
          p(R"(This framework also supports a protocol
                buffer implemention for JSON over HTTP. This
                repo also includes code for a raytracer,
                technical interview questions, a
                protobuf-based async database; a website
                archiver (to be deployed at )",
            nbsp(), a("https://cite.pub", code("cite.pub")),
            R"(); all in standard C++.)"),
          p(R"(Sharing demos of C-based programs is
                difficult because the code needs to be
                compiled for the host architecture. Demos in
                web apps, however, only require a user to
                open a website in their browser. This
                project includes a proof-of-concept
                implementation of WebAssembly running a
                C++ program in the browser (via Emscripten
                and with Bazel).
                Such a framework will allow me to share
                great demos in the future.)"),
          p(R"(-Pramod)")),
      request);
  if (!html_or.ok()) {
    return MakeWebsiteErrorResponse(html_or);
  }
  return HtmlResponse(html_or.value());
}

}  // namespace cs::apps::trycopilotai::ui

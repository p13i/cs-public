#include "cs/www/app/ui/ui.hh"

#include <charconv>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>

#include "cs/log.hh"
#include "cs/net/html/dom.hh"
#include "cs/net/html/minify.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/serialize.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/net/proto/db/db_deferred_async.hh"
#include "cs/parsers/parsers.hh"
#include "cs/util/time.hh"
#include "cs/www/app/api/auth.hh"
#include "cs/www/app/api/game.hh"
#include "cs/www/app/api/user.hh"
#include "cs/www/app/context/context.hh"
#include "cs/www/app/protos/acl.proto.hh"
#include "cs/www/app/protos/auth.proto.hh"
#include "cs/www/app/protos/context.proto.hh"
#include "cs/www/app/protos/game.proto.hh"
#include "cs/www/app/protos/gencode/game.proto.hh"
#include "cs/www/app/protos/user.proto.hh"
#include "cs/www/app/scene_animator.hh"

namespace {  // use_usings
using ::cs::NotFoundError;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::www::app::Context;
using ::cs::www::app::SceneAnimator;
using ::cs::www::app::protos::LoginRequest;
using ::cs::www::app::protos::LoginResponse;
using ::cs::www::app::protos::LogoutRequest;
using ::cs::www::app::protos::LogoutResponse;
using ::cs::www::app::protos::Token;
using ::cs::www::app::protos::User;
}  // namespace

namespace cs::www::app::ui {

#define ANONYMOUS_USERS_ONLY(request)               \
  if (!Result::IsNotFound(                          \
          cs::www::app::ui::GetAuthenticatedUser(   \
              request))) {                          \
    return Response(                                \
        cs::net::http::HTTP_403_PERMISSION_DENIED); \
  }

#define LOGGED_IN_USERS_ONLY(request)                  \
  if (!cs::www::app::ui::GetAuthenticatedUser(request) \
           .ok()) {                                    \
    return Response(                                   \
        cs::net::http::HTTP_403_PERMISSION_DENIED);    \
  }

#define ADMIN_USERS_ONLY(lhs, request)                   \
  lhs = cs::www::app::ui::GetAuthenticatedUser(request); \
  if (!user.ok() || !user.value().admin) {               \
    return Response(cs::net::http::HTTP_404_NOT_FOUND);  \
  }

// Source:
// https://chatgpt.com/share/67df4a37-ccf4-800e-bb0b-630599cd7766
std::string extractAuthToken(const std::string &cookie) {
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

ResultOr<std::string> GetToken(
    cs::net::http::Request request) {
  if (request.headers().count("Cookie") == 0) {
    return TRACE(NotFoundError("No cookie found."));
  }
  std::string cookie = request.headers()["Cookie"];
  std::string token_uuid = extractAuthToken(cookie);
  if (token_uuid.empty()) {
    return TRACE(
        NotFoundError("No auth_token found in cookie."));
  }
  return token_uuid;
}

ResultOr<User> GetAuthenticatedUser(Request request) {
  SET_OR_RET(std::string token_uuid,
             cs::www::app::ui::GetToken(request));

  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));
  JsonProtoDatabase<Token> tokens_db(DATA_DIR, "tokens");
  SET_OR_RET(
      auto token,
      tokens_db.query_view().then([&](auto query_view) {
        return query_view
            .where(LAMBDA(t, EQUALS(t.uuid, token_uuid)))
            .first();
      }));

  JsonProtoDatabase<cs::www::app::protos::User> users_db(
      DATA_DIR, "users");
  SET_OR_RET(
      auto user,
      users_db.query_view().then([&](auto query_view) {
        return query_view
            .where(
                LAMBDA(u, EQUALS(u.uuid, token.user_uuid)))
            .first();
      }));

  return user;
}

std::string MakePipe() {
  using namespace cs::net::html::dom;
  return span("&nbsp;|&nbsp;");
};

std::string MakeHeaderComponent(
    ResultOr<User> user, std::string active_page_path) {
  using namespace cs::net::html::dom;

  auto MakeLink = [&](std::string href, std::string text) {
    if (href == active_page_path) {
      return strong(text);
    } else {
      return a(href, text);
    }
  };

  std::vector<std::string> links;

  if (user.ok()) {
    if (user.value().admin) {
      // Admin user.
      links = {MakeLink("/", "Index"),
               MakeLink("/game/", "Game"),
               MakeLink("/cs/",
                        "Source Code (" + code("cs") + ")"),
               MakeLink("/home/", "Home"),
               MakeLink("/render/", "Render"),
               MakeLink("/logs/", "HTTP Logs"),
               MakeLink("/app_logs/", "App Logs"),
               MakeLink("/about/", "About"),
               MakeLink("/logout/", "Logout")};
    } else {
      // Regular logged-in user.
      links = {
          MakeLink("/", "Index"),
          MakeLink("/game/", "Game"),
          MakeLink("/cs/", "Code (" + code("cs") + ")"),
          MakeLink("/home/", "Home"),
          MakeLink("/render/", "Render"),
          MakeLink("/about/", "About"),
          MakeLink("/logout/", "Logout")};
    }
  } else {
    // Anonymous user.
    links = {MakeLink("/", "Index"),
             MakeLink("/game/", "Game"),
             MakeLink("/cs/", "Code (" + code("cs") + ")"),
             MakeLink("/render/", "Render"),
             MakeLink("/about/", "About"),
             MakeLink("/login/", "Login"),
             MakeLink("/register/", "Register")};
  }
  std::ostringstream ss;
  // Separate links with pipes.
  for (unsigned int i = 0; i < links.size(); i++) {
    ss << links[i];
    if (i < links.size() - 1) {
      ss << MakePipe();
    }
  }
  ss << hr();
  return nav(ss.str());
}

std::string MakeFooterComponent() {
  using namespace cs::net::html::dom;
  return div(
      hr(),
      div(p(code(cs::www::app::Context::Read("VERSION")
                     .value_or("v0")),
            nbsp(), "(",
            code(cs::www::app::Context::Read("COMMIT")
                     .value_or("commit")),
            ")", nbsp(), copy(), nbsp(), "2025&nbsp;",
            a("https://p13i.io/tweets/", "@p13i.io"))));
}

std::string MakeWebsite(ResultOr<User> user,
                        std::string active_page_path,
                        std::string content) {
  using namespace cs::net::html::dom;
  return cs::net::html::minify::MinifyHTML(html(
      head(
          meta({{"charset", "utf-8"}}),
          meta({{"http-equiv", "Content-Type"},
                {"content", "text/html; charset=utf-8"}}),
          R"html(<meta name="viewport" content="width=device-width, initial-scale=1">)html",
          title("C++ App"),
          style(std::string() +
#include "cs/www/app/ui/main.css.inc"
                )),
      body(MakeHeaderComponent(user, active_page_path),  //
           div(content),                                 //
           MakeFooterComponent())));
}

Response GetIndexPage(Request request) {
  using namespace cs::net::html::dom;
  return HtmlResponse(MakeWebsite(
      cs::www::app::ui::GetAuthenticatedUser(request), "/",
      div(h1("Welcome to my website!"), hr(),
          p("To code this app, I only used C++ standard "
            "library methods like",
            nbsp(), code("std::chrono"), nbsp(), "or",
            nbsp(), code("std::vector"), "."))));
}

Response GetGame(Request request) {
  using namespace cs::net::html::dom;
  return HtmlResponse(MakeWebsite(
      GetAuthenticatedUser(request), "/game/", R"html(
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
)html"));
}

Response GetHealthPage(Request request) { return cs::Ok(); }

#include <ostream>
#include <vector>

inline void WriteJson(
    const std::vector<
        std::vector<std::vector<std::vector<int>>>> &a,
    std::ostream &out) {
  out.put('[');
  for (size_t i = 0, n = a.size(); i < n; ++i) {
    if (i) out.put(',');
    out.put('[');
    const auto &b = a[i];
    for (size_t j = 0, m = b.size(); j < m; ++j) {
      if (j) out.put(',');
      out.put('[');
      const auto &c = b[j];
      for (size_t k = 0, p = c.size(); k < p; ++k) {
        if (k) out.put(',');
        out.put('[');
        const auto &d = c[k];
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

Response Render(Request request) {
  std::function<ResultOr<unsigned int>(std::string)>
      parse_unsigned_int = cs::parsers::ParseUnsignedInt;

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

  cs::www::app::protos::GetAnimationRequest
      get_animation_request;
  get_animation_request.width = width;
  get_animation_request.height = height;
  get_animation_request.num_frames = num_frames;

  const auto [get_animation_result, render_time_ms] =
      cs::util::timeit(cs::util::make_function([&]() {
        return cs::www::app::api::GetAnimationAPI().Process(
            get_animation_request);
      }));
  if (!get_animation_result.ok()) {
    return get_animation_result;
  }

  LOG(DEBUG) << "Constructing response..." << ENDL;

  cs::www::app::protos::GetAnimationResponse
      get_animation_response = get_animation_result.value();

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

  return HtmlResponse(MakeWebsite(
      cs::www::app::ui::GetAuthenticatedUser(request),
      "/render/", ss.str()));
}

Response GetLoginPage(Request request) {
  using namespace cs::net::html::dom;
  ANONYMOUS_USERS_ONLY(request);
  return HtmlResponse(MakeWebsite(
      cs::NotFoundError("ANONYMOUS_USERS_ONLY"), "/login/",
      div(h1("Login"), p("Login to your account."),
          R"html(
<form action="/login/" method="POST">
  <dl>
    <dt><label for="email">Email</label></dt>
    <dd><input type="email" id="email" name="email" required></dd>

    <dt><label for="password">Password</label></dt>
    <dd><input type="password" id="password" name="password" required></dd>
  </dl>
  <input type="submit" value="Login">
</form>
)html")));
}

Response PostLoginPage(Request request) {
  ANONYMOUS_USERS_ONLY(request);
  SET_OR_RET(LoginRequest login_request,
             cs::net::proto::api::ExtractProtoFromRequest<
                 LoginRequest>(request));

  SET_OR_RET(
      LoginResponse login_response,
      cs::www::app::api::LoginAPI().Process(login_request));

  return Response(
             cs::net::http::HTTP_302_FOUND,
             kContentTypeTextHtml,
             MakeWebsite(
                 cs::www::app::ui::GetAuthenticatedUser(
                     request),
                 "/home/",
                 "Logged in! Redirecting to <a "
                 "href=\"/home/\">home</a>..."))
      .SetHeader("Set-Cookie",
                 "auth_token=" + login_response.token.uuid +
                     "; HttpOnly; Secure; SameSite=Strict; "
                     "Path=/")
      .SetHeader("Location", "/home/");
}

Response GetLogoutPage(Request request) {
  LOGGED_IN_USERS_ONLY(request);
  using namespace cs::net::html::dom;
  return HtmlResponse(MakeWebsite(
      cs::www::app::ui::GetAuthenticatedUser(request),
      "/logout/",
      div(h1("Logout"),
          p("Are you sure you want to log out?"),
          R"html(<form action="/logout/" method="POST">
  <input type="submit" value="Logout">
</form>)html")));
}

Response PostLogoutPage(Request request) {
  LOGGED_IN_USERS_ONLY(request);
  auto token = cs::www::app::ui::GetToken(request);
  std::string email = "";
  if (!token.ok() && !Result::IsNotFound(token)) {
    LOG(WARNING) << "Token not found in request." << ENDL;
    return token;
  } else if (token.ok()) {
    LogoutRequest logout_request;
    logout_request.token_uuid = token.value();
    auto logout_response =
        cs::www::app::api::LogoutAPI().Process(
            logout_request);
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

  using namespace cs::net::html::dom;
  return HtmlResponse(
             MakeWebsite(
                 cs::www::app::ui::GetAuthenticatedUser(
                     request),
                 "/logout/",
                 div(h1("Logged out" + email),
                     p("Go to " + a("/", "main page") +
                       "."))))
      .SetHeader("Set-Cookie",
                 "auth_token=; HttpOnly; Secure; "
                 "SameSite=Strict; "
                 "Path=/; Expires=Thu, 01 Jan 1970 "
                 "00:00:00 GMT");
}

// Macro to check whether a vector contains a value. Uses
// std::find so it works for any vector element type that
// supports operator==.
#define VECTOR_CONTAINS(vec, val)                  \
  (std::find((vec).begin(), (vec).end(), (val)) != \
   (vec).end())
// Evaluate AccessControlList for a given principal and
// resource. Returns true if access is allowed, false
// otherwise.
static bool CheckACL(
    const cs::www::app::protos::AccessControlList &acl,
    const std::string &principal,
    const std::string &resource) {
  LOG(DEBUG) << "CheckACL(acl=" << acl.Serialize()
             << ", principal=" << principal
             << ", resource=" << resource << ")" << ENDL;
  for (const auto &policy : acl.policies) {
    bool principal_match =
        !policy.principals.empty() &&
        (VECTOR_CONTAINS(policy.principals, principal) ||
         VECTOR_CONTAINS(policy.principals, "*"));
    LOG(DEBUG) << "principal_match=" << principal_match
               << ENDL;

    bool resource_match =
        !policy.resources.empty() &&
        (VECTOR_CONTAINS(policy.resources, resource) ||
         VECTOR_CONTAINS(policy.resources, "*"));
    LOG(DEBUG) << "resource_match=" << resource_match
               << ENDL;

    if (principal_match && resource_match) {
      LOG(DEBUG) << "policy.allow=" << policy.allow << ENDL;
      return policy.allow;
    }
  }
  return false;
}

static std::string EscapeForHtml(const std::string &s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    switch (c) {
      case '&':
        out += "&amp;";
        break;
      case '<':
        out += "&lt;";
        break;
      case '>':
        out += "&gt;";
        break;
      case '"':
        out += "&quot;";
        break;
      case '\'':
        out += "&#39;";
        break;
      default:
        out += c;
        break;
    }
  }
  return out;
}

static std::string UrlEncode(const std::string &s) {
  std::ostringstream oss;
  oss << std::hex;
  for (unsigned char c : s) {
    if (std::isalnum(c) || c == '-' || c == '.' ||
        c == '_' || c == '~' || c == '/') {
      oss << c;
    } else {
      oss << '%' << std::uppercase << std::setw(2)
          << std::setfill('0') << int(c)
          << std::nouppercase;
      oss << std::dec;
    }
  }
  return oss.str();
}

static bool IsSubpath(const std::filesystem::path &base,
                      const std::filesystem::path &child) {
  auto bi = base.begin(), be = base.end();
  auto ci = child.begin(), ce = child.end();
  for (; bi != be && ci != ce; ++bi, ++ci) {
    if (*bi != *ci) return false;
  }
  return bi == be;  // all components of base matched
}

// Build breadcrumb navigation from a relative path
static std::string MakeBreadcrumbs(
    const std::string &rel_path) {
  using namespace ::cs::net::html::dom;
  std::ostringstream html;
  std::filesystem::path p =
      std::filesystem::path(rel_path).lexically_normal();

  html << "<nav class=\"breadcrumbs\">";

  std::string accum;
  for (auto it = p.begin(); it != p.end(); ++it) {
    accum = (std::filesystem::path(accum) / *it)
                .lexically_normal()
                .string();
    html << code("&nbsp;/&nbsp;");
    html << "<a href=\"/" << UrlEncode(accum) << "\">"
         << code(EscapeForHtml(it->string())) << "</a>";
  }

  html << "</nav>\n";
  return html.str();
}

cs::ResultOr<std::string> RenderHtmlForSourceCodePath(
    const std::string &relative_path) {
  std::ostringstream html;

  SET_OR_RET(std::string base_dir,
             cs::www::app::Context::Read("REPO_DIR"));

  std::filesystem::path base =
      std::filesystem::absolute(base_dir);
  LOG(DEBUG) << "base=" << base << ENDL;
  std::filesystem::path target =
      base / std::filesystem::path(relative_path);

  LOG(DEBUG) << "target=" << target << ENDL;

  std::error_code ec;
  std::filesystem::path base_canon =
      std::filesystem::weakly_canonical(base, ec);
  if (ec) base_canon = std::filesystem::absolute(base);
  LOG(DEBUG) << "base_canon=" << base_canon << ENDL;

  std::filesystem::path target_canon =
      std::filesystem::weakly_canonical(target, ec);
  if (ec)
    target_canon = std::filesystem::absolute(target)
                       .lexically_normal();
  LOG(DEBUG) << "target_canon=" << target_canon << ENDL;

  if (!IsSubpath(base_canon, target_canon)) {
    return TRACE(cs::PermissionDenied(
        "Access denied: Requested path is outside of the "
        "permitted folder."));
  }

  if (!std::filesystem::exists(target_canon)) {
    return TRACE(cs::NotFoundError(
        "Not found: Path does not exist, relative_path=" +
        EscapeForHtml(relative_path)));
  }

  static const std::string acl_json = "acl.json";
  std::filesystem::path policy_path =
      std::filesystem::is_directory(target_canon)
          ? target_canon / acl_json
          : target_canon.parent_path() / acl_json;
  LOG(DEBUG) << "policy_path=" << policy_path << ENDL;

  std::error_code ec_policy;
  if (!std::filesystem::exists(policy_path, ec_policy)) {
    return TRACE(cs::NotFoundError(
        "Policy file not found in the "
        "enclosing directory, "
        "policy_path=" +
        std::filesystem::relative(policy_path, base_canon)
            .string()));
  }

  SET_OR_RET(std::string acl_contents,
             cs::fs::read(policy_path.string()));
  SET_OR_RET(
      auto acl,
      cs::www::app::protos::AccessControlList().Parse(
          acl_contents));

  bool allowed = false;
  if (std::filesystem::is_directory(target_canon)) {
    for (std::filesystem::directory_entry de :
         std::filesystem::directory_iterator(
             target_canon)) {
      auto target_path =
          std::filesystem::relative(de.path(), base_canon);
      if (CheckACL(acl, "*", target_path.string())) {
        allowed = true;
        break;
      }
    }
  } else if (std::filesystem::is_regular_file(
                 target_canon)) {
    std::string filename =
        std::filesystem::relative(target_canon, base_canon)
            .string();
    LOG(DEBUG)
        << "From regular file, checking ACL for filename="
        << filename << "." << ENDL;
    allowed = CheckACL(acl, "*", filename);
    if (!CheckACL(acl, "*", filename)) {
      return cs::PermissionDenied(
          "Permission denied to regular file, filename=" +
          filename);
    }
  }

  if (!allowed) {
    return TRACE(cs::PermissionDenied(
        "Access denied to target=" +
        std::filesystem::relative(target_canon, base_canon)
            .string() +
        " per policy at "
        "policy_path=" +
        std::filesystem::relative(policy_path, base_canon)
            .string()));
  }

  using namespace ::cs::net::html::dom;

  // Static style section in a raw string
  if (std::filesystem::is_directory(target_canon)) {
    html << MakeBreadcrumbs(
                relative_path.empty() ? "." : relative_path)
         << hr();

    std::filesystem::path rel_p(relative_path);
    if (!rel_p.empty() && rel_p != ".") {
      std::filesystem::path parent = rel_p.parent_path();
      std::string parent_str = parent.string();
      if (parent_str.empty()) parent_str = ".";
      parent_str = "/" + parent_str;
      parent_str = UrlEncode(parent_str);
      LOG(DEBUG) << "parent_str=" << parent_str << ENDL;
      html << R"html(<div class="entry"><a href=")html"
           << parent_str << R"html(">[.. parent]</a></div>
)html" << hr();
    }

    std::vector<std::filesystem::directory_entry> entries;
    for (auto &de :
         std::filesystem::directory_iterator(target_canon))
      entries.push_back(de);
    std::sort(entries.begin(), entries.end(),
              [](auto &a, auto &b) {
                return a.path().filename().string() <
                       b.path().filename().string();
              });

    html << R"html(<ul class="listing">
)html";
    for (auto &de : entries) {
      auto target_path =
          std::filesystem::relative(de.path(), base_canon);
      if (!CheckACL(acl, "*", target_path)) {
        LOG(WARNING) << "Principal * doesn't have access "
                        "to target_path="
                     << target_path << ENDL;
        continue;
      }
      std::string name = de.path().filename().string();
      std::string display = EscapeForHtml(
          name + (de.is_directory() ? "/" : ""));
      if (display[0] == '.') continue;

      std::filesystem::path link_path =
          std::filesystem::path(relative_path) /
          std::filesystem::path(name);
      std::string link_str =
          "/" + link_path.lexically_normal().string();
      link_str = UrlEncode(link_str);

      html << R"html(<li class="entry"><a href=")html"
           << link_str << R"html(">)html" << code(display)
           << R"html(</a>)html";
      if (de.is_regular_file()) {
        std::error_code e2;
        auto sz = de.file_size(e2);
        if (!e2)
          html << R"html(&nbsp;<span class="meta">()html"
               << sz << R"html(&nbsp;bytes)</span>)html";
      }
      html << R"html(</li>)html";
    }
    html << R"html(</ul>)html";
    return html.str();
  }

  if (std::filesystem::is_regular_file(target_canon)) {
    std::ifstream ifs(target_canon, std::ios::binary);
    if (!ifs) {
      html
          << R"html(<h2>Error</h2><p>Failed to open file.</p></body></html>)html";
      return html.str();
    }

    std::vector<std::string> lines;
    std::string raw;
    const std::size_t probeSize = 4096;
    std::string probe;
    probe.resize(probeSize);
    ifs.read(&probe[0], (std::streamsize)probeSize);
    std::streamsize readBytes = ifs.gcount();
    bool binary = false;
    for (std::streamsize i = 0; i < readBytes; ++i) {
      unsigned char c =
          static_cast<unsigned char>(probe[i]);
      if (c == 0) {
        binary = true;
        break;
      }
    }
    ifs.clear();
    ifs.seekg(0);
    if (binary)
      return TRACE(
          cs::Error("Cannot render binary file: The "
                    "requested file appears to be binary "
                    "and cannot be rendered as text."));

    while (std::getline(ifs, raw)) {
      if (!raw.empty() && raw.back() == '\r')
        raw.pop_back();
      lines.push_back(raw);
    }

    size_t nlines = lines.size();
    size_t width =
        std::to_string(std::max<size_t>(1, nlines)).size();

    std::ostringstream ln_ss, code_ss;
    ln_ss << R"html(<pre class="code_pre">)html";
    code_ss << R"html(<pre class="code_pre">)html";

    for (size_t i = 0; i < nlines; ++i) {
      ln_ss << std::setw((int)width) << std::setfill(' ')
            << (i + 1) << '\n';
      code_ss << EscapeForHtml(lines[i]) << '\n';
    }

    if (nlines == 0) {
      ln_ss << std::setw((int)width) << 1 << '\n';
      code_ss << '\n';
    }

    ln_ss << R"html(</pre>)html";
    code_ss << R"html(</pre>)html";

    html << MakeBreadcrumbs(
        relative_path.empty()
            ? target_canon.filename().string()
            : relative_path);

    std::error_code e3;
    auto fsize =
        std::filesystem::file_size(target_canon, e3);
    html << hr();
    html << "<p>"
         << a("https://github.com/p13i/cs/blob/main/" +
                  relative_path,
              "View on Github");
    if (!e3)
      html << R"html(<span class="meta">)html" << MakePipe()
           << "Size:&nbsp;" << fsize
           << R"html( bytes.</span>)html";
    html << "</p>";
    html << hr();
    html << R"html(
<div class="code_block">
  <div class="ln_col">)html"
         << ln_ss.str() << R"html(</div>
  <div class="code_col">)html"
         << code_ss.str() << R"html(</div>
</div>
)html";

    html << R"html(
</body>
</html>
)html";
    return html.str();
  }

  html << R"html(<h2>Unsupported entry type</h2>
<p>Path refers to an unsupported filesystem object.</p>
</body></html>)html";
  return html.str();
}

Response GetHomePage(Request request) {
  LOGGED_IN_USERS_ONLY(request);

  auto user =
      cs::www::app::ui::GetAuthenticatedUser(request);

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
    SET_OR_RET(
        cs::www::app::protos::Context context,
        Context::Read("APP_CONTEXT").then([&](auto s) {
          return cs::www::app::protos::Context().Parse(s);
        }));
    for (auto route : context.routes) {
      ss << "<li><code>" << route.method
         << "&nbsp;<a href=\"" << route.path << "\">"
         << route.path << "</a></code></li>";
    }
    ss << "</ul>";
  }
  return HtmlResponse(
      MakeWebsite(user, "/home/", ss.str()));
}

Response GetLogsPage(Request request) {
  ADMIN_USERS_ONLY(auto user, request);

  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

  SET_OR_RET(
      cs::www::app::protos::ListLogsResponse response,
      cs::www::app::api::ListLogsAPI().Process(
          cs::www::app::protos::ListLogsRequest()));

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

  for (const auto &log : response.http_logs) {
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
  return HtmlResponse(
      MakeWebsite(user, "/logs/", ss.str()));
}

Response GetAppLogsPage(Request request) {
  ADMIN_USERS_ONLY(auto user, request);

  SET_OR_RET(std::string DATA_DIR,
             Context::Read("DATA_DIR"));

  SET_OR_RET(
      cs::www::app::protos::ListAppLogsResponse response,
      cs::www::app::api::ListAppLogsAPI().Process(
          cs::www::app::protos::ListAppLogsRequest()));

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

  for (const auto &log : response.items) {
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
  return HtmlResponse(
      MakeWebsite(user, "/app/logs/", ss.str()));
}

cs::net::http::Response GetRegisterPage(
    cs::net::http::Request request) {
  ANONYMOUS_USERS_ONLY(request);
  return HtmlResponse(
      MakeWebsite(cs::NotFoundError("ANONYMOUS_USERS_ONLY"),
                  "/register/",
                  R"html(
<h1>Register</h1>
<form action="/register/" method="POST">
  <dl>
    <dt><label for="email">Email:</label></dt>
    <dd><input type="email" id="email" name="email" required></dd>

    <dt><label for="password">Password:</label></dt>
    <dd><input type="password" id="password" name="password" required></dd>

    <dt><label for="confirm_password">Confirm Password:</label></dt>
    <dd><input type="password" id="confirm_password" name="confirm_password" required></dd>
  </dl>

  <button type="submit">Create Account</button>
</form>
)html"));
}

Response GetQuitPage(Request request) {
  ADMIN_USERS_ONLY(auto user, request);
  // Flush buffer with endl
  LOG(FATAL) << "Quitting app." << ENDL;
  exit(-1);
}

cs::net::http::Response PostRegisterPage(
    cs::net::http::Request request) {
  ANONYMOUS_USERS_ONLY(request);
  SET_OR_RET(
      cs::www::app::protos::CreateUserRequest request_proto,
      cs::net::proto::api::ExtractProtoFromRequest<
          cs::www::app::protos::CreateUserRequest>(
          request));
  SET_OR_RET(auto create_user_response,
             cs::www::app::api::CreateUserAPI().Process(
                 request_proto));
  return PostLoginPage(request);
}

cs::net::http::Response ApiListLogs(
    cs::net::http::Request request) {
  ADMIN_USERS_ONLY(auto user, request);
  SET_OR_RET(
      cs::www::app::protos::ListLogsRequest request_proto,
      cs::net::proto::api::ExtractProtoFromRequest<
          cs::www::app::protos::ListLogsRequest>(request));
  SET_OR_RET(auto response,
             cs::www::app::api::ListLogsAPI().Process(
                 request_proto));
  return Response(
      cs::net::http::HTTP_200_OK,
      cs::net::http::kContentTypeApplicationJson,
      response.Serialize());
}

cs::net::http::Response GetCodePage(Request request) {
  SET_OR_RET(std::string path, cs::parsers::ConsumePrefix(
                                   "/cs", request.path()));

  if (path.empty()) {
    path = "cs";
  } else {
    if (path.front() == '/') {
      path.insert(0, "cs");
    } else {
      path.insert(0, "cs/");
    }
  }

  SET_OR_RET(std::string html,
             RenderHtmlForSourceCodePath(path));
  return cs::net::http::HtmlResponse(MakeWebsite(
      GetAuthenticatedUser(request), "/cs/", html));
}

cs::net::http::Response GetNewWebsite(Request request) {
  return cs::net::http::HtmlResponse(
      cs::www::app::ui::MakeWebsite(
          GetAuthenticatedUser(request), "/new/",
          R"html(
<h1>New Website</h1>
<p>This is a new website!</p>
)html"));
}

cs::net::http::Response GetAboutPage(Request request) {
  using namespace ::cs::net::html::dom;
  return cs::net::http::HtmlResponse(
      cs::www::app::ui::MakeWebsite(
          GetAuthenticatedUser(request), "/about/",
          div(h1("About" + nbsp() + code("cs")), hr(),
              p("This web application is handled by a C++ "
                "framework made only using standard "
                "library methods, with no external "
                "includes (except curl because who wants "
                "to "
                "work directly with TLS, yikes)."),
              p("This framework also supports a protocol "
                "buffer implemention for JSON over HTTP. "
                "This repo also includes code for a "
                "raytracer, technical interview questions, "
                "a protobuf-based async database; a "
                "website archiver (to be deployed at",
                nbsp(),
                a("https://cite.pub", code("cite.pub")),
                "); all in standard C++."),
              p(R"(Sharing demos of C-based programs is 
                difficult because the code needs to be 
                compiled for the host architecture. Demos 
                in web apps, however, only require a user 
                to open a website in their browser. This 
                project includes a proof-of-concept 
                implementation of WebAssembly running a 
                C++ program in the browser (via Emscripten 
                and with Bazel). Such a framework will 
                allow me to share great demos in the 
                future.)"),
              p("-Pramod"))));
}

}  // namespace cs::www::app::ui

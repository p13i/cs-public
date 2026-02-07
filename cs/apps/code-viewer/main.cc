// cs/apps/code-viewer/main.cc
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "cs/apps/code-viewer/ui.gpt.hh"
#include "cs/apps/common/file-viewer/protos/config.proto.hh"
#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/common/load_endpoint.gpt.hh"
#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/load/balancer/protos/downstream.proto.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::code_viewer::ui::GetCodePage;
using ::cs::apps::common::GetHealth;
using ::cs::apps::common::GetLoad;
using ::cs::apps::common::file_viewer::protos::Config;
using ::cs::net::http::Request;
using ::cs::net::http::WebApp;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::Context;
}  // namespace

Result Run(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  ::cs::util::Context::Write("VERSION", config.version);
  ::cs::util::Context::Write("COMMIT", config.commit);

  WebApp app;

  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  ADD_ROUTE_CAPTURE(
      app, "GET", "/load/",
      [&app](Request r, ::cs::util::di::Context<>& c) {
        return GetLoad(
            r, c, [&app] { return app.GetThreadLoad(); });
      });

  ADD_ROUTE(app, "GET", "/*", GetCodePage);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, Run);
}

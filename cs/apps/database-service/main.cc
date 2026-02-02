// cs/apps/database-service/main.cc
#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/database-service/protos/config.proto.hh"
#include "cs/apps/database-service/rpc.gpt.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/web_app.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::common::GetHealth;
using ::cs::apps::database_service::protos::Config;
using ::cs::apps::database_service::rpc::QueryRPC;
using ::cs::apps::database_service::rpc::UpsertRPC;
using ::cs::fs::dir_exists;
using ::cs::fs::mkdir;
using ::cs::net::http::WebApp;
using ::cs::parsers::ParseArgs;
using ::cs::util::Context;
}  // namespace

Result RunDatabaseService(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  // Set DATA_DIR in context for RPC handlers to use.
  Context::Write("DATA_DIR", config.data_dir);

  if (!dir_exists(config.data_dir)) {
    OK_OR_RET(mkdir(config.data_dir));
  }

  WebApp app;
  ADD_ROUTE(app, "POST", "/rpc/query/",
            QueryRPC::HttpHandler);
  ADD_ROUTE(app, "POST", "/rpc/upsert/",
            UpsertRPC::HttpHandler);
  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunDatabaseService);
}

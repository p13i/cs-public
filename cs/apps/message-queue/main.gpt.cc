// cs/apps/message-queue/main.gpt.cc
#include <memory>
#include <string>
#include <vector>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/message-queue/protos/config.proto.hh"
#include "cs/apps/message-queue/protos/gencode/config.validate.hh"
#include "cs/apps/message-queue/rpc.gpt.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::common::GetHealth;
using ::cs::apps::message_queue::protos::Config;
using ::cs::apps::message_queue::protos::gencode::config::
    validation_generated::ConfigRules;
using ::cs::apps::message_queue::rpc::DequeueRPC;
using ::cs::apps::message_queue::rpc::EnqueueRPC;
using ::cs::net::http::WebApp;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
}  // namespace

namespace {  // helpers
using AppContext = Context<IDatabaseClient>;
}  // namespace

Result RunMessageQueueService(
    std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));
  OK_OR_RET(Validate(config, ConfigRules{}));

  auto app_ctx =
      ContextBuilder<AppContext>()
          .bind<IDatabaseClient>()
          .from([](AppContext&) {
            return std::make_shared<DatabaseClientImpl>(
                "http://database-service:8080");
          })
          .build();

  WebApp<AppContext> app(app_ctx);

  ADD_ROUTE(app, "POST", "/rpc/enqueue/",
            EnqueueRPC::HttpHandler<AppContext>);
  ADD_ROUTE(app, "POST", "/rpc/dequeue/",
            DequeueRPC::HttpHandler<AppContext>);
  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunMessageQueueService);
}

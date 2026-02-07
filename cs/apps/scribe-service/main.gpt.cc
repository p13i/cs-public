// cs/apps/scribe-service/main.gpt.cc
#include <memory>
#include <string>

#include "cs/apps/common/health_endpoint.hh"
#include "cs/apps/scribe-service/protos/config.proto.hh"
#include "cs/apps/scribe-service/protos/gencode/config.validate.hh"
#include "cs/apps/scribe-service/scribe_context.gpt.hh"
#include "cs/apps/scribe-service/ui.gpt.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::common::GetHealth;
using ::cs::apps::scribe_service::GetJobPage;
using ::cs::apps::scribe_service::GetJobTranscript;
using ::cs::apps::scribe_service::GetUploadPage;
using ::cs::apps::scribe_service::PostUpload;
using ::cs::apps::scribe_service::ScribeContext;
using ::cs::apps::scribe_service::protos::Config;
using ::cs::apps::scribe_service::protos::gencode::config::
    validation_generated::ConfigRules;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
}  // namespace

namespace {  // impl
Response HandleGetJob(Request r, ScribeContext& c) {
  if (r.path().size() > 10 &&
      r.path().compare(r.path().size() - 10, 10,
                       "/transcript") == 0) {
    return GetJobTranscript(r, c);
  }
  return GetJobPage(r, c);
}
}  // namespace

Result RunScribeService(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));
  OK_OR_RET(Validate(config, ConfigRules{}));

  ScribeContext ctx;
  std::string database_base_url =
      config.database_base_url.empty()
          ? "http://database-service:8080"
          : config.database_base_url;
  ctx.database = std::make_shared<DatabaseClientImpl>(
      database_base_url);
  ctx.blob_base_url = config.blob_base_url.empty()
                          ? "http://blob-service:8080"
                          : config.blob_base_url;
  ctx.task_queue_base_url =
      config.task_queue_base_url.empty()
          ? "http://message-queue:8080"
          : config.task_queue_base_url;

  WebApp<ScribeContext> app(ctx);

  ADD_ROUTE(app, "GET", "/", GetUploadPage);
  ADD_ROUTE(app, "POST", "/upload/", PostUpload);
  ADD_ROUTE_CAPTURE(app, "GET", "/job/*", HandleGetJob);
  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunScribeService);
}

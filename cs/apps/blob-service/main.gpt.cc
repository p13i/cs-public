// cs/apps/blob-service/main.gpt.cc
#include <memory>
#include <string>
#include <vector>

#include "cs/apps/blob-service/blob_store.gpt.hh"
#include "cs/apps/blob-service/protos/blob.proto.hh"
#include "cs/apps/blob-service/protos/config.proto.hh"
#include "cs/apps/blob-service/protos/gencode/config.validate.hh"
#include "cs/apps/blob-service/rpc.gpt.hh"
#include "cs/apps/common/health_endpoint.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/proto/db/client.gpt.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"
#include "cs/util/di/context.gpt.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::blob_service::BlobStore;
using ::cs::apps::blob_service::protos::Config;
using ::cs::apps::blob_service::protos::gencode::config::
    validation_generated::ConfigRules;
using ::cs::apps::blob_service::rpc::GetBlobMetadataRPC;
using ::cs::apps::blob_service::rpc::GetBlobRPC;
using ::cs::apps::blob_service::rpc::UpsertBlobRPC;
using ::cs::apps::common::GetHealth;
using ::cs::net::http::WebApp;
using ::cs::net::proto::db::DatabaseClientImpl;
using ::cs::net::proto::db::IDatabaseClient;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
using ::cs::util::di::Context;
using ::cs::util::di::ContextBuilder;
}  // namespace

namespace {  // helpers
using AppContext = Context<IDatabaseClient, BlobStore>;
}  // namespace

Result RunBlobService(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));
  OK_OR_RET(Validate(config, ConfigRules{}));

  auto blob_store = std::make_shared<BlobStore>("/data");

  auto app_ctx =
      ContextBuilder<AppContext>()
          .bind<IDatabaseClient>()
          .from([](AppContext&) {
            return std::make_shared<DatabaseClientImpl>(
                "http://database-service:8080");
          })
          .bind<BlobStore>()
          .from([&blob_store](AppContext&) {
            return blob_store;
          })
          .build();

  WebApp<AppContext> app(app_ctx);

  ADD_ROUTE(app, "POST", "/rpc/blob/upsert/",
            UpsertBlobRPC::HttpHandler<AppContext>);
  ADD_ROUTE(app, "POST", "/rpc/blob/get/",
            GetBlobRPC::HttpHandler<AppContext>);
  ADD_ROUTE(app, "POST", "/rpc/blob/metadata/",
            GetBlobMetadataRPC::HttpHandler<AppContext>);
  ADD_ROUTE(app, "GET", "/health/", GetHealth);

  return app.RunServer(config.host, config.port);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunBlobService);
}

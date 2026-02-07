// cs/apps/common/domain_resolver_test.gpt.cc
#include <string>

#include "cs/apps/common/routing_embedded_gen_embed.h"
#include "cs/net/load/balancer/protos/config.proto.hh"
#include "cs/result.hh"
#include "cs/util/embedded_files.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::net::load::balancer::protos::Config;
using ::cs::util::EmbeddedFileEntry;
using ::cs::util::FindEmbedded;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::IsTrue;
using ::testing::StrEq;
}  // namespace

namespace cs::apps::common {
namespace {

TEST(DomainResolverTest,
     LoadRoutingJsonParseProtoAssertFields) {
  auto json_or = FindEmbedded(
      "routing.json",
      reinterpret_cast<const EmbeddedFileEntry*>(
          kEmbeddedFiles_routing),
      kEmbeddedFilesCount_routing);
  ASSERT_THAT(json_or.ok(), IsTrue()) << json_or.message();

  auto config_or = Config().Parse(json_or.value());
  ASSERT_THAT(config_or.ok(), IsTrue())
      << config_or.message();
  const Config& config = config_or.value();

  ASSERT_THAT(config.prod.routes.size(), Ge(5))
      << "prod routes must include www, code, data, cite, "
         "scribe";
  ASSERT_THAT(config.prod.routes.at("www.trycopilot.ai"),
              StrEq("www-trycopilot-ai"));
  ASSERT_THAT(config.prod.routes.at("code.trycopilot.ai"),
              StrEq("code-viewer"));
  ASSERT_THAT(config.prod.routes.at("data.trycopilot.ai"),
              StrEq("data-viewer"));
  ASSERT_THAT(config.prod.routes.at("www.cite.pub"),
              StrEq("www-cite-pub"));
  ASSERT_THAT(config.prod.routes.at("scribe.trycopilot.ai"),
              StrEq("scribe-service"));

  ASSERT_THAT(config.dev.routes.size(), Ge(5))
      << "dev routes must include dev.www, dev.code, "
         "dev.data, dev.cite, dev.scribe";
  ASSERT_THAT(config.dev.routes.at("dev.www.trycopilot.ai"),
              StrEq("www-trycopilot-ai"));
  ASSERT_THAT(
      config.dev.routes.at("dev.code.trycopilot.ai"),
      StrEq("code-viewer"));
  ASSERT_THAT(
      config.dev.routes.at("dev.data.trycopilot.ai"),
      StrEq("data-viewer"));
  ASSERT_THAT(config.dev.routes.at("dev.www.cite.pub"),
              StrEq("www-cite-pub"));
  ASSERT_THAT(
      config.dev.routes.at("dev.scribe.trycopilot.ai"),
      StrEq("scribe-service"));
}

}  // namespace
}  // namespace cs::apps::common

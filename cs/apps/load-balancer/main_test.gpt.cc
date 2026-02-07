// cs/apps/load-balancer/main_test.gpt.cc
#include <string>
#include <vector>

#include "cs/net/load/balancer/protos/config.proto.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::net::load::balancer::protos::Config;
using ::cs::parsers::ParseArgs;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::StrEq;
}  // namespace

class LoadBalancerArgParserTest : public ::testing::Test {};

TEST_F(LoadBalancerArgParserTest, ParseAllFields) {
  std::vector<std::string> argv = {
      "program", "--host=0.0.0.0", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("0.0.0.0"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(LoadBalancerArgParserTest, ParseOnlyHost) {
  std::vector<std::string> argv = {"program",
                                   "--host=localhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(0));
}

TEST_F(LoadBalancerArgParserTest, ParseOnlyPort) {
  std::vector<std::string> argv = {"program",
                                   "--port=9000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(9000));
}

TEST_F(LoadBalancerArgParserTest, ParseHostAndPort) {
  std::vector<std::string> argv = {
      "program", "--host=192.168.1.1", "--port=5000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("192.168.1.1"));
  ASSERT_THAT(config.port, Eq(5000));
}

TEST_F(LoadBalancerArgParserTest, MissingFieldUsesDefault) {
  std::vector<std::string> argv = {"program"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(0));
}

TEST_F(LoadBalancerArgParserTest, InvalidPortValue) {
  std::vector<std::string> argv = {"program",
                                   "--port=invalid"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Didn't find int in string"));
}

TEST_F(LoadBalancerArgParserTest, NegativePortValue) {
  std::vector<std::string> argv = {"program", "--port=-1"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.port, Eq(-1));
}

TEST_F(LoadBalancerArgParserTest, MissingEqualsSign) {
  std::vector<std::string> argv = {"program", "--port8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr(
          "Argument must be in format --field=value"));
}

TEST_F(LoadBalancerArgParserTest, UnknownField) {
  std::vector<std::string> argv = {"program",
                                   "--unknown_field=value"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(result.message(),
              ::testing::HasSubstr(
                  "Unknown field: --unknown_field"));
}

TEST_F(LoadBalancerArgParserTest, EmptyArgv) {
  std::vector<std::string> argv = {};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(result.message(),
              ::testing::HasSubstr("argv cannot be empty"));
}

TEST_F(LoadBalancerArgParserTest,
       ArgumentNotStartingWithDoubleDash) {
  std::vector<std::string> argv = {"program",
                                   "host=localhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Argument must start with --"));
}

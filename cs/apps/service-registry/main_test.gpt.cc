// cs/apps/service-registry/main_test.gpt.cc
#include <string>
#include <vector>

#include "cs/apps/service-registry/protos/config.proto.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::service_registry::protos::Config;
using ::cs::parsers::ParseArgs;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::StrEq;
}  // namespace

class ServiceRegistryArgParserTest
    : public ::testing::Test {};

// Valid parsing tests

TEST_F(ServiceRegistryArgParserTest, ParseAllFields) {
  std::vector<std::string> argv = {
      "program", "--host=0.0.0.0", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("0.0.0.0"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest, ParseOnlyHost) {
  std::vector<std::string> argv = {"program",
                                   "--host=localhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(0));
}

TEST_F(ServiceRegistryArgParserTest, ParseOnlyPort) {
  std::vector<std::string> argv = {"program",
                                   "--port=9000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(9000));
}

TEST_F(ServiceRegistryArgParserTest, ParseHostAndPort) {
  std::vector<std::string> argv = {
      "program", "--host=192.168.1.1", "--port=5000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("192.168.1.1"));
  ASSERT_THAT(config.port, Eq(5000));
}

TEST_F(ServiceRegistryArgParserTest,
       ParseFieldsInReverseOrder) {
  std::vector<std::string> argv = {"program", "--port=3000",
                                   "--host=127.0.0.1"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("127.0.0.1"));
  ASSERT_THAT(config.port, Eq(3000));
}

TEST_F(ServiceRegistryArgParserTest,
       MissingFieldUsesDefault) {
  std::vector<std::string> argv = {"program"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(0));
}

// Edge cases for host field

TEST_F(ServiceRegistryArgParserTest, ParseEmptyHost) {
  std::vector<std::string> argv = {
      "program", "--host=", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       ParseHostWithSpecialCharacters) {
  std::vector<std::string> argv = {
      "program", "--host=example.com:8080", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("example.com:8080"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       ParseHostWithUnderscores) {
  std::vector<std::string> argv = {
      "program", "--host=my_service.example.com",
      "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("my_service.example.com"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest, ParseHostWithHyphens) {
  std::vector<std::string> argv = {
      "program", "--host=my-service.example.com",
      "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("my-service.example.com"));
  ASSERT_THAT(config.port, Eq(8080));
}

// Edge cases for port field

TEST_F(ServiceRegistryArgParserTest, ParseZeroPort) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=0"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(0));
}

TEST_F(ServiceRegistryArgParserTest, ParseNegativePort) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=-1"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(-1));
}

TEST_F(ServiceRegistryArgParserTest, ParseLargePort) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=65535"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(65535));
}

TEST_F(ServiceRegistryArgParserTest, ParseVeryLargePort) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=2147483647"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(2147483647));
}

// Invalid input tests

TEST_F(ServiceRegistryArgParserTest, InvalidPortValue) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=invalid"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Didn't find int in string"));
}

TEST_F(ServiceRegistryArgParserTest, PortWithDecimal) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=8080.5"};
  auto result = ParseArgs<Config>(argv);
  // Parser may accept decimal and truncate, or reject it
  // This test verifies behavior - adjust expectation based
  // on actual
  if (result.ok()) {
    Config config = result.value();
    // If accepted, it should truncate to 8080
    ASSERT_THAT(config.port, Eq(8080));
  } else {
    ASSERT_THAT(result.message(),
                ::testing::AnyOf(
                    ::testing::HasSubstr(
                        "Didn't find int in string"),
                    ::testing::HasSubstr("CheckInBounds")));
  }
}

TEST_F(ServiceRegistryArgParserTest, PortWithLeadingZeros) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=008080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest, MissingEqualsSign) {
  std::vector<std::string> argv = {"program", "--port8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr(
          "Argument must be in format --field=value"));
}

TEST_F(ServiceRegistryArgParserTest,
       MissingEqualsSignForHost) {
  std::vector<std::string> argv = {"program",
                                   "--hostlocalhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr(
          "Argument must be in format --field=value"));
}

TEST_F(ServiceRegistryArgParserTest, UnknownField) {
  std::vector<std::string> argv = {"program",
                                   "--unknown_field=value"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(result.message(),
              ::testing::HasSubstr(
                  "Unknown field: --unknown_field"));
}

TEST_F(ServiceRegistryArgParserTest,
       UnknownFieldWithKnownFields) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=8080",
      "--unknown_field=value"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(result.message(),
              ::testing::HasSubstr(
                  "Unknown field: --unknown_field"));
}

TEST_F(ServiceRegistryArgParserTest, EmptyArgv) {
  std::vector<std::string> argv = {};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(result.message(),
              ::testing::HasSubstr("argv cannot be empty"));
}

TEST_F(ServiceRegistryArgParserTest,
       ArgumentNotStartingWithDoubleDash) {
  std::vector<std::string> argv = {"program",
                                   "host=localhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Argument must start with --"));
}

TEST_F(ServiceRegistryArgParserTest, SingleDashArgument) {
  std::vector<std::string> argv = {"program",
                                   "-host=localhost"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Argument must start with --"));
}

TEST_F(ServiceRegistryArgParserTest, TooShortArgument) {
  std::vector<std::string> argv = {"program", "-"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::HasSubstr("Argument must start with --"));
}

// Field value edge cases

TEST_F(ServiceRegistryArgParserTest, EmptyFieldValue) {
  std::vector<std::string> argv = {
      "program", "--host=", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq(""));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest, EmptyPortValue) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port="};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsFalse());
  ASSERT_THAT(
      result.message(),
      ::testing::AnyOf(
          ::testing::HasSubstr("Didn't find int in string"),
          ::testing::HasSubstr("CheckInBounds"),
          ::testing::HasSubstr("out of bounds")));
}

TEST_F(ServiceRegistryArgParserTest, FieldValueWithEquals) {
  std::vector<std::string> argv = {
      "program", "--host=key=value", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("key=value"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       FieldValueWithSpecialCharacters) {
  std::vector<std::string> argv = {
      "program", "--host=host@example.com", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("host@example.com"));
  ASSERT_THAT(config.port, Eq(8080));
}

// Duplicate field tests

TEST_F(ServiceRegistryArgParserTest,
       DuplicateHostOverwrites) {
  std::vector<std::string> argv = {
      "program", "--host=first", "--host=second",
      "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("second"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       DuplicatePortOverwrites) {
  std::vector<std::string> argv = {
      "program", "--host=localhost", "--port=8080",
      "--port=9000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("localhost"));
  ASSERT_THAT(config.port, Eq(9000));
}

// Real-world usage scenarios

TEST_F(ServiceRegistryArgParserTest, ProductionLikeConfig) {
  std::vector<std::string> argv = {
      "program", "--host=0.0.0.0", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("0.0.0.0"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       DevelopmentLikeConfig) {
  std::vector<std::string> argv = {
      "program", "--host=127.0.0.1", "--port=3000"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("127.0.0.1"));
  ASSERT_THAT(config.port, Eq(3000));
}

TEST_F(ServiceRegistryArgParserTest, IPv6Host) {
  std::vector<std::string> argv = {"program", "--host=::1",
                                   "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("::1"));
  ASSERT_THAT(config.port, Eq(8080));
}

TEST_F(ServiceRegistryArgParserTest,
       HostnameWithPortInHost) {
  std::vector<std::string> argv = {
      "program", "--host=example.com:443", "--port=8080"};
  auto result = ParseArgs<Config>(argv);
  ASSERT_THAT(result.ok(), IsTrue());
  Config config = result.value();
  ASSERT_THAT(config.host, StrEq("example.com:443"));
  ASSERT_THAT(config.port, Eq(8080));
}

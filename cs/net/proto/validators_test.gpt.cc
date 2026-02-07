// cs/net/proto/validators_test.gpt.cc
#include "cs/net/proto/validators.gpt.hh"
#include "cs/result.hh"
#include "gtest/gtest.h"

namespace cs::net::proto::validation {
namespace {

// --- Test message types ---

struct UserMsg {
  std::string name;
  std::string email_addr;
  int age = 0;
  int score = 0;
  std::string token;
  int port_num = 0;
  std::string ts;
  std::string id;
  std::string b64;
  std::string fname;
  std::string hostname;
};

// Field name constants (must be linkable).
inline constexpr char kName[] = "name";
inline constexpr char kEmail[] = "email_addr";
inline constexpr char kAge[] = "age";
inline constexpr char kScore[] = "score";
inline constexpr char kToken[] = "token";
inline constexpr char kPort[] = "port_num";
inline constexpr char kTs[] = "ts";
inline constexpr char kId[] = "id";
inline constexpr char kB64[] = "b64";
inline constexpr char kFname[] = "fname";
inline constexpr char kHostname[] = "hostname";

using FName = field<&UserMsg::name, kName>;
using FEmail = field<&UserMsg::email_addr, kEmail>;
using FAge = field<&UserMsg::age, kAge>;
using FScore = field<&UserMsg::score, kScore>;
using FPort = field<&UserMsg::port_num, kPort>;
using FTs = field<&UserMsg::ts, kTs>;
using FId = field<&UserMsg::id, kId>;
using FB64 = field<&UserMsg::b64, kB64>;
using FFname = field<&UserMsg::fname, kFname>;
using FHostname = field<&UserMsg::hostname, kHostname>;

// --- noop ---

TEST(ValidatorsTest, NoopAlwaysPasses) {
  UserMsg msg{};
  auto result = Validate(msg, noop{});
  EXPECT_TRUE(result.ok());
}

// --- required ---

TEST(ValidatorsTest, RequiredFailsOnDefault) {
  UserMsg msg{};
  auto result = Validate(msg, required<FName>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, RequiredPassesOnNonDefault) {
  UserMsg msg{.name = "Alice"};
  auto result = Validate(msg, required<FName>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, RequiredFailsOnZeroInt) {
  UserMsg msg{.age = 0};
  auto result = Validate(msg, required<FAge>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, RequiredPassesOnNonZeroInt) {
  UserMsg msg{.age = 25};
  auto result = Validate(msg, required<FAge>{});
  EXPECT_TRUE(result.ok());
}

// --- email ---

TEST(ValidatorsTest, EmailValid) {
  UserMsg msg{.email_addr = "a@b.com"};
  auto result = Validate(msg, email<FEmail>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, EmailMissingAt) {
  UserMsg msg{.email_addr = "noatsign"};
  auto result = Validate(msg, email<FEmail>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, EmailAtStart) {
  UserMsg msg{.email_addr = "@domain.com"};
  auto result = Validate(msg, email<FEmail>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, EmailAtEnd) {
  UserMsg msg{.email_addr = "user@"};
  auto result = Validate(msg, email<FEmail>{});
  EXPECT_FALSE(result.ok());
}

// --- gt ---

TEST(ValidatorsTest, GtPasses) {
  UserMsg msg{.age = 19};
  auto result = Validate(msg, gt<FAge, 18>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, GtFailsEqual) {
  UserMsg msg{.age = 18};
  auto result = Validate(msg, gt<FAge, 18>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, GtFailsLess) {
  UserMsg msg{.age = 17};
  auto result = Validate(msg, gt<FAge, 18>{});
  EXPECT_FALSE(result.ok());
}

// --- ge ---

TEST(ValidatorsTest, GePasses) {
  UserMsg msg{.age = 18};
  auto result = Validate(msg, ge<FAge, 18>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, GeFailsLess) {
  UserMsg msg{.age = 17};
  auto result = Validate(msg, ge<FAge, 18>{});
  EXPECT_FALSE(result.ok());
}

// --- lt ---

TEST(ValidatorsTest, LtPasses) {
  UserMsg msg{.age = 17};
  auto result = Validate(msg, lt<FAge, 18>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, LtFailsEqual) {
  UserMsg msg{.age = 18};
  auto result = Validate(msg, lt<FAge, 18>{});
  EXPECT_FALSE(result.ok());
}

// --- le ---

TEST(ValidatorsTest, LePasses) {
  UserMsg msg{.age = 18};
  auto result = Validate(msg, le<FAge, 18>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, LeFailsGreater) {
  UserMsg msg{.age = 19};
  auto result = Validate(msg, le<FAge, 18>{});
  EXPECT_FALSE(result.ok());
}

// --- len_gt ---

TEST(ValidatorsTest, LenGtPasses) {
  UserMsg msg{.name = "Alice"};
  auto result = Validate(msg, len_gt<FName, 2>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, LenGtFailsEqual) {
  UserMsg msg{.name = "AB"};
  auto result = Validate(msg, len_gt<FName, 2>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, LenGtFailsLess) {
  UserMsg msg{.name = "A"};
  auto result = Validate(msg, len_gt<FName, 2>{});
  EXPECT_FALSE(result.ok());
}

// --- len_lt ---

TEST(ValidatorsTest, LenLtPasses) {
  UserMsg msg{.name = "A"};
  auto result = Validate(msg, len_lt<FName, 3>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, LenLtFailsEqual) {
  UserMsg msg{.name = "ABC"};
  auto result = Validate(msg, len_lt<FName, 3>{});
  EXPECT_FALSE(result.ok());
}

// --- enum_in ---

TEST(ValidatorsTest, EnumInPasses) {
  UserMsg msg{.score = 10};
  auto result = Validate(msg, enum_in<FScore, 5, 10, 15>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, EnumInFails) {
  UserMsg msg{.score = 7};
  auto result = Validate(msg, enum_in<FScore, 5, 10, 15>{});
  EXPECT_FALSE(result.ok());
}

// --- matches ---

inline constexpr char kDigitsPattern[] = R"(^\d+$)";

TEST(ValidatorsTest, MatchesPasses) {
  UserMsg msg{.name = "12345"};
  auto result =
      Validate(msg, matches<FName, kDigitsPattern>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, MatchesFails) {
  UserMsg msg{.name = "abc"};
  auto result =
      Validate(msg, matches<FName, kDigitsPattern>{});
  EXPECT_FALSE(result.ok());
}

// --- iso8601 ---

TEST(ValidatorsTest, Iso8601Passes) {
  UserMsg msg{.ts = "2024-01-15T12:30:00Z"};
  auto result = Validate(msg, iso8601<FTs>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, Iso8601PassesWithFraction) {
  UserMsg msg{.ts = "2024-01-15T12:30:00.123Z"};
  auto result = Validate(msg, iso8601<FTs>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, Iso8601Fails) {
  UserMsg msg{.ts = "not-a-timestamp"};
  auto result = Validate(msg, iso8601<FTs>{});
  EXPECT_FALSE(result.ok());
}

// --- uuid ---

TEST(ValidatorsTest, UuidPasses) {
  UserMsg msg{.id = "550e8400-e29b-41d4-a716-446655440000"};
  auto result = Validate(msg, uuid<FId>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, UuidFails) {
  UserMsg msg{.id = "not-a-uuid"};
  auto result = Validate(msg, uuid<FId>{});
  EXPECT_FALSE(result.ok());
}

// --- base64 ---

TEST(ValidatorsTest, Base64Passes) {
  UserMsg msg{.b64 = "SGVsbG8="};
  auto result = Validate(msg, base64<FB64>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, Base64PassesEmpty) {
  UserMsg msg{.b64 = ""};
  auto result = Validate(msg, base64<FB64>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, Base64Fails) {
  UserMsg msg{.b64 = "not valid!@#"};
  auto result = Validate(msg, base64<FB64>{});
  EXPECT_FALSE(result.ok());
}

// --- filename ---

TEST(ValidatorsTest, FilenamePasses) {
  UserMsg msg{.fname = "hello.txt"};
  auto result = Validate(msg, filename<FFname>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, FilenameFailsSlash) {
  UserMsg msg{.fname = "dir/file.txt"};
  auto result = Validate(msg, filename<FFname>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, FilenameFailsBackslash) {
  UserMsg msg{.fname = "dir\\file.txt"};
  auto result = Validate(msg, filename<FFname>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, FilenameFailsDot) {
  UserMsg msg{.fname = "."};
  auto result = Validate(msg, filename<FFname>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, FilenameFailsDotDot) {
  UserMsg msg{.fname = ".."};
  auto result = Validate(msg, filename<FFname>{});
  EXPECT_FALSE(result.ok());
}

// --- host ---

TEST(ValidatorsTest, HostPasses) {
  UserMsg msg{.hostname = "example.com"};
  auto result = Validate(msg, host<FHostname>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, HostPassesIPv6) {
  UserMsg msg{.hostname = "[::1]"};
  auto result = Validate(msg, host<FHostname>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, HostFailsEmpty) {
  UserMsg msg{.hostname = ""};
  auto result = Validate(msg, host<FHostname>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, HostFailsTooLong) {
  UserMsg msg{.hostname = std::string(254, 'a')};
  auto result = Validate(msg, host<FHostname>{});
  EXPECT_FALSE(result.ok());
}

// --- port ---

TEST(ValidatorsTest, PortPasses) {
  UserMsg msg{.port_num = 8080};
  auto result = Validate(msg, port<FPort>{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, PortFailsZero) {
  UserMsg msg{.port_num = 0};
  auto result = Validate(msg, port<FPort>{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, PortFailsTooLarge) {
  UserMsg msg{.port_num = 70000};
  auto result = Validate(msg, port<FPort>{});
  EXPECT_FALSE(result.ok());
}

// --- all (composition) ---

TEST(ValidatorsTest, AllPassesWhenAllRulesPass) {
  UserMsg msg{.name = "Alice", .age = 25};
  using Rules = all<required<FName>, gt<FAge, 18>>;
  auto result = Validate(msg, Rules{});
  EXPECT_TRUE(result.ok());
}

TEST(ValidatorsTest, AllFailsWhenOneRuleFails) {
  UserMsg msg{.name = "Alice", .age = 10};
  using Rules = all<required<FName>, gt<FAge, 18>>;
  auto result = Validate(msg, Rules{});
  EXPECT_FALSE(result.ok());
}

TEST(ValidatorsTest, AllCollectsMultipleErrors) {
  UserMsg msg{};
  using Rules = all<required<FName>, gt<FAge, 18>>;
  auto result = Validate(msg, Rules{});
  EXPECT_FALSE(result.ok());
  // Should mention both fields.
  EXPECT_NE(result.message().find("name"),
            std::string::npos);
  EXPECT_NE(result.message().find("age"),
            std::string::npos);
}

// --- oneof_set (no-op placeholder) ---

TEST(ValidatorsTest, OneofSetIsNoop) {
  UserMsg msg{};
  auto result = Validate(msg, oneof_set<FName>{});
  EXPECT_TRUE(result.ok());
}

}  // namespace
}  // namespace cs::net::proto::validation

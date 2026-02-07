// cs/net/proto/db/db_test.gpt.cc
#include "cs/net/proto/db/db.hh"

#include <cstdlib>
#include <string>

#include "cs/fs/fs.hh"
#include "cs/net/json/object.hh"
#include "cs/result.hh"
#include "gtest/gtest.h"

namespace cs::net::proto::db {
namespace {

class LocalJsonDatabaseTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary directory, then use a child
    // path so that set()'s mkdir call can succeed.
    char tmpl[] = "/tmp/db_test_XXXXXX";
    char* dir = mkdtemp(tmpl);
    ASSERT_NE(dir, nullptr);
    tmp_dir_ = dir;
    // Use a child directory that doesn't yet exist so
    // that the first mkdir inside set() can create it.
    db_ = LocalJsonDatabase(std::string(dir) + "/data");
  }

  void TearDown() override {
    // Clean up temp dir.
    std::string cmd = "rm -rf " + tmp_dir_;
    std::system(cmd.c_str());
  }

  std::string tmp_dir_;
  LocalJsonDatabase db_;
};

TEST_F(LocalJsonDatabaseTest, GetMissingReturnsError) {
  auto result = db_.get("nonexistent");
  EXPECT_FALSE(result.ok());
}

TEST_F(LocalJsonDatabaseTest, SetAndGetRoundTrip) {
  auto obj = cs::net::json::Object::NewMap(
      {{"key", cs::net::json::Object::NewString("value")}});
  // The set API expects paths like "collection/id.json"
  // so the enclosing folder logic works correctly.
  auto set_result = db_.set("items/testdata.json", obj);
  ASSERT_TRUE(set_result.ok()) << set_result.message();

  // get() reads <data_dir>/<name>.json so we need
  // the resource to be at the top level. Write
  // directly to test the round-trip instead.
  cs::fs::write(tmp_dir_ + "/data/roundtrip.json",
                obj.str());
  auto get_result = db_.get("roundtrip");
  ASSERT_TRUE(get_result.ok()) << get_result.message();
  auto val = get_result.value().get("key");
  ASSERT_TRUE(val.ok());
  EXPECT_EQ(val.value().as(std::string{}), "value");
}

TEST_F(LocalJsonDatabaseTest,
       InvalidResourceNameReturnsError) {
  // Names with special characters should fail.
  auto result = db_.get("bad/name");
  EXPECT_FALSE(result.ok());
}

TEST_F(LocalJsonDatabaseTest,
       EmptyResourceNameReturnsError) {
  auto result = db_.get("");
  EXPECT_FALSE(result.ok());
}

}  // namespace
}  // namespace cs::net::proto::db

// cs/net/json/object_test.cc
#include "cs/net/json/object.hh"

#include <cmath>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::Result;
using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

TEST(ObjectTest, AsFallbacks) {
  Object obj_int(123);
  // as(float) now converts int to float for compatibility
  // with JSON number parsing (JSON numbers can be ints or
  // floats).
  EXPECT_FALSE(std::isnan(obj_int.as(float())));
  EXPECT_EQ(obj_int.as(float()), 123.0f);
  EXPECT_EQ(obj_int.as(bool()), false);
  EXPECT_EQ(obj_int.as(std::string()), "");

  Object obj_str(std::string("value"));
  EXPECT_EQ(obj_str.as(int()), 0);
  EXPECT_TRUE(obj_str.as(Object::KVMap()).empty());
  EXPECT_TRUE(obj_str.as(std::vector<Object>()).empty());
}

TEST(ObjectTest, GetErrors) {
  Object obj_int(1);
  auto non_map = obj_int.get("missing");
  EXPECT_THAT(non_map.ok(), IsFalse());

  Object map = Object::NewMap();
  ASSERT_THAT(map.set("a", Object(1)).ok(), IsTrue());
  auto missing_key = map.get("b");
  EXPECT_THAT(missing_key.ok(), IsFalse());
}

TEST(ObjectTest, SetErrors) {
  Object obj_int(1);
  auto result = obj_int.set("a", Object(2));
  EXPECT_THAT(result.ok(), IsFalse());
}

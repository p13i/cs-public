// cs/net/json/serialize_test.cc
#include "cs/net/json/serialize.hh"

#include <iostream>
#include <sstream>

#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::Result;
using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::cs::net::json::parsers::ParseArray;
using ::cs::net::json::parsers::ParseBoolean;
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::json::parsers::ParseString;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::HasSubstr;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

class SerializeTest : public ::testing::Test {};

TEST_F(SerializeTest, ExampleMap) {
  Object object = Object(std::map<std::string, Object>{
      {"key", Object(std::string("value"))},
      {"key2", Object(std::vector<Object>{
                   Object(true),
                   Object(false),
                   Object(std::string("hello")),
                   Object(1.4f),
               })}});
  std::stringstream ss;
  ss << object;
  std::string actual = ss.str();
  std::string expected =
      R"json({"key":"value","key2":[true,false,"hello",1.4]})json";
  ASSERT_THAT(actual, StrEq(expected));

  unsigned int cursor = 0;
  auto result = ParseObject(actual, &cursor);

  // Check objects are equal.
  ASSERT_THAT(result.ok(), IsTrue());
  ASSERT_THAT(cursor, Eq(actual.length()));
  Object parsed_object = result.value();
  ASSERT_THAT(parsed_object.type(), Eq(Type::kMap))
      << parsed_object;
  auto map = parsed_object.as(
      std::map<std::string, cs::net::json::Object>());
  ASSERT_THAT(map.size(), Eq(2));
  ASSERT_THAT(map["key"].type(), Eq(Type::kString));
  ASSERT_THAT(map["key"].as(std::string()), StrEq("value"));
  ASSERT_THAT(map["key2"].type(), Eq(Type::kArray));
  auto array =
      map["key2"].as(std::vector<cs::net::json::Object>());
  ASSERT_THAT(array.size(), Eq(4));
  ASSERT_THAT(array[0].type(), Eq(Type::kBoolean));
  ASSERT_THAT(array[0].as(bool()), IsTrue());
  ASSERT_THAT(array[1].type(), Eq(Type::kBoolean));
  ASSERT_THAT(array[1].as(bool()), IsFalse());
  ASSERT_THAT(array[2].type(), Eq(Type::kString));
}

TEST_F(SerializeTest, WithIndent) {
  Object object = Object(std::map<std::string, Object>{
      {"key", Object(std::string("value"))},
      {"key2", Object(std::vector<Object>{
                   Object(true),
                   Object(false),
                   Object(std::string("hello")),
                   Object(1.4f),
               })}});
  std::stringstream ss;
  cs::net::json::SerializeObjectPrettyPrintRecurse(
      ss, object, 4, 0);
  std::string actual = ss.str();
  std::string expected =
      // clang-format off
R"json({
    "key": "value",
    "key2": [
        true,
        false,
        "hello",
        1.4
    ]
})json";
  // clang-format on
  ASSERT_THAT(actual, StrEq(expected));
}

TEST_F(SerializeTest, EscapesStrings) {
  std::string input;
  input.push_back('a');
  input.push_back('\b');
  input.push_back('\f');
  input.push_back('\n');
  input.push_back('\r');
  input.push_back('\t');
  input.push_back('"');
  input.push_back('\\');
  input.push_back('\x01');

  Object object(input);
  std::string actual = SerializeObject(object, 0);
  EXPECT_THAT(actual, HasSubstr("\\b"));
  EXPECT_THAT(actual, HasSubstr("\\f"));
  EXPECT_THAT(actual, HasSubstr("\\n"));
  EXPECT_THAT(actual, HasSubstr("\\r"));
  EXPECT_THAT(actual, HasSubstr("\\t"));
  EXPECT_THAT(actual, HasSubstr("\\\""));
  EXPECT_THAT(actual, HasSubstr("\\\\"));
  EXPECT_THAT(actual, HasSubstr("\\u0001"));
}
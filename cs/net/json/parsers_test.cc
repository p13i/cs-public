#include "cs/net/json/parsers.hh"

#include <variant>

#include "cs/net/json/object.hh"
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
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

class ParseTest : public ::testing::Test {
 public:
  void SetUp() override { _cursor = 0; }

  unsigned int _cursor;
};

class ParseBooleanTest : public ParseTest {};

TEST_F(ParseBooleanTest, EmptyString) {
  auto result = ParseBoolean("", &_cursor);
  ASSERT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseBooleanTest, True) {
  auto result = ParseBoolean("true", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), IsTrue());
  EXPECT_THAT(_cursor, Eq(4));
}

TEST_F(ParseBooleanTest, False) {
  auto result = ParseBoolean("false", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), IsFalse());
  EXPECT_THAT(_cursor, Eq(5));
}

TEST_F(ParseBooleanTest, TrueWithStuffAfter) {
  auto result = ParseBoolean("trueabc", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(_cursor, Eq(4));
  EXPECT_THAT(result.data(), IsTrue());
}

TEST_F(ParseBooleanTest, FalseWithStuffAfter) {
  auto result = ParseBoolean("falseabc", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(_cursor, Eq(5));
  EXPECT_THAT(result.data(), IsFalse());
}

TEST_F(ParseBooleanTest, WithWhitespaceBefore) {
  auto result = ParseBoolean(" false", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(_cursor, Eq(6));
}

class ParseNumberTest : public ParseTest {
 protected:
  template <typename T>
  void ExpectParseNumber(const std::string& str,
                         T expected_value) {
    auto result_variant = ParseNumber(str, &_cursor);
    ASSERT_OK(result_variant);
    ASSERT_TRUE(
        std::holds_alternative<T>(result_variant.value()));
    EXPECT_THAT(std::get<T>(result_variant.value()),
                Eq(expected_value));
    // Cursor should be at end of string.
    EXPECT_THAT(_cursor, Eq(str.length()));
  }
};

TEST_F(ParseNumberTest, Int0) {
  ExpectParseNumber<int>("0", 0);
}

TEST_F(ParseNumberTest, Float0dot0) {
  ExpectParseNumber<float>("0.0", 0);
}

TEST_F(ParseNumberTest, Int1) {
  ExpectParseNumber<int>("1", 1);
}

TEST_F(ParseNumberTest, Float1dot0) {
  ExpectParseNumber<float>("1.0", 1);
}

TEST_F(ParseNumberTest, Float1dot) {
  ExpectParseNumber<float>("1.", 1);
}

TEST_F(ParseNumberTest, Float0_1) {
  ExpectParseNumber<float>("0.1", 0.1);
}

TEST_F(ParseNumberTest, Float1_1) {
  ExpectParseNumber<float>("1.1", 1.1);
}

TEST_F(ParseNumberTest, IntNeg0) {
  ExpectParseNumber<int>("-0", 0);
}

TEST_F(ParseNumberTest, FloatNeg0) {
  ExpectParseNumber<float>("-0.", 0);
}

TEST_F(ParseNumberTest, IntPos0) {
  ExpectParseNumber<int>("+0", 0);
}

TEST_F(ParseNumberTest, FloatPos0) {
  ExpectParseNumber<float>("+0.", 0);
}

TEST_F(ParseNumberTest, Int100) {
  ExpectParseNumber<int>("100", 100);
}

TEST_F(ParseNumberTest, Float100) {
  ExpectParseNumber<float>("100.", 100);
}

TEST_F(ParseNumberTest, Float100_001) {
  ExpectParseNumber<float>("100.001", 100.001);
}

TEST_F(ParseNumberTest, Float1e1) {
  ExpectParseNumber<float>("1e0", 1);
}

TEST_F(ParseNumberTest, Float12e2) {
  ExpectParseNumber<float>("12e2", 1200);
}

class ParseStringTest : public ParseTest {};

TEST_F(ParseStringTest, EmptyString) {
  EXPECT_THAT(ParseString("\"\"", &_cursor).value(),
              StrEq(""));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseStringTest, abc) {
  EXPECT_THAT(ParseString("\"abc\"", &_cursor).value(),
              StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(5));
}

TEST_F(ParseStringTest, abcWithSpaces) {
  EXPECT_THAT(ParseString("\" a b c \"", &_cursor).value(),
              StrEq(" a b c "));
  EXPECT_THAT(_cursor, Eq(9));
}

TEST_F(ParseStringTest, Newline) {
  EXPECT_THAT(ParseString("\"\n\"", &_cursor).value(),
              StrEq("\n"));
  EXPECT_THAT(_cursor, Eq(3));
}

TEST_F(ParseStringTest, Escaping) {
  EXPECT_THAT(ParseString(R"("")", &_cursor).value(),
              StrEq(""));
  EXPECT_THAT(_cursor, Eq(2));
}
TEST_F(ParseStringTest, Escaping2) {
  EXPECT_THAT(ParseString(R"("\\")", &_cursor).value(),
              StrEq("\\"));
  EXPECT_THAT(_cursor, Eq(4));
}
TEST_F(ParseStringTest, Escaping3) {
  ASSERT_OK_EXPECT_MATCHING(ParseString("\"\"", &_cursor),
                            StrEq(""));
  EXPECT_THAT(_cursor, Eq(2));
}
TEST_F(ParseStringTest, Escaping4) {
  ASSERT_OK_EXPECT_MATCHING(
      ParseString("\"\\\"\"", &_cursor), StrEq("\""));
  EXPECT_THAT(_cursor, Eq(4));
}
TEST_F(ParseStringTest, Escaping5) {
  ASSERT_OK_EXPECT_MATCHING(
      ParseString("\"\\\"\\\"\"", &_cursor), StrEq("\"\""));
  EXPECT_THAT(_cursor, Eq(6));
}
TEST_F(ParseStringTest, Escaping6) {
  ASSERT_OK_EXPECT_MATCHING(
      ParseString("\"\\\n\"", &_cursor), StrEq("\n"));
  EXPECT_THAT(_cursor, Eq(4));
}

class ParseArrayTest : public ParseTest {};

TEST_F(ParseArrayTest, EmptyArray) {
  auto result = ParseObject("[]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kArray));
  EXPECT_THAT(result.value().as_array().size(), Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseArrayTest, ArrayWithOneElement) {
  auto result = ParseArray("[1]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.size(), Eq(1));
  EXPECT_THAT(array.at(0).type(), Eq(Type::kNumber));
  EXPECT_THAT(array.at(0).as(int()), Eq(1));
  EXPECT_THAT(_cursor, Eq(3));
}

TEST_F(ParseArrayTest, ArrayWithTwoElements) {
  auto result = ParseObject("[1,2]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kArray));
  auto array = result.value().as_array();
  EXPECT_THAT(array.size(), Eq(2));
  EXPECT_THAT(array.at(0).type(), Eq(Type::kNumber));
  EXPECT_THAT(array.at(0).as(int()), Eq(1));
  EXPECT_THAT(array.at(1).type(), Eq(Type::kNumber));
  EXPECT_THAT(array.at(1).as(int()), Eq(2));
  EXPECT_THAT(_cursor, Eq(5));
}

TEST_F(ParseArrayTest,
       ArrayWithOneStringAndOneFloatAndOneBool) {
  auto result = ParseObject("[\"abc\",1.1,true]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(3));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kString));
  EXPECT_THAT(array.as_array().at(0).as_string(),
              StrEq("abc"));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as_array().at(2).type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(array.as_array().at(2).as_bool(), IsTrue());
  EXPECT_THAT(_cursor, Eq(16));
}

TEST_F(ParseArrayTest, NestedArray) {
  auto result = ParseObject("[[1,2],[3,4]]", &_cursor);
  ASSERT_TRUE(result.ok()) << result;

  auto array = result.value();
  EXPECT_EQ(array.type(), Type::kArray);
  EXPECT_EQ(array.as_array().size(), 2);

  auto subArray1 = array.as_array().at(0).as_array();
  EXPECT_EQ(subArray1.size(), 2);
  EXPECT_EQ(subArray1.at(0).type(), Type::kNumber);
  EXPECT_THAT(subArray1.at(0).as(int()), Eq(1));
  EXPECT_EQ(subArray1.at(1).type(), Type::kNumber);
  EXPECT_THAT(subArray1.at(1).as(int()), Eq(2));

  auto subArray2 = array.as_array().at(1).as_array();
  EXPECT_EQ(subArray2.size(), 2);
  EXPECT_EQ(subArray2.at(0).type(), Type::kNumber);
  EXPECT_THAT(subArray2.at(0).as(int()), Eq(3));
  EXPECT_EQ(subArray2.at(1).type(), Type::kNumber);
  EXPECT_THAT(subArray2.at(1).as(int()), Eq(4));

  EXPECT_EQ(_cursor, 13);
}

class ParseMapTest : public ParseTest {};

TEST_F(ParseMapTest, EmptyMap) {
  auto result = ParseObject("{}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kMap));
  EXPECT_THAT(result.value().as_map().size(), Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseMapTest, MapWithOneElement) {
  auto result = ParseObject("{\"a\":1}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(1));
  EXPECT_THAT(map.as_map().at("a").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(map.as_map().at("a").as(int()), Eq(1));
  EXPECT_THAT(_cursor, Eq(7));
}

TEST_F(ParseMapTest, MapWithTwoElements) {
  auto result = ParseObject("{\"a\":1,\"b\":2}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(2));
  EXPECT_THAT(map.as_map().at("a").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(map.as_map().at("a").as(int()), Eq(1));
  EXPECT_THAT(map.as_map().at("b").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(map.as_map().at("b").as(int()), Eq(2));
  EXPECT_THAT(_cursor, Eq(13));
}

TEST_F(ParseMapTest,
       MapWithOneStringAndOneFloatAndOneBool) {
  auto result = ParseObject(
      "{\"a\":\"abc\",\"b\":1.1,\"c\":true}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(3));
  EXPECT_THAT(map.as_map().at("a").type(),
              Eq(Type::kString));
  EXPECT_THAT(map.as_map().at("a").as_string(),
              StrEq("abc"));
  EXPECT_THAT(map.as_map().at("b").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(map.as_map().at("b").as(float()),
              FloatEq(1.1));
  EXPECT_THAT(map.as_map().at("c").type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(map.as_map().at("c").as_bool(), IsTrue());
  EXPECT_THAT(_cursor, Eq(28));
}

TEST_F(ParseMapTest, NestedMap) {
  auto result =
      ParseObject("{\"a\":{\"b\":1,\"c\":2}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(1));
  auto nestedMap = map.as_map().at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap.as_map().size(), Eq(2));
  EXPECT_THAT(nestedMap.as_map().at("b").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedMap.as_map().at("b").as(int()), Eq(1));
  EXPECT_THAT(nestedMap.as_map().at("c").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedMap.as_map().at("c").as(int()), Eq(2));
  EXPECT_THAT(_cursor, Eq(19));
}

TEST_F(ParseMapTest, MapWithArray) {
  auto result = ParseObject("{\"a\":[1,2,3]}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(1));
  auto array = map.as_map().at("a");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(3));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(int()), Eq(1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(int()), Eq(2));
  EXPECT_THAT(array.as_array().at(2).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(2).as(int()), Eq(3));
  EXPECT_THAT(_cursor, Eq(13));
}

TEST_F(ParseMapTest, MapWithMapAndArray) {
  auto result =
      ParseObject("{\"a\":{\"b\":[1,2,3]}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(1));
  auto nestedMap = map.as_map().at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap.as_map().size(), Eq(1));
  auto array = nestedMap.as_map().at("b");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(3));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(int()), Eq(1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(int()), Eq(2));
  EXPECT_THAT(array.as_array().at(2).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(2).as(int()), Eq(3));
  EXPECT_THAT(_cursor, Eq(19));
}

TEST_F(ParseMapTest, MapWithMapAndArrayAndMap) {
  auto result = ParseObject(
      "{\"a\":{\"b\":[1,2,3],\"c\":{\"d\":4}}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(1));
  auto nestedMap = map.as_map().at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap.as_map().size(), Eq(2));
  auto array = nestedMap.as_map().at("b");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(3));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(int()), Eq(1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(int()), Eq(2));
  EXPECT_THAT(array.as_array().at(2).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(2).as(int()), Eq(3));
  auto nestedNestedMap = nestedMap.as_map().at("c");
  EXPECT_THAT(nestedNestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedNestedMap.as_map().size(), Eq(1));
  EXPECT_THAT(nestedNestedMap.as_map().at("d").type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedNestedMap.as_map().at("d").as(int()),
              Eq(4));
  EXPECT_THAT(_cursor, Eq(31));
}

class ParseObjectTest : public ParseTest {};

TEST_F(ParseObjectTest, EmptyString) {
  auto result = ParseObject("", &_cursor);
  ASSERT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, EmptyMap) {
  auto result = ParseObject("{}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kMap));
  EXPECT_THAT(result.value().as_map().size(), Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseObjectTest, EmptyArray) {
  auto result = ParseObject("[]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kArray));
  EXPECT_THAT(result.value().as_array().size(), Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseObjectTest,
       ArrayWithOneFloatAndOneBoolAndOneString) {
  std::string str = "[1.1,true,\"abc\"]";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(3));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(array.as_array().at(1).as_bool(), IsTrue());
  EXPECT_THAT(array.as_array().at(2).type(),
              Eq(Type::kString));
  EXPECT_THAT(array.as_array().at(2).as_string(),
              StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, ArrayWithTwoFloats) {
  auto result = ParseObject("[1.1,2.2]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(2));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(float()),
              FloatEq(2.2));
  EXPECT_THAT(_cursor, Eq(9));
}

TEST_F(ParseObjectTest,
       NestedMapsWithArraysBoolsFloatsAndStrings) {
  std::string str =
      "{\"a\":[1.1,2.2],\"b\":{\"c\":true,\"d\":\"abc\"}}";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(map.as_map().size(), Eq(2));
  auto array = map.as_map().at("a");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(array.as_array().size(), Eq(2));
  EXPECT_THAT(array.as_array().at(0).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(0).as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as_array().at(1).type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as_array().at(1).as(float()),
              FloatEq(2.2));
  auto nestedMap = map.as_map().at("b");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap.as_map().size(), Eq(2));
  EXPECT_THAT(nestedMap.as_map().at("c").type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(nestedMap.as_map().at("c").as_bool(),
              IsTrue());
  EXPECT_THAT(nestedMap.as_map().at("d").type(),
              Eq(Type::kString));
  EXPECT_THAT(nestedMap.as_map().at("d").as_string(),
              StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, OneFloat) {
  std::string str = "1.1";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kNumber));
  EXPECT_THAT(result.value().as(float()), FloatEq(1.1));
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, OneBool) {
  std::string str = "false";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kBoolean));
  EXPECT_THAT(result.value().as_bool(), IsFalse());
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, OneString) {
  std::string str = "\"abc\"";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kString));
  EXPECT_THAT(result.value().as_string(), StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(str.size()));
}
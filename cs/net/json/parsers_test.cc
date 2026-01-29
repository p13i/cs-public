// cs/net/json/parsers_test.cc
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
using ::cs::net::json::parsers::ParseMap;
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::json::parsers::ParseString;
using ::cs::net::json::parsers::operator>>;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::Ge;
using ::testing::Gt;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Lt;
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

TEST_F(ParseNumberTest, ParseNumberNoCursorInt) {
  auto result = ParseNumber("42");
  ASSERT_THAT(result.ok(), IsTrue());
  ASSERT_TRUE(std::holds_alternative<int>(result.value()));
  EXPECT_THAT(std::get<int>(result.value()), Eq(42));
}

TEST_F(ParseNumberTest, ParseNumberNoCursorFloat) {
  auto result = ParseNumber("1.5");
  ASSERT_THAT(result.ok(), IsTrue());
  ASSERT_TRUE(
      std::holds_alternative<float>(result.value()));
  EXPECT_THAT(std::get<float>(result.value()),
              FloatEq(1.5f));
}

TEST_F(ParseNumberTest, ParseNumberStopsAtWhitespace) {
  _cursor = 0;
  auto result = ParseNumber("42 ", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  ASSERT_TRUE(std::holds_alternative<int>(result.value()));
  EXPECT_THAT(std::get<int>(result.value()), Eq(42));
  EXPECT_THAT(_cursor, Eq(2u));
}

TEST_F(ParseNumberTest, AcceptsVerySmallFloatUnderflow) {
  // Test the specific case from the error message
  // This number may be denormalized but should still parse
  // successfully
  _cursor = 0;
  auto result = ParseNumber("4.465938e-41", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  ASSERT_TRUE(
      std::holds_alternative<float>(result.value()));
  float value = std::get<float>(result.value());
  // May be a very small denormalized number or 0.0, both
  // are acceptable
  EXPECT_THAT(value, Ge(0.0f));
  EXPECT_THAT(value, Lt(1e-40f));
  EXPECT_THAT(_cursor, Eq(12u));
}

TEST_F(ParseNumberTest,
       AcceptsVerySmallFloatLargeNegativeExponent) {
  _cursor = 0;
  auto result = ParseNumber("1e-100", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  ASSERT_TRUE(
      std::holds_alternative<float>(result.value()));
  float value = std::get<float>(result.value());
  // Should be 0.0 due to underflow
  EXPECT_THAT(value, FloatEq(0.0f));
  EXPECT_THAT(_cursor, Eq(6u));
}

TEST_F(ParseNumberTest, AcceptsVerySmallFloatWithDecimal) {
  _cursor = 0;
  auto result = ParseNumber(
      "0.0000000000000000000000000000000000001", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  ASSERT_TRUE(
      std::holds_alternative<float>(result.value()));
  float value = std::get<float>(result.value());
  // Should be 0.0 or very small due to underflow/rounding
  EXPECT_THAT(value, Ge(0.0f));
  EXPECT_THAT(value, Lt(1e-36f));
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

TEST_F(ParseStringTest, ParseStringDefaultCursor) {
  auto result = ParseString("\"abc\"");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), StrEq("abc"));
}

TEST_F(ParseStringTest, MissingClosingQuote) {
  auto result = ParseString("\"abc", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseStringTest, InvalidStart) {
  auto result = ParseString("abc", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST(ParseOperators, ShiftOperatorParsesObject) {
  Object obj;
  auto result = std::string("{\"a\":1}") >> &obj;
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(obj.type(), Eq(Type::kMap));
}

class ParseArrayTest : public ParseTest {};

TEST_F(ParseArrayTest, MissingOpenBracket) {
  auto result = ParseArray("1,2]", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseArrayTest, MissingClosingBracket) {
  auto result = ParseArray("[1,2", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseArrayTest, TrailingComma) {
  auto result = ParseArray("[1,2,]", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseArrayTest, InvalidDelimiter) {
  auto result = ParseArray("[1;2]", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseArrayTest, MissingEndBracketOnlyOpen) {
  auto result = ParseArray("[", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

class ParseMapTest2 : public ParseTest {};

TEST_F(ParseMapTest2, MissingOpenBrace) {
  auto result = ParseMap("\"a\":1}", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseMapTest2, MissingClosingBrace) {
  auto result = ParseMap("{\"a\":1", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseMapTest2, TrailingComma) {
  auto result = ParseMap("{\"a\":1,}", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseMapTest2, InvalidDelimiter) {
  auto result = ParseMap("{\"a\":1;}", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseMapTest2, MissingEndBraceOnlyOpen) {
  auto result = ParseMap("{", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseArrayTest, EmptyArray) {
  auto result = ParseObject("[]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kArray));
  EXPECT_THAT(result.value()
                  .as(std::vector<cs::net::json::Object>())
                  .size(),
              Eq(0));
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
  auto array = result.value().as(
      std::vector<cs::net::json::Object>());
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
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(3));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kString));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(std::string()),
              StrEq("abc"));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .as(bool()),
              IsTrue());
  EXPECT_THAT(_cursor, Eq(16));
}

TEST_F(ParseArrayTest, NestedArray) {
  auto result = ParseObject("[[1,2],[3,4]]", &_cursor);
  ASSERT_TRUE(result.ok()) << result;

  auto array = result.value();
  EXPECT_EQ(array.type(), Type::kArray);
  EXPECT_EQ(
      array.as(std::vector<cs::net::json::Object>()).size(),
      2);

  auto subArray1 =
      array.as(std::vector<cs::net::json::Object>())
          .at(0)
          .as(std::vector<cs::net::json::Object>());
  EXPECT_EQ(subArray1.size(), 2);
  EXPECT_EQ(subArray1.at(0).type(), Type::kNumber);
  EXPECT_THAT(subArray1.at(0).as(int()), Eq(1));
  EXPECT_EQ(subArray1.at(1).type(), Type::kNumber);
  EXPECT_THAT(subArray1.at(1).as(int()), Eq(2));

  auto subArray2 =
      array.as(std::vector<cs::net::json::Object>())
          .at(1)
          .as(std::vector<cs::net::json::Object>());
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
  EXPECT_THAT(result.value()
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseMapTest, MapWithOneElement) {
  auto result = ParseObject("{\"a\":1}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(1));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .type(),
      Eq(Type::kNumber));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .as(int()),
      Eq(1));
  EXPECT_THAT(_cursor, Eq(7));
}

TEST_F(ParseMapTest, MapWithTwoElements) {
  auto result = ParseObject("{\"a\":1,\"b\":2}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(2));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .type(),
      Eq(Type::kNumber));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .as(int()),
      Eq(1));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("b")
          .type(),
      Eq(Type::kNumber));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("b")
          .as(int()),
      Eq(2));
  EXPECT_THAT(_cursor, Eq(13));
}

TEST_F(ParseMapTest,
       MapWithOneStringAndOneFloatAndOneBool) {
  auto result = ParseObject(
      "{\"a\":\"abc\",\"b\":1.1,\"c\":true}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(3));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .type(),
      Eq(Type::kString));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a")
          .as(std::string()),
      StrEq("abc"));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("b")
          .type(),
      Eq(Type::kNumber));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("b")
          .as(float()),
      FloatEq(1.1));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("c")
          .type(),
      Eq(Type::kBoolean));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("c")
          .as(bool()),
      IsTrue());
  EXPECT_THAT(_cursor, Eq(28));
}

TEST_F(ParseMapTest, NestedMap) {
  auto result =
      ParseObject("{\"a\":{\"b\":1,\"c\":2}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(1));
  auto nestedMap =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(2));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("b")
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("b")
                  .as(int()),
              Eq(1));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("c")
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("c")
                  .as(int()),
              Eq(2));
  EXPECT_THAT(_cursor, Eq(19));
}

TEST_F(ParseMapTest, MapWithArray) {
  auto result = ParseObject("{\"a\":[1,2,3]}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(1));
  auto array =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(3));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(int()),
              Eq(1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(int()),
              Eq(2));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .as(int()),
              Eq(3));
  EXPECT_THAT(_cursor, Eq(13));
}

TEST_F(ParseMapTest, MapWithMapAndArray) {
  auto result =
      ParseObject("{\"a\":{\"b\":[1,2,3]}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(1));
  auto nestedMap =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(1));
  auto array = nestedMap
                   .as(std::map<std::string,
                                cs::net::json::Object>())
                   .at("b");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(3));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(int()),
              Eq(1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(int()),
              Eq(2));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .as(int()),
              Eq(3));
  EXPECT_THAT(_cursor, Eq(19));
}

TEST_F(ParseMapTest, MapWithMapAndArrayAndMap) {
  auto result = ParseObject(
      "{\"a\":{\"b\":[1,2,3],\"c\":{\"d\":4}}}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto map = result.value();
  EXPECT_THAT(map.type(), Eq(Type::kMap));
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(1));
  auto nestedMap =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(2));
  auto array = nestedMap
                   .as(std::map<std::string,
                                cs::net::json::Object>())
                   .at("b");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(3));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(int()),
              Eq(1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(int()),
              Eq(2));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .as(int()),
              Eq(3));
  auto nestedNestedMap =
      nestedMap
          .as(std::map<std::string,
                       cs::net::json::Object>())
          .at("c");
  EXPECT_THAT(nestedNestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedNestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(1));
  EXPECT_THAT(nestedNestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("d")
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(nestedNestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("d")
                  .as(int()),
              Eq(4));
  EXPECT_THAT(_cursor, Eq(31));
}

class ParseObjectTest : public ParseTest {};

TEST_F(ParseObjectTest, EmptyString) {
  auto result = ParseObject("", &_cursor);
  ASSERT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, ParseObjectNoCursor) {
  auto result = ParseObject("true");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value().as(bool()), IsTrue());
}

TEST_F(ParseObjectTest, RejectsLeadingPlusNumber) {
  auto result = ParseObject("+7", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsMinusWithoutDigit) {
  auto result = ParseObject("-", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsInvalidNumberStart) {
  auto result = ParseObject("x", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsHugeFloat) {
  auto result = ParseObject("1e5000", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsHugeNegativeFloat) {
  auto result = ParseObject("-1e5000", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, AcceptsVerySmallFloatUnderflow) {
  // This number may be denormalized but should be accepted
  // as it results in a finite value (0.0 or denormalized)
  auto result = ParseObject("4.465938e-41", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kNumber));
  // May be a very small denormalized number or 0.0, both
  // are acceptable
  float value = result.value().as(float());
  EXPECT_THAT(value, Ge(0.0f));
  EXPECT_THAT(value, Lt(1e-40f));
  EXPECT_THAT(_cursor, Eq(12));
}

TEST_F(ParseObjectTest,
       AcceptsVerySmallFloatNegativeExponent) {
  auto result = ParseObject("1e-45", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kNumber));
  float value = result.value().as(float());
  // Should be 0.0 or very small due to underflow
  EXPECT_THAT(value, FloatEq(0.0f));
  EXPECT_THAT(_cursor, Eq(5));
}

TEST_F(ParseObjectTest,
       AcceptsVerySmallFloatLargeNegativeExponent) {
  auto result = ParseObject("1.23e-100", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kNumber));
  float value = result.value().as(float());
  // Should be 0.0 due to underflow
  EXPECT_THAT(value, FloatEq(0.0f));
  EXPECT_THAT(_cursor, Eq(9));
}

TEST_F(ParseObjectTest,
       AcceptsVerySmallNegativeFloatUnderflow) {
  // Negative underflow should also be accepted
  auto result = ParseObject("-4.465938e-41", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kNumber));
  float value = result.value().as(float());
  // May be a very small negative denormalized number (not
  // necessarily exactly -0.0)
  EXPECT_THAT(value, ::testing::Gt(-1e-40f));
  EXPECT_THAT(value, ::testing::Lt(0.0f));
  EXPECT_THAT(_cursor, Eq(13));
}

TEST_F(ParseObjectTest, RejectsHugeInt) {
  auto result =
      ParseObject("9999999999999999999999", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsNullPrefix) {
  auto result = ParseObject("nul", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, ParsesStrictEscapes) {
  std::string input = R"("\\\"\/\b\f\n\r\t")";
  auto result = ParseObject(input, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue());
  std::string expected;
  expected.push_back('\\');
  expected.push_back('"');
  expected.push_back('/');
  expected.push_back('\b');
  expected.push_back('\f');
  expected.push_back('\n');
  expected.push_back('\r');
  expected.push_back('\t');
  EXPECT_THAT(result.value().as(std::string()),
              StrEq(expected));
}

TEST_F(ParseObjectTest, RejectsInvalidEscape) {
  auto result = ParseObject(R"("\x")", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsInvalidUnicodeHex) {
  auto result = ParseObject(R"("\uZZZZ")", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsInvalidUnicodeSurrogateHex) {
  auto result = ParseObject(R"("\uD83D\uZZZZ")", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest,
       RejectsInvalidUnicodeSurrogateRange) {
  auto result = ParseObject(R"("\uD83D\u0041")", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsUnexpectedLowSurrogate) {
  auto result = ParseObject(R"("\uDC00")", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, RejectsMissingClosingQuote) {
  auto result = ParseObject("\"abc", &_cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(ParseObjectTest, EmptyMap) {
  auto result = ParseObject("{}", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kMap));
  EXPECT_THAT(result.value()
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseObjectTest, EmptyArray) {
  auto result = ParseObject("[]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kArray));
  EXPECT_THAT(result.value()
                  .as(std::vector<cs::net::json::Object>())
                  .size(),
              Eq(0));
  EXPECT_THAT(_cursor, Eq(2));
}

TEST_F(ParseObjectTest,
       ArrayWithOneFloatAndOneBoolAndOneString) {
  std::string str = "[1.1,true,\"abc\"]";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(3));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(bool()),
              IsTrue());
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .type(),
              Eq(Type::kString));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(2)
                  .as(std::string()),
              StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, ArrayWithTwoFloats) {
  auto result = ParseObject("[1.1,2.2]", &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  auto array = result.value();
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(2));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(float()),
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
  EXPECT_THAT(
      map.as(std::map<std::string, cs::net::json::Object>())
          .size(),
      Eq(2));
  auto array =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("a");
  EXPECT_THAT(array.type(), Eq(Type::kArray));
  EXPECT_THAT(
      array.as(std::vector<cs::net::json::Object>()).size(),
      Eq(2));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(0)
                  .as(float()),
              FloatEq(1.1));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .type(),
              Eq(Type::kNumber));
  EXPECT_THAT(array.as(std::vector<cs::net::json::Object>())
                  .at(1)
                  .as(float()),
              FloatEq(2.2));
  auto nestedMap =
      map.as(std::map<std::string, cs::net::json::Object>())
          .at("b");
  EXPECT_THAT(nestedMap.type(), Eq(Type::kMap));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .size(),
              Eq(2));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("c")
                  .type(),
              Eq(Type::kBoolean));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("c")
                  .as(bool()),
              IsTrue());
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("d")
                  .type(),
              Eq(Type::kString));
  EXPECT_THAT(nestedMap
                  .as(std::map<std::string,
                               cs::net::json::Object>())
                  .at("d")
                  .as(std::string()),
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
  EXPECT_THAT(result.value().as(bool()), IsFalse());
  EXPECT_THAT(_cursor, Eq(str.size()));
}

TEST_F(ParseObjectTest, OneString) {
  std::string str = "\"abc\"";
  auto result = ParseObject(str, &_cursor);
  ASSERT_THAT(result.ok(), IsTrue()) << result;
  EXPECT_THAT(result.value().type(), Eq(Type::kString));
  EXPECT_THAT(result.value().as(std::string()),
              StrEq("abc"));
  EXPECT_THAT(_cursor, Eq(str.size()));
}
// cs/parsers/parsers_test.gpt.cc
#include "cs/parsers/parsers.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::parsers::CheckInBounds;
using ::cs::parsers::Consume;
using ::cs::parsers::ConsumePrefix;
using ::cs::parsers::ConsumeUntil;
using ::cs::parsers::FindFirstIndexOf;
using ::cs::parsers::HasFileExtension;
using ::cs::parsers::IncrementCursor;
using ::cs::parsers::MaybeConsumeWhitespace;
using ::cs::parsers::ParseFloat;
using ::cs::parsers::ParseInt;
using ::cs::parsers::ParseUnsignedInt;
using ::cs::parsers::TryConsumeString;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
}  // namespace

TEST(ParsersTest, CheckInBoundsError) {
  auto result = CheckInBounds("a", 2);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST(ParsersTest, IncrementCursorOutOfBounds) {
  unsigned int cursor = 5;
  auto result = IncrementCursor("a", &cursor);
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST(ParsersTest, TryConsumeString) {
  unsigned int cursor = 0;
  EXPECT_THAT(TryConsumeString("hello", "he", &cursor),
              IsTrue());
  EXPECT_THAT(cursor, Eq(2u));
  EXPECT_THAT(TryConsumeString("hello", "zz", &cursor),
              IsFalse());
}

TEST(ParsersTest, MaybeConsumeWhitespace) {
  unsigned int cursor = 0;
  ASSERT_THAT(
      MaybeConsumeWhitespace(" \n\tabc", &cursor).ok(),
      IsTrue());
  EXPECT_THAT(cursor, Eq(3u));
}

TEST(ParsersTest, ParseIntWithSigns) {
  unsigned int cursor = 0;
  auto pos = ParseInt("+42", &cursor);
  ASSERT_THAT(pos.ok(), IsTrue());
  EXPECT_THAT(pos.value(), Eq(42));
  cursor = 0;
  auto neg = ParseInt("-7", &cursor);
  ASSERT_THAT(neg.ok(), IsTrue());
  EXPECT_THAT(neg.value(), Eq(-7));
}

TEST(ParsersTest, ParseIntNoDigits) {
  auto result = ParseInt("abc");
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST(ParsersTest, ParseUnsignedIntNoDigits) {
  auto result = ParseUnsignedInt("abc");
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST(ParsersTest, ParseUnsignedIntWithPlus) {
  auto result = ParseUnsignedInt("+42");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), Eq(42u));
}

TEST(ParsersTest, ParseFloatWithExponent) {
  auto result = ParseFloat("-1.5e2");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_NEAR(result.value(), -150.0f, 1e-4f);
}

TEST(ParsersTest, ParseFloatExponentSigns) {
  auto positive = ParseFloat("1e+2");
  ASSERT_THAT(positive.ok(), IsTrue());
  EXPECT_NEAR(positive.value(), 100.0f, 1e-4f);

  auto negative = ParseFloat("1e-2");
  ASSERT_THAT(negative.ok(), IsTrue());
  EXPECT_NEAR(negative.value(), 0.01f, 1e-4f);
}

TEST(ParsersTest, ConsumePrefixCases) {
  EXPECT_THAT(ConsumePrefix("", "abc").ok(), IsFalse());
  EXPECT_THAT(ConsumePrefix("ab", "").ok(), IsFalse());
  EXPECT_THAT(ConsumePrefix("abcd", "ab").ok(), IsFalse());
  EXPECT_THAT(ConsumePrefix("zz", "hello").ok(), IsFalse());
  auto result = ConsumePrefix("pre", "prefix");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), Eq("fix"));
}

TEST(ParsersTest, HasFileExtensionCases) {
  auto short_name = HasFileExtension("a", ".txt");
  EXPECT_THAT(short_name.ok(), IsTrue());
  EXPECT_THAT(short_name.value(), IsFalse());
  auto has_ext = HasFileExtension("file.txt", ".txt");
  EXPECT_THAT(has_ext.ok(), IsTrue());
  EXPECT_THAT(has_ext.value(), IsTrue());
  auto mismatch = HasFileExtension("file.txt", ".md");
  EXPECT_THAT(mismatch.ok(), IsTrue());
  EXPECT_THAT(mismatch.value(), IsFalse());
}

TEST(ParsersTest, FindFirstIndexOfCases) {
  auto result = FindFirstIndexOf("abc", "c");
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_THAT(result.value(), Eq(2u));
  auto missing = FindFirstIndexOf("abc", "z");
  EXPECT_THAT(missing.ok(), IsFalse());
}

TEST(ParsersTest, ConsumeUntilAndConsume) {
  unsigned int cursor = 0;
  auto until = ConsumeUntil("hello,world", &cursor, ",");
  ASSERT_THAT(until.ok(), IsTrue());
  EXPECT_THAT(until.value(), Eq("hello"));

  cursor = 0;
  auto stopped = Consume("abc,def", &cursor, ",", "abc");
  ASSERT_THAT(stopped.ok(), IsTrue());
  EXPECT_THAT(stopped.value(), Eq("abc"));

  cursor = 0;
  auto consumed = Consume("abc123", &cursor, "", "abc123");
  ASSERT_THAT(consumed.ok(), IsTrue());
  EXPECT_THAT(consumed.value(), Eq("abc123"));

  cursor = 0;
  auto invalid = Consume("abc$", &cursor, "", "abc");
  EXPECT_THAT(invalid.ok(), IsFalse());
}

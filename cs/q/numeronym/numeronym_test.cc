// cs/q/numeronym/numeronym_test.cc
#include "gmock/gmock.h"
#include "gtest/gtest.h"

extern "C" {
#include "numeronym.h"
}

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

// -------------------- InArrayBounds --------------------
TEST(InArrayBoundsTest, ValidIndices) {
  const char* s = "abc";
  EXPECT_THAT(InArrayBounds(s, 3, 0), IsTrue());
  EXPECT_THAT(InArrayBounds(s, 3, 2), IsTrue());
}

TEST(InArrayBoundsTest, InvalidIndices) {
  const char* s = "abc";
  EXPECT_THAT(InArrayBounds(s, 3, 3), IsFalse());
  EXPECT_THAT(InArrayBounds(s, 3, 10), IsFalse());
}

// -------------------- IsAlpha --------------------
TEST(IsAlphaTest, AlphabeticChars) {
  EXPECT_THAT(IsAlpha('a'), IsTrue());
  EXPECT_THAT(IsAlpha('z'), IsTrue());
  EXPECT_THAT(IsAlpha('A'), IsTrue());
  EXPECT_THAT(IsAlpha('Z'), IsTrue());
}

TEST(IsAlphaTest, NonAlphabeticChars) {
  EXPECT_THAT(IsAlpha('0'), IsFalse());
  EXPECT_THAT(IsAlpha('-'), IsFalse());
  EXPECT_THAT(IsAlpha(' '), IsFalse());
}

// -------------------- IsNumeral --------------------
TEST(IsNumeralTest, DigitChars) {
  for (char c = '0'; c <= '9'; ++c) {
    EXPECT_THAT(IsNumeral(c), IsTrue());
  }
}

TEST(IsNumeralTest, NonDigitChars) {
  EXPECT_THAT(IsNumeral('a'), IsFalse());
  EXPECT_THAT(IsNumeral('Z'), IsFalse());
  EXPECT_THAT(IsNumeral('-'), IsFalse());
}

// -------------------- TryParseUInt --------------------
TEST(TryParseUIntTest, SingleDigit) {
  char pattern[] = "5x";
  size_t i = 0, number = 0;
  EXPECT_THAT(TryParseUInt(pattern, &i, 2, &number),
              IsTrue());
  EXPECT_THAT(number, Eq(5u));
  EXPECT_THAT(i, Eq(0u));
}

TEST(TryParseUIntTest, MultipleDigits) {
  char pattern[] = "123abc";
  size_t i = 0, number = 0;
  EXPECT_THAT(TryParseUInt(pattern, &i, 6, &number),
              IsTrue());
  EXPECT_THAT(number, Eq(123u));
  EXPECT_THAT(i, Eq(2u));  // last digit index
}

TEST(TryParseUIntTest, LeadingZeros) {
  char pattern[] = "007bond";
  size_t i = 0, number = 0;
  EXPECT_THAT(TryParseUInt(pattern, &i, 7, &number),
              IsTrue());
  EXPECT_THAT(number, Eq(7u));  // still parses to 7
  EXPECT_THAT(i, Eq(2u));
}

TEST(TryParseUIntTest, NoDigits) {
  char pattern[] = "abc";
  size_t i = 0, number = 0;
  EXPECT_THAT(TryParseUInt(pattern, &i, 3, &number),
              IsFalse());
}

TEST(TryParseUIntTest, MidStringDigits) {
  char pattern[] = "a12b";
  size_t i = 1, number = 0;  // start at '1'
  EXPECT_THAT(TryParseUInt(pattern, &i, 4, &number),
              IsTrue());
  EXPECT_THAT(number, Eq(12u));
  EXPECT_THAT(i, Eq(2u));
}

// -------------------- ScrollForward --------------------
TEST(ScrollForwardTest, BasicForward) {
  const char* input = "Facebook";
  size_t j = 0;
  EXPECT_THAT(ScrollForward(input, &j, 8, 2), IsTrue());
  EXPECT_THAT(j, Eq(2u));
}

TEST(ScrollForwardTest, ExactEnd) {
  const char* input = "hi";
  size_t j = 0;
  EXPECT_THAT(ScrollForward(input, &j, 2, 2), IsTrue());
  EXPECT_THAT(j, Eq(2u));  // valid to land on end
}

TEST(ScrollForwardTest, ZeroAdvance) {
  const char* input = "abc";
  size_t j = 1;
  EXPECT_THAT(ScrollForward(input, &j, 3, 0), IsTrue());
  EXPECT_THAT(j, Eq(1u));  // unchanged
}

TEST(ScrollForwardTest, OutOfBoundsAdvance) {
  const char* input = "abc";
  size_t j = 2;
  EXPECT_THAT(ScrollForward(input, &j, 3, 5), IsFalse());
  EXPECT_THAT(j, Eq(2u));  // unchanged
}

TEST(ScrollForwardTest, AlreadyAtEnd) {
  const char* input = "abc";
  size_t j = 3;  // at end
  EXPECT_THAT(ScrollForward(input, &j, 3, 1), IsFalse());
  EXPECT_THAT(j, Eq(3u));
}

TEST(ScrollForwardTest, MidwayAdvance) {
  const char* input = "abcdef";
  size_t j = 2;  // at 'c'
  EXPECT_THAT(ScrollForward(input, &j, 6, 3), IsTrue());
  EXPECT_THAT(j, Eq(5u));  // lands on index 5 ('f')
}

TEST(ScrollForwardTest, EmptyInput) {
  const char* input = "";
  size_t j = 0;
  EXPECT_THAT(ScrollForward(input, &j, 0, 0), IsTrue());
  EXPECT_THAT(j, Eq(0u));
  EXPECT_THAT(ScrollForward(input, &j, 0, 1), IsFalse());
  EXPECT_THAT(j, Eq(0u));
}

// -------------------- IsNumeronym --------------------
TEST(IsNumeronymTest, ExactMatches) {
  EXPECT_THAT(IsNumeronym((char*)"Facebook", 8,
                          (char*)"Facebook", 8),
              IsTrue());
  EXPECT_THAT(IsNumeronym((char*)"a", 1, (char*)"a", 1),
              IsTrue());
  EXPECT_THAT(IsNumeronym((char*)"", 0, (char*)"", 0),
              IsTrue());
}

TEST(IsNumeronymTest, ValidAbbreviations) {
  EXPECT_THAT(
      IsNumeronym((char*)"F2eb2k", 6, (char*)"Facebook", 8),
      IsTrue());
  EXPECT_THAT(
      IsNumeronym((char*)"8", 1, (char*)"Facebook", 8),
      IsTrue());
  EXPECT_THAT(
      IsNumeronym((char*)"i18n", 4,
                  (char*)"internationalization", 20),
      IsTrue());
  EXPECT_THAT(IsNumeronym((char*)"1acebook", 8,
                          (char*)"Facebook", 8),
              IsTrue());
  EXPECT_THAT(IsNumeronym((char*)"Faceboo1", 8,
                          (char*)"Facebook", 8),
              IsTrue());
  EXPECT_THAT(IsNumeronym((char*)"F02eb2k", 7,
                          (char*)"Facebook", 8),
              IsTrue());  // leading zero
}

TEST(IsNumeronymTest, InvalidAbbreviations) {
  EXPECT_THAT(
      IsNumeronym((char*)"i5a5o5n", 8,
                  (char*)"internationalization", 20),
      IsFalse());
  EXPECT_THAT(
      IsNumeronym((char*)"F1book", 6, (char*)"Facebook", 8),
      IsFalse());  // too short
  EXPECT_THAT(IsNumeronym((char*)"Facebookk", 9,
                          (char*)"Facebook", 8),
              IsFalse());  // too long
  EXPECT_THAT(
      IsNumeronym((char*)"9", 1, (char*)"Facebook", 8),
      IsFalse());  // overshoot
  EXPECT_THAT(
      IsNumeronym((char*)"F2ab2k", 6, (char*)"Facebook", 8),
      IsFalse());  // wrong letters
  EXPECT_THAT(
      IsNumeronym((char*)"i19n", 4,
                  (char*)"internationalization", 20),
      IsFalse());  // wrong skip
}

TEST(IsNumeronymTest, EdgeCases) {
  EXPECT_THAT(IsNumeronym((char*)"", 0, (char*)"a", 1),
              IsFalse());  // empty pattern, non-empty input
  EXPECT_THAT(IsNumeronym((char*)"1", 1, (char*)"", 0),
              IsFalse());  // non-empty pattern, empty input
  EXPECT_THAT(
      IsNumeronym((char*)"abcd", 4, (char*)"abc", 3),
      IsFalse());  // pattern longer than input
  EXPECT_THAT(
      IsNumeronym((char*)"abc", 3, (char*)"abcd", 4),
      IsFalse());  // input longer than pattern
}

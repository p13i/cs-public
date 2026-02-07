// cs/renderer/numbers/numbers_test.gpt.cc
#include "cs/renderer/numbers/clamp.hh"
#include "cs/renderer/numbers/in_range.hh"
#include "gtest/gtest.h"

namespace cs::numbers {
namespace {

// --- clamp tests ---

TEST(ClampTest, WithinRange) {
  EXPECT_EQ(clamp(5, 0, 10), 5);
}

TEST(ClampTest, BelowMin) {
  EXPECT_EQ(clamp(-1, 0, 10), 0);
}

TEST(ClampTest, AboveMax) {
  EXPECT_EQ(clamp(15, 0, 10), 10);
}

TEST(ClampTest, AtMin) { EXPECT_EQ(clamp(0, 0, 10), 0); }

TEST(ClampTest, AtMax) { EXPECT_EQ(clamp(10, 0, 10), 10); }

TEST(ClampTest, FloatBelowMin) {
  EXPECT_FLOAT_EQ(clamp(-0.5f, 0.0f, 1.0f), 0.0f);
}

TEST(ClampTest, FloatAboveMax) {
  EXPECT_FLOAT_EQ(clamp(1.5f, 0.0f, 1.0f), 1.0f);
}

// --- in_range tests ---

TEST(InRangeTest, InsideRange) {
  EXPECT_TRUE(in_range(5, 0, 10));
}

TEST(InRangeTest, AtLower) {
  EXPECT_TRUE(in_range(0, 0, 10));
}

TEST(InRangeTest, AtUpper) {
  // Upper bound is exclusive.
  EXPECT_FALSE(in_range(10, 0, 10));
}

TEST(InRangeTest, BelowLower) {
  EXPECT_FALSE(in_range(-1, 0, 10));
}

TEST(InRangeTest, AboveUpper) {
  EXPECT_FALSE(in_range(11, 0, 10));
}

TEST(InRangeTest, FloatInside) {
  EXPECT_TRUE(in_range(0.5f, 0.0f, 1.0f));
}

TEST(InRangeTest, FloatAtUpper) {
  EXPECT_FALSE(in_range(1.0f, 0.0f, 1.0f));
}

}  // namespace
}  // namespace cs::numbers

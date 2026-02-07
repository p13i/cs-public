// cs/renderer/precision/floats_test.gpt.cc
#include "cs/renderer/precision/floats.hh"

#include "gtest/gtest.h"

namespace cs::renderer::precision {
namespace {

TEST(FloatsNearTest, ExactlyEqual) {
  EXPECT_TRUE(FloatsNear(1.0f, 1.0f));
}

TEST(FloatsNearTest, WithinEpsilon) {
  EXPECT_TRUE(FloatsNear(1.0f, 1.00005f));
}

TEST(FloatsNearTest, OutsideEpsilon) {
  EXPECT_FALSE(FloatsNear(1.0f, 1.001f));
}

TEST(FloatsNearTest, Zeros) {
  EXPECT_TRUE(FloatsNear(0.0f, 0.0f));
}

TEST(FloatsNearTest, NegativeValues) {
  EXPECT_TRUE(FloatsNear(-1.0f, -1.00005f));
}

TEST(FloatsNearTest, CustomEpsilon) {
  EXPECT_TRUE(FloatsNear(1.0f, 1.05f, 0.1f));
  EXPECT_FALSE(FloatsNear(1.0f, 1.2f, 0.1f));
}

}  // namespace
}  // namespace cs::renderer::precision

// cs/renderer/linalg/polar2cart_test.gpt.cc
#include "cs/renderer/linalg/polar2cart.hh"

#include <cmath>

#include "gtest/gtest.h"

namespace cs::renderer::linalg {
namespace {

static constexpr float kEps = 1e-4f;

TEST(Polar2CartTest, Origin) {
  auto p = polar2cart(0.0f, 0.0f, 0.0f);
  EXPECT_NEAR(p.x, 0.0f, kEps);
  EXPECT_NEAR(p.y, 0.0f, kEps);
  EXPECT_NEAR(p.z, 0.0f, kEps);
}

TEST(Polar2CartTest, AlongZ) {
  // theta=0 -> z-axis: x=0, y=0, z=r
  auto p = polar2cart(5.0f, 0.0f, 0.0f);
  EXPECT_NEAR(p.x, 0.0f, kEps);
  EXPECT_NEAR(p.y, 0.0f, kEps);
  EXPECT_NEAR(p.z, 5.0f, kEps);
}

TEST(Polar2CartTest, AlongX) {
  // theta=pi/2, phi=0 -> x-axis: x=r, y=0, z=0
  float pi2 = static_cast<float>(M_PI) / 2.0f;
  auto p = polar2cart(3.0f, pi2, 0.0f);
  EXPECT_NEAR(p.x, 3.0f, kEps);
  EXPECT_NEAR(p.y, 0.0f, kEps);
  EXPECT_NEAR(p.z, 0.0f, kEps);
}

TEST(Polar2CartTest, AlongY) {
  // theta=pi/2, phi=pi/2 -> y-axis: x=0, y=r, z=0
  float pi2 = static_cast<float>(M_PI) / 2.0f;
  auto p = polar2cart(4.0f, pi2, pi2);
  EXPECT_NEAR(p.x, 0.0f, kEps);
  EXPECT_NEAR(p.y, 4.0f, kEps);
  EXPECT_NEAR(p.z, 0.0f, kEps);
}

TEST(Polar2CartTest, NegativeZ) {
  // theta=pi -> -z axis: x=0, y=0, z=-r
  float pi = static_cast<float>(M_PI);
  auto p = polar2cart(2.0f, pi, 0.0f);
  EXPECT_NEAR(p.x, 0.0f, kEps);
  EXPECT_NEAR(p.y, 0.0f, kEps);
  EXPECT_NEAR(p.z, -2.0f, kEps);
}

}  // namespace
}  // namespace cs::renderer::linalg

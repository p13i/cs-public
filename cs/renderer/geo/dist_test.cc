// cs/renderer/geo/dist_test.cc
#include "cs/renderer/geo/dist.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/math/constants.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::dist;
using ::cs::renderer::geo::Point3;
using ::cs::renderer::precision::FloatsNear;
}  // namespace

TEST(Dist, Basic) {
  EXPECT_TRUE(FloatsNear(dist(Point3(), Point3(1, 1, 1)),
                         sqrtf(3)));
}

// See:
// https://www.wolframalpha.com/input?i=distance+from+%28-1%2C2.5%2C3.5%29+to+%28-5%2C2.5%2C-7.5%29
TEST(Dist, Basic2) {
  EXPECT_TRUE(FloatsNear(
      dist(Point3(-1, 2.5, 3.5), Point3(-5, 2.5, -7.5)),
      11.7047));
}

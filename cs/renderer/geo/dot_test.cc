// cs/renderer/geo/dot_test.cc
#include "cs/renderer/geo/dot.hh"

#include "cs/renderer/geo/point3.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::dot;
using ::cs::renderer::geo::Point3;
}  // namespace

TEST(Dot, Basic) {
  EXPECT_EQ(dot(Point3(1, 2, 3), Point3(4, 5, 6)),
            1 * 4 + 2 * 5 + 3 * 6);
}

// cs/renderer/geo/cross_test.cc
#include "cs/renderer/geo/cross.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::cross;
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Vector3;
}  // namespace

TEST(CrossProduct, CoordinateAxes) {
  EXPECT_EQ(cross(Vector3(Point3(1, 0, 0)),
                  Vector3(Point3(0, 1, 0))),
            Vector3(Point3(0, 0, 1)));
}

// cs/renderer/geo/point3_test.cc
#include "cs/renderer/geo/point3.h"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::testing::HasSubstr;
}  // namespace

TEST(Point3, OperatorAdd) {
  EXPECT_EQ(Point3(1, 2, 3) + Point3(4, 5, 6),
            Point3(5, 7, 9));
}

TEST(Point3, OperatorSubtract) {
  EXPECT_EQ(Point3(1, 2, 3) - Point3(4, 5, 6),
            Point3(-3, -3, -3));
}

TEST(Point3, OperatorMultiply) {
  EXPECT_EQ(Point3(1, 2, 3) * 3, Point3(3, 6, 9));
}

TEST(Point3, OperatorDivide) {
  EXPECT_EQ(Point3(1, 2, 3) / 3,
            Point3(1 / 3.f, 2 / 3.f, 3 / 3.f));
}

TEST(Point3, StreamOutput) {
  std::ostringstream os;
  os << Point3(1.0f, -2.0f, 3.0f);
  EXPECT_THAT(os.str(), HasSubstr("Point3("));
}

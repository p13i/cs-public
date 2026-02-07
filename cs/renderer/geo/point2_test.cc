// cs/renderer/geo/point2_test.cc
#include "cs/renderer/geo/point2.hh"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point2;
using ::testing::HasSubstr;
}  // namespace

TEST(Point2, OperatorAdd) {
  EXPECT_EQ(Point2(1, 2) + Point2(4, 5), Point2(5, 7));
}

TEST(Point3, OperatorSubtract) {
  EXPECT_EQ(Point2(1, 2) - Point2(4, 5), Point2(-3, -3));
}

TEST(Point3, OperatorMultiply) {
  EXPECT_EQ(Point2(1, 2) * 3, Point2(3, 6));
}

TEST(Point3, OperatorDivide) {
  EXPECT_EQ(Point2(1, 2) / 3, Point2(1 / 3.f, 2 / 3.f));
}

TEST(Point2, StreamOutput) {
  std::ostringstream os;
  os << Point2(1.5f, -2.0f);
  EXPECT_THAT(os.str(), HasSubstr("Point2("));
}

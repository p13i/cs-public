// cs/renderer/geo/vector3_test.cc
#include "cs/renderer/geo/vector3.h"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Vector3;
using ::testing::HasSubstr;
}  // namespace

TEST(Vector3, OneArgConstructor) {
  EXPECT_EQ(Vector3(Point3(1, 1, 1)), Vector3(1, 1, 1));
}

TEST(Vector3, OperatorAdd) {
  EXPECT_EQ(
      Vector3(Point3(1, 1, 1)) + Vector3(Point3(1, 2, 3)),
      Vector3(Point3(2, 3, 4)));
}

TEST(Vector3, OperatorSubtract) {
  EXPECT_EQ(
      Vector3(Point3(1, 1, 1)) - Vector3(Point3(1, 2, 3)),
      Vector3(Point3(0, -1, -2)));
}

TEST(Vector3, MagnitudeTest) {
  // Inline the variables for the test case
  Point3 a(1.0f, 2.0f, 3.0f);
  Point3 b(4.0f, 5.0f, 6.0f);
  Vector3 vec(b - a);

  // Calculate the magnitude manually (sqrt((4-1)^2 +
  // (5-2)^2 + (6-3)^2) = sqrt(27) ≈ 5.196152)
  EXPECT_FLOAT_EQ(vec.magnitude(), 5.196152f);
}

TEST(Vector3, Unit) {
  EXPECT_EQ(
      Vector3(Point3(1, 1, 1)).unit(),
      Vector3(Point3(0.57735026918962576450914878050196,
                     0.57735026918962576450914878050196,
                     0.57735026918962576450914878050196)));
}

TEST(Vector3, StreamOutput) {
  std::ostringstream os;
  os << Vector3(1.0f, 2.0f, 3.0f);
  EXPECT_THAT(os.str(), HasSubstr("Vector3("));
}

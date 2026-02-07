// cs/renderer/geo/ray3_test.cc
#include "cs/renderer/geo/ray3.h"

#include "cs/renderer/math/constants.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Ray3;
using ::cs::renderer::geo::Vector3;
}  // namespace

TEST(Ray3, DirectionVectorIsUnit) {
  Ray3 ray(Point3(), Point3(1, 1, 1));
  EXPECT_EQ(ray.direction, ray.direction.unit());
  EXPECT_EQ(ray.direction,
            Vector3(0.57735026918962576450914878050196,
                    0.57735026918962576450914878050196,
                    0.57735026918962576450914878050196));
}

TEST(Ray3, OperatorTimeOneComponent) {
  Ray3 ray(Point3(), Point3(1, 0, 0));
  EXPECT_EQ(ray(3), Point3(3, 0, 0));
}

TEST(Ray3, OperatorTimeOnUnitVector) {
  Ray3 ray(Point3(), Point3(1, 1, 1));
  EXPECT_EQ(ray(3),
            Point3(1.7320508075688772935274463415059,
                   1.7320508075688772935274463415059,
                   1.7320508075688772935274463415059));
}

TEST(Ray3, ThetaIsPiOver2) {
  Ray3 ray(Point3(), PIf / 2.f, 0);
  Vector3 expected(1, 0, 0);
  EXPECT_EQ(ray.direction, expected);
}

TEST(Ray3, ThetaIsPiOver4) {
  Ray3 ray(Point3(), PIf / 4.f, 0);
  Vector3 expected(
      Point3(sqrtf(2) / 2.f, 0, sqrtf(2) / 2.f));
  EXPECT_EQ(ray.direction, expected);
}

TEST(Ray3, PiOver2PhiStillPointsToZ) {
  Ray3 ray(Point3(), 0, PIf / 2.f);
  Vector3 expected(Point3(0, 0, 1));
  EXPECT_EQ(ray.direction, expected);
}

TEST(Ray3, ThetaAndPhiArePiOver2) {
  Ray3 ray(Point3(), PIf / 2.f, PIf / 2.f);
  Vector3 expected(Point3(0, 1, 0));
  EXPECT_EQ(ray.direction, expected);
}

TEST(Ray3, ThetaAndPhiArePiOver4) {
  Ray3 ray(Point3(), PIf / 4.f, PIf / 4.f);
  Vector3 expected(
      Point3(1 / 2.f, 1 / 2.f, sqrtf(2) / 2.f));
  EXPECT_EQ(ray.direction, expected);
}

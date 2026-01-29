// cs/renderer/shapes/plane_test.cc
#include "cs/renderer/shapes/plane.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;
using r3 = ::cs::renderer::geo::Ray3;

using ::cs::renderer::shapes::Plane;

TEST(Plane, IntersectionOnXAxis) {
  r3 ray(p3(), p3(1, 0, 0));
  Plane plane(p3(1, 0, 0).unit(), -2);

  p3 intersection;
  v3 normal;
  EXPECT_TRUE(
      plane.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection, p3(2, 0, 0));
  EXPECT_EQ(normal, v3(p3(-1, 0, 0)));
}

TEST(Plane, RejectsNonUnitNormal) {
  r3 ray(p3(), p3(1, 0, 0));
  Plane plane(p3(2, 0, 0), -2);
  p3 intersection;
  v3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsNonUnitRay) {
  r3 ray(p3(), p3(1, 0, 0));
  ray.direction = v3(1.0f, 1.0f, 0.0f);
  Plane plane(p3(1, 0, 0).unit(), -2);
  p3 intersection;
  v3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsParallelRay) {
  r3 ray(p3(), p3(1, 0, 0));
  Plane plane(p3(0, 1, 0).unit(), -2);
  p3 intersection;
  v3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsIntersectionBehindRay) {
  r3 ray(p3(), p3(-1, 0, 0));
  Plane plane(p3(1, 0, 0).unit(), -2);
  p3 intersection;
  v3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

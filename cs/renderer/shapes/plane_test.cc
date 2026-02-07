// cs/renderer/shapes/plane_test.cc
#include "cs/renderer/shapes/plane.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Ray3;
using ::cs::renderer::geo::Vector3;
using ::cs::renderer::shapes::Plane;
}  // namespace

TEST(Plane, IntersectionOnXAxis) {
  Ray3 ray(Point3(), Point3(1, 0, 0));
  Plane plane(Point3(1, 0, 0).unit(), -2);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      plane.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection, Point3(2, 0, 0));
  EXPECT_EQ(normal, Vector3(Point3(-1, 0, 0)));
}

TEST(Plane, RejectsNonUnitNormal) {
  Ray3 ray(Point3(), Point3(1, 0, 0));
  Plane plane(Point3(2, 0, 0), -2);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsNonUnitRay) {
  Ray3 ray(Point3(), Point3(1, 0, 0));
  ray.direction = Vector3(1.0f, 1.0f, 0.0f);
  Plane plane(Point3(1, 0, 0).unit(), -2);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsParallelRay) {
  Ray3 ray(Point3(), Point3(1, 0, 0));
  Plane plane(Point3(0, 1, 0).unit(), -2);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

TEST(Plane, RejectsIntersectionBehindRay) {
  Ray3 ray(Point3(), Point3(-1, 0, 0));
  Plane plane(Point3(1, 0, 0).unit(), -2);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      plane.intersected_by(ray, &intersection, &normal));
}

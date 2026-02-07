// cs/renderer/shapes/sphere_test.cc
#include "cs/renderer/shapes/sphere.hh"

#include "cs/renderer/math/constants.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Ray3;
using ::cs::renderer::geo::Vector3;
using ::cs::renderer::shapes::Sphere;
}  // namespace

TEST(Sphere, IntersectionOnXAxis) {
  Ray3 ray(Point3(0, 0, 0), Point3(1, 0, 0));
  Sphere sphere(Point3(5, 0, 0), 0.5);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      sphere.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection, Point3(4.5, 0, 0));
  EXPECT_EQ(normal, Vector3(Point3(-1, 0, 0)));
}

TEST(Sphere, IntersectionOnYAxis) {
  Ray3 ray(Point3(0, 0, 0), Point3(0, 1, 0));
  Sphere sphere(Point3(0, 5, 0), 1);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      sphere.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection, Point3(0, 4, 0));
  EXPECT_EQ(normal, Vector3(Point3(0, -1, 0)));
}

TEST(Sphere, IntersectionOnZAxis) {
  Ray3 ray(Point3(0, 0, 0), Point3(0, 0, 1));
  Sphere sphere(Point3(0, 0, 5), 1);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      sphere.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection, Point3(0, 0, 4));
  EXPECT_EQ(normal, Vector3(Point3(0, 0, -1)));
}

TEST(Sphere, IntersectionOnXYPlane) {
  Ray3 ray(Point3(0, 0, 0), Point3(1, 1, 0));
  Sphere sphere(Point3(5, 5, 0), 1);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      sphere.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection,
            Point3(5 - sqrtf(2) / 2, 5 - sqrtf(2) / 2, 0));
  EXPECT_EQ(normal, Vector3(Point3(-sqrtf(2) / 2,
                                   -sqrtf(2) / 2, 0)));
}

TEST(Sphere, IntersectionInQuadrantOne) {
  Ray3 ray(Point3(0, 0, 0), Point3(1, 1, 1));
  Sphere sphere(Point3(5, 5, 5), 1);

  Point3 intersection;
  Vector3 normal;
  EXPECT_TRUE(
      sphere.intersected_by(ray, &intersection, &normal));

  EXPECT_EQ(intersection,
            Point3(5 - sqrtf(3) / 3, 5 - sqrtf(3) / 3,
                   5 - sqrtf(3) / 3));
  EXPECT_EQ(normal,
            Vector3(Point3(-sqrtf(3) / 3, -sqrtf(3) / 3,
                           -sqrtf(3) / 3)));
}

TEST(Sphere, RejectsNonUnitRay) {
  Ray3 ray(Point3(0, 0, 0), Point3(1, 0, 0));
  ray.direction = Vector3(2.0f, 0.0f, 0.0f);
  Sphere sphere(Point3(5, 0, 0), 1);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      sphere.intersected_by(ray, &intersection, &normal));
}

TEST(Sphere, RejectsMissingSphere) {
  Ray3 ray(Point3(0, 0, 0), Point3(1, 0, 0));
  Sphere sphere(Point3(0, 0, 5), 1);
  Point3 intersection;
  Vector3 normal;
  EXPECT_FALSE(
      sphere.intersected_by(ray, &intersection, &normal));
}

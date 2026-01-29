// cs/renderer/shapes/sphere.cc
#include "cs/renderer/shapes/sphere.hh"

#include <cmath>

#include "cs/renderer/geo/dot.hh"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/precision/floats.hh"
#include "cs/renderer/shapes/shape.hh"

using r3 = ::cs::renderer::geo::Ray3;
using v3 = ::cs::renderer::geo::Vector3;
using p3 = ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::dot;
using ::cs::renderer::precision::FloatsNear;

bool cs::renderer::shapes::Sphere::intersected_by(
    r3 ray, p3* at_point, v3* at_normal) {
  float a = dot(ray.direction, ray.direction);
  float b = 2 * dot(ray.direction, (ray.origin - center));
  float c =
      dot((ray.origin - center), (ray.origin - center)) -
      radius * radius;

  if (!FloatsNear(a, 1.f)) {
    return false;
  }

  float discriminant = b * b - 4 * c;
  if (discriminant < 0.f) {
    return false;
  }

  float t_intersect;
  float t0 = (-b + sqrtf(discriminant)) / 2.f;
  float t1 = (-b - sqrtf(discriminant)) / 2.f;
  // Intersection time must be the positve solution
  if (t0 < t1) {
    t_intersect = t0;  // LCOV_EXCL_LINE
  } else {
    t_intersect = t1;
  }

  p3 intersection = ray(t_intersect);
  v3 normal = ((intersection - center) / radius);
  normal = normal.unit();

  *at_point = intersection;
  *at_normal = normal;
  return true;
};

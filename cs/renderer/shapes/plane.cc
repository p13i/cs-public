#include "cs/renderer/shapes/plane.hh"

#include "cs/renderer/geo/dot.hh"
#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/precision/floats.hh"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;
using r3 = ::cs::renderer::geo::Ray3;
using ::cs::renderer::geo::dot;

bool cs::renderer::shapes::Plane::intersected_by(
    r3 ray, p3* at_point, v3* at_normal) {
  v3 unit_normal(p3(a, b, c));  // p_n
  if (!unit_normal.is_unit()) {
    return false;
  }
  if (!ray.direction.is_unit()) {
    return false;
  }
  float v_d = dot(unit_normal, ray.direction);
  if (FloatsNear(v_d, 0)) {
    // Parallel
    return false;
  }
  float v_0 = -1 * (dot(unit_normal, ray.origin) + d);
  float t = v_0 / v_d;
  if (t < 0) {
    // Intersection behind ray origin
    return false;
  }
  *at_point = ray(t);
  if (v_d > 0) {
    // Normal is facing away so flip it
    unit_normal = unit_normal * -1.f;
  }
  *at_normal = unit_normal;
  return true;
}

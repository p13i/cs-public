// cs/renderer/shapes/sphere.hh
#ifndef CS_RENDERER_SHAPES_SPHERE_HH
#define CS_RENDERER_SHAPES_SPHERE_HH

#include <cmath>

#include "cs/renderer/geo/dot.hh"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/math/constants.h"
#include "cs/renderer/shapes/shape.hh"

using r3 = ::cs::renderer::geo::Ray3;
using v3 = ::cs::renderer::geo::Vector3;
using p3 = ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::dot;

namespace cs::renderer::shapes {
struct Sphere : public Shape {
  p3 center;
  float radius;

  Sphere(p3 center, float radius)
      : center(center), radius(radius) {}

  bool intersected_by(r3 ray, p3* at_point,
                      v3* at_normal) override;
};
}  // namespace cs::renderer::shapes

#endif  // CS_RENDERER_SHAPES_SPHERE_HH
// cs/renderer/shapes/plane.hh
#ifndef CS_RENDERER_SHAPES_PLANE_HH
#define CS_RENDERER_SHAPES_PLANE_HH

#include <cmath>

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/shapes/shape.hh"

using r3 = ::cs::renderer::geo::Ray3;
using v3 = ::cs::renderer::geo::Vector3;
using p3 = ::cs::renderer::geo::Point3;

namespace cs::renderer::shapes {
struct Plane : public Shape {
  float a, b, c, d;

  Plane(p3 abc, float d) : Plane(abc.x, abc.y, abc.z, d) {}

  Plane(float a, float b, float c, float d)
      : a(a), b(b), c(c), d(d) {}

  bool intersected_by(r3 ray, p3* at_point,
                      v3* at_normal) override;
};
}  // namespace cs::renderer::shapes

#endif  // CS_RENDERER_SHAPES_PLANE_HH
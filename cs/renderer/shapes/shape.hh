// cs/renderer/shapes/shape.hh
#ifndef CS_RENDERER_SHAPES_SHAPE_HH
#define CS_RENDERER_SHAPES_SHAPE_HH

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/geo/vector3.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;
using r3 = ::cs::renderer::geo::Ray3;

namespace cs::renderer::shapes {
class Shape {
 public:
  virtual bool intersected_by(r3 ray, p3* at_point,
                              v3* at_normal) = 0;
  virtual ~Shape() = default;
};
};  // namespace cs::renderer::shapes

#endif  // CS_RENDERER_SHAPES_SHAPE_HH
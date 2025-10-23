#ifndef CS_RENDERER_GEO_DOT_HH
#define CS_RENDERER_GEO_DOT_HH

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;

namespace cs::renderer::geo {
// Performs a standard dot product.
float dot(const p3& a, const p3& b);
float dot(const v3& a, const v3& b);
}  // namespace cs::renderer::geo

#endif  // CS_RENDERER_GEO_DOT_HH
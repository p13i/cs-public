#ifndef CS_RENDERER_GEO_CROSS_HH
#define CS_RENDERER_GEO_CROSS_HH

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;

namespace cs::renderer::geo {
v3 cross(v3 uno, v3 dos);
}  // namespace cs::renderer::geo

#endif  // CS_RENDERER_GEO_CROSS_HH
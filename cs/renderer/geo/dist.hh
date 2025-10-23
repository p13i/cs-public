#ifndef CS_RENDERER_GEO_DIST_HH
#define CS_RENDERER_GEO_DIST_HH

#include "cs/renderer/geo/point3.h"

namespace cs::renderer::geo {

using p3 = ::cs::renderer::geo::Point3;

float dist(p3 a, p3 b);

}  // namespace cs::renderer::geo

#endif  // CS_RENDERER_GEO_DIST_HH
#ifndef CS_RENDERER_LINALG_POLAR2CART_HH
#define CS_RENDERER_LINALG_POLAR2CART_HH

#include <cmath>

#include "cs/renderer/geo/point3.h"

using ::cs::renderer::geo::Point3;

namespace cs::renderer::linalg {
Point3 polar2cart(float r, float theta, float phi);
}  // namespace cs::renderer::linalg

#endif  // CS_RENDERER_LINALG_POLAR2CART_HH
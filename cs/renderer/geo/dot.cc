#include "cs/renderer/geo/dot.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/math/constants.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;

namespace cs::renderer::geo {
// Performs a standard dot product.
float dot(const p3& a, const p3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(const v3& a, const v3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
}  // namespace cs::renderer::geo

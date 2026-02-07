// cs/renderer/geo/dot.cc
#include "cs/renderer/geo/dot.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/math/constants.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Vector3;
}  // namespace

namespace cs::renderer::geo {
// Performs a standard dot product.
float dot(const Point3& a, const Point3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(const Vector3& a, const Vector3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
}  // namespace cs::renderer::geo

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"

namespace cs::renderer::geo {

float dist(p3 a, p3 b) {
  p3 diff = b - a;
  float x2 = diff.x * diff.x;
  float y2 = diff.y * diff.y;
  float z2 = diff.z * diff.z;
  return sqrtf(x2 + y2 + z2);
}

}  // namespace cs::renderer::geo

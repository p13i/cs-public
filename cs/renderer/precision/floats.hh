// cs/renderer/precision/floats.hh
#ifndef CS_RENDERER_PRECISION_FLOATS_HH
#define CS_RENDERER_PRECISION_FLOATS_HH

#include <cmath>

namespace cs::renderer::precision {
constexpr float kFloatEpsilon = 0.0001f;
inline bool FloatsNear(float a, float b,
                       float epsilon = kFloatEpsilon) {
  return std::fabs(a - b) < epsilon;
}
};  // namespace cs::renderer::precision

#endif  // CS_RENDERER_PRECISION_FLOATS_HH
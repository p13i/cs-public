// cs/renderer/audio/sinusodal.cc
#include "cs/renderer/audio/sinusodal.h"

#include <cmath>

#include "cs/renderer/math/constants.h"

float cs::audio::sinusodal(float freqencyHz, float phase) {
  return static_cast<float>(
      sinf(freqencyHz * 2.f * PIf * phase));
};
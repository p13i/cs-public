// cs/renderer/audio/sinusodal_test.cc
#include "cs/renderer/audio/sinusodal.h"

#include "cs/renderer/precision/floats.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::audio::sinusodal;
using ::cs::renderer::precision::FloatsNear;
}  // namespace

TEST(Sinusodal, Zero) {
  EXPECT_TRUE(FloatsNear(sinusodal(440, 0), 0));
}

TEST(Sinusodal, HalfPhase) {
  EXPECT_TRUE(FloatsNear(sinusodal(440, 0.5), 0));
}

// cs/apps/trycopilot.ai/text/fonts/mono_test.gpt.cc
#include "cs/apps/trycopilot.ai/text/fonts/mono.hh"

#include "gtest/gtest.h"

using ::cs::apps::trycopilotai::text::fonts::
    SampleCharacterPixel;

TEST(MonoFontTest, SamplesLettersAndSpace) {
  EXPECT_TRUE(SampleCharacterPixel('A', 3, 0));
  EXPECT_FALSE(SampleCharacterPixel('A', 0, 0));
  EXPECT_FALSE(SampleCharacterPixel(' ', 0, 0));
  EXPECT_FALSE(SampleCharacterPixel('#', 0, 0));
}

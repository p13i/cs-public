// cs/apps/trycopilot.ai/fps_counter_test.gpt.cc
#include "cs/apps/trycopilot.ai/fps_counter.hh"

#include "gtest/gtest.h"

namespace cs::apps::trycopilotai {
namespace {

TEST(AppFpsCounterTest, NoFramesReturnNegative) {
  AppFpsCounter counter(60);
  EXPECT_FLOAT_EQ(counter.fps(), -1.0f);
}

TEST(AppFpsCounterTest, OneFrameReturnNegative) {
  AppFpsCounter counter(60);
  counter.newFrame();
  EXPECT_FLOAT_EQ(counter.fps(), -1.0f);
}

TEST(AppFpsCounterTest, NewFrameDoesNotCrash) {
  AppFpsCounter counter(30);
  for (int i = 0; i < 100; ++i) {
    counter.newFrame();
  }
  // Just verifies no crash or overflow.
}

TEST(AppFpsCounterTest, ZeroFpsQueueSize) {
  // fps=0 means maxQueueSize=0, so each newFrame
  // pops the queue to 0 before pushing.
  AppFpsCounter counter(0);
  counter.newFrame();
  counter.newFrame();
  // Should not crash; fps() returns -1 or a real
  // value depending on queue size.
}

}  // namespace
}  // namespace cs::apps::trycopilotai

// cs/apps/trycopilot.ai/scene_animator_test.gpt.cc
// cs/apps/trycopilot.ai/scene_animator_test.cc
#include "cs/apps/trycopilot.ai/scene_animator.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::apps::trycopilotai::SceneAnimator;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::Gt;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

class SceneAnimatorTest : public ::testing::Test {
 protected:
  SceneAnimatorTest() {}
  void RenderTest(unsigned int width, unsigned int height,
                  unsigned int num_frames) {
    // Build scene
    std::tuple<uint, uint> film_dimensions(width, height);
    SceneAnimator animator(num_frames, film_dimensions);
    // Render out frames
    std::vector<Film> frames;
    [[maybe_unused]]
    const auto render_time_ms =
        cs::util::timeit([&frames, &animator]() {
          frames = animator.render_all_frames();
        });
    EXPECT_THAT(render_time_ms, Gt(0));
  }
};

#define RENDER_TEST(width, height, num_frames) \
  TEST_F(SceneAnimatorTest,                    \
         At##width##x##height##num_frames) {   \
    RenderTest(width, height, num_frames);     \
  }

RENDER_TEST(256, 256, 1);

RENDER_TEST(255, 256, 1);

RENDER_TEST(250, 256, 1);

RENDER_TEST(240, 256, 1);

RENDER_TEST(230, 255, 1);

RENDER_TEST(225, 255, 1);

RENDER_TEST(224, 255, 1);

RENDER_TEST(223, 255, 1);

RENDER_TEST(200, 255, 1);

TEST_F(SceneAnimatorTest, CallbackInvoked) {
  std::tuple<uint, uint> film_dimensions(8, 8);
  SceneAnimator animator(1, film_dimensions);
  unsigned int callbacks = 0;
  auto frames = animator.render_all_frames(
      [&](unsigned int /*render_time_ms*/,
          unsigned int index, unsigned int total) {
        callbacks++;
        EXPECT_EQ(index, 1u);
        EXPECT_EQ(total, 1u);
      });
  EXPECT_EQ(callbacks, 1u);
  EXPECT_EQ(frames.size(), 1u);
}

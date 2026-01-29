// cs/renderer/pixel_test.gpt.cc
#include "cs/renderer/pixel.hh"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::renderer::Pixel;
using ::testing::HasSubstr;

TEST(PixelTest, StreamOutput) {
  Pixel px(1, 2, 3, 4);
  std::ostringstream os;
  os << px;
  EXPECT_THAT(os.str(), HasSubstr("Pixel("));
}

TEST(PixelTest, IndexAccess) {
  Pixel px(10, 11, 12, 13);
  EXPECT_EQ(px[0], 10);
  EXPECT_EQ(px[1], 11);
  EXPECT_EQ(px[2], 12);
  EXPECT_EQ(px[3], 13);
}

TEST(PixelTest, IndexOutOfRange) {
  Pixel px(1, 2, 3, 4);
  EXPECT_DEATH({ (void)px[9]; }, ".*");
}

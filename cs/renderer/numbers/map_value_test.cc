// cs/renderer/numbers/map_value_test.cc
#include "cs/renderer/numbers/map_value.hh"

#include "cs/renderer/numbers/in_range.hh"
#include "cs/renderer/precision/floats.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::numbers::in_range;
using ::cs::numbers::map_value;
using ::cs::renderer::precision::FloatsNear;

TEST(MapValueTest, TestInRange) {
  float value = 50.0f;
  float value_range_start = 0.0f;
  float value_range_end = 100.0f;
  float new_range_start = 10.0f;
  float new_range_end = 200.0f;

  float mapped_value =
      map_value(value, value_range_start, value_range_end,
                new_range_start, new_range_end);

  EXPECT_TRUE(FloatsNear(mapped_value, 105.0f));
}

TEST(InRangeTest, LowerInclusiveUpperExclusive) {
  EXPECT_TRUE(in_range(0, 0, 3));
  EXPECT_TRUE(in_range(2, 0, 3));
  EXPECT_FALSE(in_range(3, 0, 3));
  EXPECT_FALSE(in_range(-1, 0, 3));
}

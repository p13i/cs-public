// cs/q/trees/iterate_base_test.gpt.cc
#include "cs/q/trees/iterate.hh"
#include "gtest/gtest.h"

TEST(TreeIteratorTest, DefaultsReturnEmpty) {
  cs::q::trees::TreeIterator<int> iter(nullptr);
  EXPECT_FALSE(iter.HasNext());
  EXPECT_EQ(iter.Next(), 0);
}

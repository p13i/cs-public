// cs/q/trees/iterate_base_test.gpt.cc
#include "cs/q/trees/iterate.hh"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::q::trees::TreeIterator;
}  // namespace

TEST(TreeIteratorTest, DefaultsReturnEmpty) {
  TreeIterator<int> iter(nullptr);
  EXPECT_FALSE(iter.HasNext());
  EXPECT_EQ(iter.Next(), 0);
}

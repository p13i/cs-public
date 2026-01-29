// cs/q/heap/heap_cpp_test.gpt.cc
#include <limits.h>

#include "cs/q/heap/heap.hh"
#include "gtest/gtest.h"

TEST(MaxHeapTest, PeekEmptyReturnsMin) {
  cs::q::MaxHeap<int> heap(3);
  auto value = heap.Peek();
  EXPECT_EQ(value.second, INT_MIN);
}

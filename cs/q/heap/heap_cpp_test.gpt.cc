// cs/q/heap/heap_cpp_test.gpt.cc
#include <limits.h>

#include "cs/q/heap/heap.hh"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::q::MaxHeap;
}  // namespace

TEST(MaxHeapTest, PeekEmptyReturnsMin) {
  MaxHeap<int> heap(3);
  auto value = heap.Peek();
  EXPECT_EQ(value.second, INT_MIN);
}

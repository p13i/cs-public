// cs/q/heap/heap_test.cc
extern "C" {
#include "heap.h"

#include "limits.h"
}

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "heap.hh"

namespace {  // use_usings
using ::cs::q::MaxHeap;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::Not;
using ::testing::StrEq;
}  // namespace

namespace {
TEST(NewHeap, Success) {
  int capacity = 3;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(heap, Not(Eq((Heap*)NULL)));
  EXPECT_THAT(heap->capacity, Eq(3));
  EXPECT_THAT(heap->size, Eq(0));
  EXPECT_THAT(heap->values, Not(Eq((int*)NULL)));
}

TEST(HeapifyUp, NoChange) {
  int capacity = 3;
  Heap* heap = NewHeap(capacity);
  heap->values[1] = 10;
  heap->values[2] = 9;
  heap->values[3] = 8;
  HeapifyUp(heap, 1);
  EXPECT_THAT(heap->values[1], Eq(10));
  EXPECT_THAT(heap->values[2], Eq(9));
  EXPECT_THAT(heap->values[3], Eq(8));
}

TEST(Heap, SortsValues) {
  int capacity = 3;
  Heap* heap = NewHeap(capacity);
  // 2 -> 3 -> 1 should be saved as [0, 3, 2, 1]
  EXPECT_THAT(Insert(heap, 2), IsTrue());
  EXPECT_THAT(Insert(heap, 3), IsTrue());
  EXPECT_THAT(Insert(heap, 1), IsTrue());
  EXPECT_THAT(Insert(heap, 1), IsFalse());
  EXPECT_THAT(heap->values[0], Eq(0));
  EXPECT_THAT(heap->values[1], Eq(3));
  EXPECT_THAT(heap->values[2], Eq(2));
  EXPECT_THAT(heap->values[3], Eq(1));
  // Pop elements
  EXPECT_THAT(PeekMax(heap), Eq(3));
  EXPECT_THAT(PopMax(heap), Eq(3));
  EXPECT_THAT(PopMax(heap), Eq(2));
  EXPECT_THAT(PopMax(heap), Eq(1));
  EXPECT_THAT(PopMax(heap), Eq(INT_MIN));
}

// 1. Small heap, single insert
TEST(Heap, SingleInsertAndPop) {
  int capacity = 1;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(Insert(heap, 42), IsTrue());
  EXPECT_THAT(PeekMax(heap), Eq(42));
  EXPECT_THAT(PopMax(heap), Eq(42));
  EXPECT_THAT(PopMax(heap), Eq(INT_MIN));
}

// 2. Insertion beyond capacity
TEST(Heap, CapacityExceeded) {
  int capacity = 2;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(Insert(heap, 5), IsTrue());
  EXPECT_THAT(Insert(heap, 10), IsTrue());
  EXPECT_THAT(Insert(heap, 3),
              IsFalse());  // exceeds capacity
}

// 3. Descending order after mixed inserts
TEST(Heap, SortsDescending) {
  int capacity = 4;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(Insert(heap, 7), IsTrue());
  EXPECT_THAT(Insert(heap, 1), IsTrue());
  EXPECT_THAT(Insert(heap, 9), IsTrue());
  EXPECT_THAT(Insert(heap, 3), IsTrue());
  EXPECT_THAT(PopMax(heap), Eq(9));
  EXPECT_THAT(PopMax(heap), Eq(7));
  EXPECT_THAT(PopMax(heap), Eq(3));
  EXPECT_THAT(PopMax(heap), Eq(1));
}

// 4. Duplicate values allowed
TEST(Heap, HandlesDuplicates) {
  int capacity = 5;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(Insert(heap, 4), IsTrue());
  EXPECT_THAT(Insert(heap, 4), IsTrue());
  EXPECT_THAT(Insert(heap, 4), IsTrue());
  EXPECT_THAT(PopMax(heap), Eq(4));
  EXPECT_THAT(PopMax(heap), Eq(4));
  EXPECT_THAT(PopMax(heap), Eq(4));
}

// 5. Pop from empty
TEST(Heap, EmptyPop) {
  int capacity = 2;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(PopMax(heap), Eq(INT_MIN));
}

// 6. Peek from empty
TEST(Heap, EmptyPeek) {
  int capacity = 2;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(PeekMax(heap), Eq(INT_MIN));
}

// 7. Min and Max tracked correctly
TEST(Heap, PeekMax) {
  int capacity = 3;
  Heap* heap = NewHeap(capacity);
  Insert(heap, 10);
  Insert(heap, 20);
  Insert(heap, 5);
  EXPECT_THAT(PeekMax(heap), Eq(20));
}

// 8. Insert decreasing sequence
TEST(Heap, InsertDescending) {
  int capacity = 5;
  Heap* heap = NewHeap(capacity);
  EXPECT_TRUE(Insert(heap, 5));
  EXPECT_TRUE(Insert(heap, 4));
  EXPECT_TRUE(Insert(heap, 3));
  EXPECT_TRUE(Insert(heap, 2));
  EXPECT_TRUE(Insert(heap, 1));
  EXPECT_THAT(PopMax(heap), Eq(5));
  EXPECT_THAT(PopMax(heap), Eq(4));
  EXPECT_THAT(PopMax(heap), Eq(3));
  EXPECT_THAT(PopMax(heap), Eq(2));
  EXPECT_THAT(PopMax(heap), Eq(1));
}

// 9. Insert increasing sequence
TEST(Heap, InsertAscending) {
  int capacity = 5;
  Heap* heap = NewHeap(capacity);
  Insert(heap, 1);
  Insert(heap, 2);
  Insert(heap, 3);
  Insert(heap, 4);
  Insert(heap, 5);
  EXPECT_THAT(PopMax(heap), Eq(5));
  EXPECT_THAT(PopMax(heap), Eq(4));
  EXPECT_THAT(PopMax(heap), Eq(3));
  EXPECT_THAT(PopMax(heap), Eq(2));
  EXPECT_THAT(PopMax(heap), Eq(1));
}

// 10. Stress small capacity
TEST(Heap, StressTinyHeap) {
  int capacity = 1;
  Heap* heap = NewHeap(capacity);
  EXPECT_THAT(Insert(heap, 100), IsTrue());
  EXPECT_THAT(Insert(heap, 200), IsFalse());
  EXPECT_THAT(PopMax(heap), Eq(100));
  EXPECT_THAT(PopMax(heap), Eq(INT_MIN));
}

// 11. Negative numbers
TEST(Heap, HandlesNegativeValues) {
  int capacity = 4;
  Heap* heap = NewHeap(capacity);
  Insert(heap, -5);
  Insert(heap, -1);
  Insert(heap, -10);
  Insert(heap, -3);
  EXPECT_THAT(PopMax(heap), Eq(-1));
  EXPECT_THAT(PopMax(heap), Eq(-3));
  EXPECT_THAT(PopMax(heap), Eq(-5));
  EXPECT_THAT(PopMax(heap), Eq(-10));
}

// 12. Interleaved inserts and pops
TEST(Heap, InterleavedOps) {
  int capacity = 5;
  Heap* heap = NewHeap(capacity);
  Insert(heap, 8);
  Insert(heap, 2);
  EXPECT_THAT(PopMax(heap), Eq(8));
  Insert(heap, 10);
  Insert(heap, 1);
  EXPECT_THAT(PopMax(heap), Eq(10));
  EXPECT_THAT(PopMax(heap), Eq(2));
  EXPECT_THAT(PopMax(heap), Eq(1));
}

// Test C++ implementation
TEST(MaxHeap, Basic) {
  MaxHeap<int> heap{3};
  EXPECT_TRUE(heap.Insert(3, 3));
  EXPECT_TRUE(heap.Insert(2, 2));
  EXPECT_TRUE(heap.Insert(1, 1));
  EXPECT_FALSE(heap.Insert(1, 1));
  EXPECT_THAT(heap.Pop(), FieldsAre(3, 3));
  EXPECT_THAT(heap.Pop(), FieldsAre(2, 2));
  EXPECT_THAT(heap.Pop(), FieldsAre(1, 1));
  EXPECT_THAT(heap.Pop(), FieldsAre(0, INT_MIN));
}

}  // namespace
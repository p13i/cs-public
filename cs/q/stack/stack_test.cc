
#include "stack.hh"

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
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

namespace {}  // namespace

#include <memory>
#include <optional>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "stack.hh"  // your Stack<T> interface

using cs::q::stack::Stack;
using ::testing::Eq;
using ::testing::Optional;

// ---------- Basic behavior ----------
TEST(StackTest, NewlyCreatedStackIsEmpty) {
  Stack<int> s;
  EXPECT_THAT(s.Size(), Eq(0u));
  EXPECT_THAT(s.PeekFront(), Eq(std::nullopt));
  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
}

TEST(StackTest, PushSingleElement) {
  Stack<int> s;
  s.PushFront(42);
  EXPECT_THAT(s.Size(), Eq(1u));
  EXPECT_THAT(s.PeekFront(), Optional(42));
}

TEST(StackTest, PopSingleElement) {
  Stack<int> s;
  s.PushFront(99);
  EXPECT_THAT(s.PopFront(), Optional(99));
  EXPECT_THAT(s.Size(), Eq(0u));
  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
}

TEST(StackTest, PeekDoesNotRemove) {
  Stack<int> s;
  s.PushFront(10);
  EXPECT_THAT(s.PeekFront(), Optional(10));
  EXPECT_THAT(s.Size(), Eq(1u));
  EXPECT_THAT(s.PeekFront(), Optional(10));
  EXPECT_THAT(s.Size(), Eq(1u));
}

// ---------- LIFO property ----------
TEST(StackTest, MultiplePushPopOrder) {
  Stack<int> s;
  s.PushFront(1);
  s.PushFront(2);
  s.PushFront(3);

  EXPECT_THAT(s.Size(), Eq(3u));

  EXPECT_THAT(s.PopFront(), Optional(3));
  EXPECT_THAT(s.PopFront(), Optional(2));
  EXPECT_THAT(s.PopFront(), Optional(1));
  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
  EXPECT_THAT(s.Size(), Eq(0u));
}

// ---------- Interleaved ----------
TEST(StackTest, InterleavedOperations) {
  Stack<int> s;
  s.PushFront(5);
  s.PushFront(6);
  EXPECT_THAT(s.PopFront(), Optional(6));
  s.PushFront(7);
  EXPECT_THAT(s.PeekFront(), Optional(7));
  EXPECT_THAT(s.PopFront(), Optional(7));
  EXPECT_THAT(s.PopFront(), Optional(5));
  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
}

// ---------- With non-POD types ----------
TEST(StackTest, WorksWithStdString) {
  Stack<std::string> s;
  s.PushFront("hello");
  s.PushFront("world");

  EXPECT_THAT(s.Size(), Eq(2u));
  EXPECT_THAT(s.PeekFront(),
              Optional(std::string("world")));
  EXPECT_THAT(s.PopFront(), Optional(std::string("world")));
  EXPECT_THAT(s.PopFront(), Optional(std::string("hello")));
  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
}

TEST(StackTest, WorksWithUniquePtr) {
  Stack<std::unique_ptr<int>> s;
  s.PushFront(std::make_unique<int>(42));
  s.PushFront(std::make_unique<int>(99));

  auto v1 = s.PopFront();
  ASSERT_TRUE(v1.has_value());
  EXPECT_THAT(*v1.value(), Eq(99));

  auto v2 = s.PopFront();
  ASSERT_TRUE(v2.has_value());
  EXPECT_THAT(*v2.value(), Eq(42));

  EXPECT_THAT(s.PopFront(), Eq(std::nullopt));
}

#include "cs/q/queue/queue.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

using ::testing::Eq;
using ::testing::Optional;

using cs::q::queue::Queue;

// Helper struct for custom type testing
struct Point {
  int x, y;
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

// ---------- Empty queue tests ----------
TEST(QueueTest, InitiallyEmpty) {
  Queue<int> q;
  EXPECT_THAT(q.Size(), Eq(0));
  EXPECT_THAT(q.PeekFront(), Eq(std::nullopt));
  EXPECT_THAT(q.PeekBack(), Eq(std::nullopt));
  EXPECT_THAT(q.PopFront(), Eq(std::nullopt));
  EXPECT_THAT(q.PopBack(), Eq(std::nullopt));
}

// ---------- Single element consistency ----------
TEST(QueueTest, SingleElementConsistency) {
  Queue<int> q1;
  q1.PushFront(42);
  EXPECT_THAT(q1.PeekFront(), Optional(42));
  EXPECT_THAT(q1.PeekBack(), Optional(42));

  Queue<int> q2;
  q2.PushBack(99);
  EXPECT_THAT(q2.PeekFront(), Optional(99));
  EXPECT_THAT(q2.PeekBack(), Optional(99));
}

// ---------- FIFO / PushBack & PopFront ----------
TEST(QueueTest, PushBackPopFrontFIFO) {
  Queue<int> q;
  for (int i = 1; i <= 5; ++i) q.PushBack(i);

  EXPECT_THAT(q.Size(), Eq(5));
  for (int i = 1; i <= 5; ++i) {
    EXPECT_THAT(q.PeekFront(), Optional(i));
    EXPECT_THAT(q.PopFront(), Optional(i));
  }
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- LIFO / PushFront & PopBack ----------
TEST(QueueTest, PushFrontPopBackLIFO) {
  Queue<std::string> q;
  q.PushFront("one");
  q.PushFront("two");
  q.PushFront("three");

  EXPECT_THAT(q.Size(), Eq(3));
  EXPECT_THAT(q.PeekBack(), Optional(std::string("one")));
  EXPECT_THAT(q.PopBack(), Optional(std::string("one")));
  EXPECT_THAT(q.PopBack(), Optional(std::string("two")));
  EXPECT_THAT(q.PopBack(), Optional(std::string("three")));
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Mixed front/back operations ----------
TEST(QueueTest, MixedOperations) {
  Queue<int> q;
  q.PushBack(10);  // [10]
  q.PushFront(5);  // [5, 10]
  q.PushBack(15);  // [5, 10, 15]

  EXPECT_THAT(q.Size(), Eq(3));
  EXPECT_THAT(q.PeekFront(), Optional(5));
  EXPECT_THAT(q.PeekBack(), Optional(15));

  EXPECT_THAT(q.PopFront(), Optional(5));   // [10, 15]
  EXPECT_THAT(q.PopBack(), Optional(15));   // [10]
  EXPECT_THAT(q.PopFront(), Optional(10));  // []
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Custom structs ----------
TEST(QueueTest, CustomStructs) {
  Queue<Point> q;
  q.PushBack(Point{1, 2});
  q.PushBack(Point{3, 4});

  EXPECT_THAT(q.Size(), Eq(2));
  EXPECT_THAT(q.PeekFront(), Optional(Point{1, 2}));
  EXPECT_THAT(q.PeekBack(), Optional(Point{3, 4}));

  EXPECT_THAT(q.PopFront(), Optional(Point{1, 2}));
  EXPECT_THAT(q.PopFront(), Optional(Point{3, 4}));
  EXPECT_THAT(q.PopFront(), Eq(std::nullopt));
}

// ---------- Move-only types ----------
TEST(QueueTest, UniquePtrSupport) {
  Queue<std::unique_ptr<int>> q;
  q.PushBack(std::make_unique<int>(42));
  q.PushFront(std::make_unique<int>(7));

  EXPECT_THAT(q.Size(), Eq(2));

  auto v1 = q.PopFront();
  ASSERT_TRUE(v1.has_value());
  EXPECT_THAT(**v1, Eq(7));

  auto v2 = q.PopBack();
  ASSERT_TRUE(v2.has_value());
  EXPECT_THAT(**v2, Eq(42));

  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Many elements stress test ----------
TEST(QueueTest, LargeNumberOfElements) {
  Queue<int> q;
  constexpr int N = 1000;
  for (int i = 0; i < N; ++i) q.PushBack(i);
  EXPECT_THAT(q.Size(), Eq(N));

  for (int i = 0; i < N; ++i)
    EXPECT_THAT(q.PopFront(), Optional(i));
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Interleaved front/back stress test ----------
TEST(QueueTest, InterleavedPushFrontBackPopFrontBack) {
  Queue<int> q;
  q.PushBack(1);    // [1]
  q.PushFront(0);   // [0,1]
  q.PushBack(2);    // [0,1,2]
  q.PushFront(-1);  // [-1,0,1,2]

  EXPECT_THAT(q.Size(), Eq(4));
  EXPECT_THAT(q.PopFront(), Optional(-1));  // [0,1,2]
  EXPECT_THAT(q.PopBack(), Optional(2));    // [0,1]
  EXPECT_THAT(q.PopFront(), Optional(0));   // [1]
  EXPECT_THAT(q.PopBack(), Optional(1));    // []
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Const correctness ----------
TEST(QueueTest, ConstQueuePeekAndSize) {
  Queue<int> q;
  q.PushBack(10);
  q.PushFront(5);

  const auto& cq = q;
  EXPECT_THAT(cq.Size(), Eq(2));
  EXPECT_THAT(cq.PeekFront(), Optional(5));
  EXPECT_THAT(cq.PeekBack(), Optional(10));
}

// ---------- Memory safety with unique_ptr ----------
TEST(QueueTest, MemorySafetyUniquePtrNoLeaks) {
  Queue<std::unique_ptr<int>> q;

  // Push and pop multiple unique_ptrs
  for (int i = 0; i < 100; ++i) {
    q.PushBack(std::make_unique<int>(i));
  }

  for (int i = 0; i < 50; ++i) {
    auto v = q.PopFront();
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v.value(), i);
  }

  for (int i = 100; i < 150; ++i) {
    q.PushFront(std::make_unique<int>(i));
  }

  while (q.Size() > 0) {
    auto v = q.PopBack();
    ASSERT_TRUE(v.has_value());
  }

  // At this point, all unique_ptrs should be safely deleted
  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Stress delete correctness ----------
TEST(QueueTest, StressDeleteNoDoubleFree) {
  Queue<std::unique_ptr<int>> q;

  for (int i = 0; i < 500; ++i) {
    q.PushBack(std::make_unique<int>(i));
  }

  // Pop some from front
  for (int i = 0; i < 250; ++i) {
    auto v = q.PopFront();
    ASSERT_TRUE(v.has_value());
  }

  // Pop remaining from back
  while (q.Size() > 0) {
    auto v = q.PopBack();
    ASSERT_TRUE(v.has_value());
  }

  EXPECT_THAT(q.Size(), Eq(0));
}

// ---------- Repeated destruction of queue ----------
TEST(QueueTest, DestructorCalledMultipleTimesSafely) {
  {
    Queue<std::unique_ptr<int>> q;
    for (int i = 0; i < 100; ++i) {
      q.PushBack(std::make_unique<int>(i));
    }
  }  // q goes out of scope; destructor should delete all
     // nodes safely

  {
    Queue<int> q;
    q.PushBack(1);
    q.PushFront(2);
  }  // q destroyed; no leaks or double delete
}

// ---------- Randomized push/pop patterns ----------
TEST(QueueTest, RandomizedPushPopMemorySafety) {
  Queue<std::unique_ptr<int>> q;
  std::vector<std::unique_ptr<int>> refs;

  for (int i = 0; i < 200; ++i) {
    if (i % 3 == 0) {
      q.PushBack(std::make_unique<int>(i));
    } else if (i % 3 == 1) {
      q.PushFront(std::make_unique<int>(i));
    } else if (q.Size() > 0) {
      q.PopFront();
    }
  }

  while (q.Size() > 0) {
    q.PopBack();
  }

  EXPECT_THAT(q.Size(), Eq(0));
}

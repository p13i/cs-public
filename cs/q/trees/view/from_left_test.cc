// cs/q/trees/view/from_left_test.cc
#include <string>
#include <vector>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/view.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::q::queue::Queue;
using ::cs::q::trees::LeftViewBFS;
using ::cs::q::trees::Node;
using ::testing::Eq;
using ::testing::Optional;
}  // namespace

// Helper: free tree memory
template <typename T>
void DeleteTree(Node<T>* root) {
  if (!root) return;
  DeleteTree(root->left);
  DeleteTree(root->right);
  delete root;
}

// Helper: drain queue into vector
template <typename T>
std::vector<T> QueueToVector(Queue<T> q) {
  std::vector<T> out;
  while (q.Size() > 0) {
    out.push_back(q.PopFront().value());
  }
  return out;
}

TEST(LeftViewBFS, EmptyTree) {
  Node<int>* root = nullptr;
  auto view = LeftViewBFS(root);
  EXPECT_THAT(view.Size(), Eq(0u));
}

TEST(LeftViewBFS, SingleNode) {
  Node<int> root{42, nullptr, nullptr};
  auto view = LeftViewBFS(&root);
  EXPECT_THAT(view.Size(), Eq(1u));
  EXPECT_THAT(view.PopFront(), Optional(42));
}

/*
Complete binary tree
      1
    /   \
   2     3
  / \   / \
 4  5  6   7
*/
TEST(LeftViewBFS, CompleteBinaryTree) {
  Node<int> n4{4, nullptr, nullptr};
  Node<int> n5{5, nullptr, nullptr};
  Node<int> n6{6, nullptr, nullptr};
  Node<int> n7{7, nullptr, nullptr};
  Node<int> n2{2, &n4, &n5};
  Node<int> n3{3, &n6, &n7};
  Node<int> n1{1, &n2, &n3};

  auto view = LeftViewBFS(&n1);
  // Expected left view: 1,2,4
  EXPECT_THAT(view.Size(), Eq(3u));
  EXPECT_THAT(view.PopFront(), Optional(1));
  EXPECT_THAT(view.PopFront(), Optional(2));
  EXPECT_THAT(view.PopFront(), Optional(4));
}

/*
Left skewed tree
1
|
2
|
3
*/
TEST(LeftViewBFS, LeftSkewedTree) {
  Node<int> n3{3, nullptr, nullptr};
  Node<int> n2{2, &n3, nullptr};
  Node<int> n1{1, &n2, nullptr};

  auto view = LeftViewBFS(&n1);
  // Expected left view: 1,2,3
  EXPECT_THAT(view.Size(), Eq(3u));
  EXPECT_THAT(view.PopFront(), Optional(1));
  EXPECT_THAT(view.PopFront(), Optional(2));
  EXPECT_THAT(view.PopFront(), Optional(3));
}

/*
Right skewed tree
1
 \
  2
   \
    3
*/
TEST(LeftViewBFS, RightSkewedTree) {
  Node<int> n3{3, nullptr, nullptr};
  Node<int> n2{2, nullptr, &n3};
  Node<int> n1{1, nullptr, &n2};

  auto view = LeftViewBFS(&n1);
  // Expected left view: 1,2,3
  EXPECT_THAT(view.Size(), Eq(3u));
  EXPECT_THAT(view.PopFront(), Optional(1));
  EXPECT_THAT(view.PopFront(), Optional(2));
  EXPECT_THAT(view.PopFront(), Optional(3));
}

/*
Mixed tree with occlusion
      1
    /   \
   2     3
    \     \
     4     5
*/
TEST(LeftViewBFS, MixedTree) {
  Node<int> n4{4, nullptr, nullptr};
  Node<int> n5{5, nullptr, nullptr};
  Node<int> n2{2, nullptr, &n4};
  Node<int> n3{3, nullptr, &n5};
  Node<int> n1{1, &n2, &n3};

  auto view = LeftViewBFS(&n1);
  // Expected left view: 1,2,4
  EXPECT_THAT(view.Size(), Eq(3u));
  EXPECT_THAT(view.PopFront(), Optional(1));
  EXPECT_THAT(view.PopFront(), Optional(2));
  EXPECT_THAT(view.PopFront(), Optional(4));
}

/*
 Single child nodes
       1
     /
    2
     \
      3
*/
TEST(LeftViewBFS, SingleChildNodes) {
  Node<int> n3{3, nullptr, nullptr};
  Node<int> n2{2, nullptr, &n3};
  Node<int> n1{1, &n2, nullptr};

  auto view = LeftViewBFS(&n1);
  // Expected left view: 1,2,3
  EXPECT_THAT(view.Size(), Eq(3u));
  EXPECT_THAT(view.PopFront(), Optional(1));
  EXPECT_THAT(view.PopFront(), Optional(2));
  EXPECT_THAT(view.PopFront(), Optional(3));
}

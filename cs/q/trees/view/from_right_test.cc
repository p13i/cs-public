// cs/q/trees/view/from_right_test.cc
// cs/q/trees/view_from_right_test.cc
#include <string>
#include <vector>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/view.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::q::queue::Queue;
using ::cs::q::trees::Node;
using ::cs::q::trees::RightViewBFS;
using ::testing::Eq;
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

// Empty tree -> empty result
TEST(RightViewBFS, HandlesEmptyTree) {
  Node<int>* root = nullptr;
  auto q = RightViewBFS<int>(root);
  EXPECT_EQ(q.Size(), 0u);
  auto v = QueueToVector(std::move(q));
  EXPECT_TRUE(v.empty());
}

// Single node
TEST(RightViewBFS, SingleNode) {
  Node<int>* root = new Node<int>(42);
  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  EXPECT_EQ(v.size(), 1u);
  ASSERT_EQ(v[0], 42);
  DeleteTree(root);
}

/*
Left-skewed: nodes at each level only on left
  1
 /
2
/
//3*/
TEST(RightViewBFS, LeftSkewedTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->left->left = new Node<int>(3);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  // Right view of a strictly left-skewed tree is [1,2,3]
  std::vector<int> expected = {1, 2, 3};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
Right-skewed: nodes at each level only on right
1
 \
  2
   \
    3
*/
TEST(RightViewBFS, RightSkewedTree) {
  Node<int>* root = new Node<int>(1);
  root->right = new Node<int>(2);
  root->right->right = new Node<int>(3);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {1, 2, 3};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
Balanced-ish tree with right nodes that hide left nodes
on some levels:
     1
    / \
   2   3
  / \   \
 4   5   6
*/
TEST(RightViewBFS, MixedTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->right = new Node<int>(3);
  root->left->left = new Node<int>(4);
  root->left->right = new Node<int>(5);
  root->right->right = new Node<int>(6);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  // Rightmost at level 0: 1
  // Rightmost at level 1: 3
  // Rightmost at level 2: 6
  std::vector<int> expected = {1, 3, 6};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
Full binary tree
       1
     /   \
    2     3
   / \   / \
  4  5  6   7
*/
TEST(RightViewBFS, FullBinaryTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->right = new Node<int>(3);
  root->left->left = new Node<int>(4);
  root->left->right = new Node<int>(5);
  root->right->left = new Node<int>(6);
  root->right->right = new Node<int>(7);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {1, 3, 7};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// Duplicate values
// Tree shape similar to MixedTree but values repeat.
TEST(RightViewBFS, Duplicates) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(1);
  root->right = new Node<int>(1);
  root->left->right = new Node<int>(1);
  root->right->right = new Node<int>(1);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  // rightmost values per level
  std::vector<int> expected = {1, 1, 1};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
Irregular structure that ensures algorithm correctly
skips nulls on a level
     10
    /  \
   5    2
    \    \
     7    3
Right view: [10,2,3]
*/
TEST(RightViewBFS, MissingChildrenPerLevel) {
  Node<int>* root = new Node<int>(10);
  root->left = new Node<int>(5);
  root->right = new Node<int>(2);
  root->left->right = new Node<int>(7);
  root->right->right = new Node<int>(3);

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {10, 2, 3};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// Larger depth test (right-leaning chain) to exercise
// multiple levels
TEST(RightViewBFS, DeepRightChain) {
  const int depth = 12;
  Node<int>* root = new Node<int>(0);
  Node<int>* cur = root;
  for (int i = 1; i < depth; ++i) {
    cur->right = new Node<int>(i);
    cur = cur->right;
  }

  auto q = RightViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));

  // Expect [0,1,2,...,depth-1]
  std::vector<int> expected;
  for (int i = 0; i < depth; ++i) expected.push_back(i);
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// Non-integer test to verify template works with other
// types (std::string)
TEST(RightViewBFS, StringValues) {
  Node<std::string>* root = new Node<std::string>("root");
  root->left = new Node<std::string>("L");
  root->right = new Node<std::string>("R");
  root->left->left = new Node<std::string>("LL");
  root->right->right = new Node<std::string>("RR");

  auto q = RightViewBFS<std::string>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<std::string> expected = {"root", "R", "RR"};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

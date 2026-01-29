// cs/q/trees/view/from_top_test.cc
// cs/q/trees/view_from_top_test.cc
#include <string>
#include <vector>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/view.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cs::q::trees;
using ::testing::Eq;

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
std::vector<T> QueueToVector(cs::q::queue::Queue<T> q) {
  std::vector<T> out;
  while (q.Size() > 0) {
    out.push_back(q.PopFront().value());
  }
  return out;
}

// 1) Empty tree -> empty result
TEST(TopViewBFS, HandlesEmptyTree) {
  Node<int>* root = nullptr;
  auto q = TopViewBFS<int>(root);
  EXPECT_EQ(q.Size(), 0u);
  auto v = QueueToVector(std::move(q));
  EXPECT_TRUE(v.empty());
}

// 2) Single node
TEST(TopViewBFS, SingleNode) {
  Node<int>* root = new Node<int>(42);
  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  ASSERT_EQ(v.size(), 1u);
  EXPECT_EQ(v[0], 42);
  DeleteTree(root);
}

/*
3) Left-skewed chain: top-view should include every node,
left-to-right order Tree (hd):   1(0)
            /
         2(-1)
        /
      3(-2)
Expected left-to-right horiz. distances: hd -2, -1, 0 ->
{3,2,1}
*/
TEST(TopViewBFS, LeftSkewedTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->left->left = new Node<int>(3);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {3, 2, 1};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// 4) Right-skewed chain: top-view is {1,2,3}
TEST(TopViewBFS, RightSkewedTree) {
  Node<int>* root = new Node<int>(1);
  root->right = new Node<int>(2);
  root->right->right = new Node<int>(3);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {1, 2, 3};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
5) Full binary tree
        1
      /   \
     2     3
    / \   / \
   4  5  6   7
Top view (leftmost -> rightmost): {4,2,1,3,7}
*/
TEST(TopViewBFS, FullBinaryTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->right = new Node<int>(3);
  root->left->left = new Node<int>(4);
  root->left->right = new Node<int>(5);
  root->right->left = new Node<int>(6);
  root->right->right = new Node<int>(7);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {4, 2, 1, 3, 7};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
6) Mixed tree where some nodes are hidden below others
      1
     / \
    2   3
   / \   \
  4   5   6
Horizontal distances: -2:4, -1:2, 0:1 (5 hidden), 1:3, 2:6
=> {4,2,1,3,6}
*/
TEST(TopViewBFS, MixedTree) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(2);
  root->right = new Node<int>(3);
  root->left->left = new Node<int>(4);
  root->left->right = new Node<int>(5);
  root->right->right = new Node<int>(6);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {4, 2, 1, 3, 6};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

/*
7) Irregular structure (missing children) to ensure nulls
are skipped 10
   /  \
  5    2
   \    \
    7    3
Top view: hd -1:5, 0:10, 1:2, 2:3 => {5,10,2,3}
*/
TEST(TopViewBFS, MissingChildrenPerLevel) {
  Node<int>* root = new Node<int>(10);
  root->left = new Node<int>(5);
  root->right = new Node<int>(2);
  root->left->right = new Node<int>(7);
  root->right->right = new Node<int>(3);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<int> expected = {5, 10, 2, 3};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// 8) Deeper tree (to exercise multiple levels)
TEST(TopViewBFS, DeepRightChain) {
  const int depth = 12;
  Node<int>* root = new Node<int>(0);
  Node<int>* cur = root;
  for (int i = 1; i < depth; ++i) {
    cur->right = new Node<int>(i);
    cur = cur->right;
  }

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));

  std::vector<int> expected;
  for (int i = 0; i < depth; ++i) expected.push_back(i);
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// 9) Duplicate values (ensure algorithm does not confuse
// value equality with position)
TEST(TopViewBFS, Duplicates) {
  Node<int>* root = new Node<int>(1);
  root->left = new Node<int>(1);
  root->right = new Node<int>(1);
  root->left->right = new Node<int>(1);
  root->right->right = new Node<int>(1);

  auto q = TopViewBFS<int>(root);
  auto v = QueueToVector(std::move(q));
  // Expected by horizontal distance (leftmost -> rightmost)
  std::vector<int> expected = {1, 1, 1, 1};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

// 10) Non-integer types: strings
TEST(TopViewBFS, StringValues) {
  Node<std::string>* root = new Node<std::string>("root");
  root->left = new Node<std::string>("L");
  root->right = new Node<std::string>("R");
  root->left->left = new Node<std::string>("LL");
  root->right->right = new Node<std::string>("RR");

  auto q = TopViewBFS<std::string>(root);
  auto v = QueueToVector(std::move(q));
  std::vector<std::string> expected = {"LL", "L", "root",
                                       "R", "RR"};
  EXPECT_EQ(v, expected);

  DeleteTree(root);
}

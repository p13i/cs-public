// cs/q/trees/iterate/pre_order_test.gpt.cc
// Leetcode 144: Binary Tree Preorder Traversal
// Each test includes an ASCII sketch of the tree and the
// expected pre-order traversal.

#include <vector>

#include "cs/q/trees/iterate.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cs::q::trees {
namespace {

template <typename T, typename L = std::nullptr_t,
          typename R = std::nullptr_t>
Node<T>* NodeOf(T v, L l = nullptr, R r = nullptr) {
  return new Node<T>(v, static_cast<Node<T>*>(l),
                     static_cast<Node<T>*>(r));
}

template <typename T>
void DeleteTree(Node<T>* root) {
  if (root == nullptr) return;
  DeleteTree(root->left);
  DeleteTree(root->right);
  delete root;
}

template <typename T>
std::vector<T> Collect(Node<T>* root) {
  std::vector<T> out;
  PreOrderTreeIterator<T> it(root);
  while (it.HasNext()) {
    out.push_back(it.Next());
  }
  return out;
}

//        (empty)
// expected: {}
TEST(PreOrderTreeIteratorGPT, EmptyTree) {
  Node<int>* root = nullptr;
  EXPECT_THAT(Collect(root), ::testing::ElementsAre());
}

//      1
// expected: 1
TEST(PreOrderTreeIteratorGPT, SingleNode) {
  Node<int>* root = NodeOf(1);
  EXPECT_THAT(Collect(root), ::testing::ElementsAre(1));
  DeleteTree(root);
}

//       3
//      /
//     2
//    /
//   1
// expected: 3 2 1
TEST(PreOrderTreeIteratorGPT, LeftSkewed) {
  Node<int>* root = NodeOf(3, NodeOf(2, NodeOf(1)));
  EXPECT_THAT(Collect(root),
              ::testing::ElementsAre(3, 2, 1));
  DeleteTree(root);
}

//   1
//    \
//     2
//      \
//       3
// expected: 1 2 3
TEST(PreOrderTreeIteratorGPT, RightSkewed) {
  Node<int>* root =
      NodeOf(1, nullptr, NodeOf(2, nullptr, NodeOf(3)));
  EXPECT_THAT(Collect(root),
              ::testing::ElementsAre(1, 2, 3));
  DeleteTree(root);
}

//       4
//     /   \
//    2     6
//   / \   / \
//  1   3 5   7
// expected: 4 2 1 3 6 5 7
TEST(PreOrderTreeIteratorGPT, PerfectBST) {
  Node<int>* root =
      NodeOf(4, NodeOf(2, NodeOf(1), NodeOf(3)),
             NodeOf(6, NodeOf(5), NodeOf(7)));
  EXPECT_THAT(Collect(root),
              ::testing::ElementsAre(4, 2, 1, 3, 6, 5, 7));
  DeleteTree(root);
}

//       5
//     /   \
//    3     8
//   /     /
//  2     7
//   \     \
//    2     9
// expected: 5 3 2 2 8 7 9 (note repeated 2)
TEST(PreOrderTreeIteratorGPT,
     MissingChildrenAndDuplicates) {
  Node<int>* root =
      NodeOf(5, NodeOf(3, NodeOf(2, nullptr, NodeOf(2))),
             NodeOf(8, NodeOf(7, nullptr, NodeOf(9))));
  EXPECT_THAT(Collect(root),
              ::testing::ElementsAre(5, 3, 2, 2, 8, 7, 9));
  DeleteTree(root);
}

//       10
//      /  \
//     5    15
//      \     \
//       6     20
//            /
//           18
// expected: 10 5 6 15 20 18
TEST(PreOrderTreeIteratorGPT, UnbalancedMixed) {
  Node<int>* root = NodeOf(
      10, NodeOf(5, nullptr, NodeOf(6)),
      NodeOf(15, nullptr, NodeOf(20, NodeOf(18), nullptr)));
  EXPECT_THAT(Collect(root),
              ::testing::ElementsAre(10, 5, 6, 15, 20, 18));
  DeleteTree(root);
}

//       4
//     /   \
//    2     5
//   / \
//  1   3
// expected: HasNext toggles false after final Next
TEST(PreOrderTreeIteratorGPT, HasNextDrains) {
  Node<int>* root =
      NodeOf(4, NodeOf(2, NodeOf(1), NodeOf(3)), NodeOf(5));
  PreOrderTreeIterator<int> it(root);
  EXPECT_TRUE(it.HasNext());
  EXPECT_EQ(it.Next(), 4);
  EXPECT_TRUE(it.HasNext());
  EXPECT_EQ(it.Next(), 2);
  EXPECT_TRUE(it.HasNext());
  EXPECT_EQ(it.Next(), 1);
  EXPECT_TRUE(it.HasNext());
  EXPECT_EQ(it.Next(), 3);
  EXPECT_TRUE(it.HasNext());
  EXPECT_EQ(it.Next(), 5);
  EXPECT_FALSE(it.HasNext());
  DeleteTree(root);
}

}  // namespace
}  // namespace cs::q::trees

// cs/q/trees/nodes_distance_k/nodes_distance_k_test.gpt.cc
// cs/q/trees/nodes_distance_k_test.gpt.cc
// Leetcode 863: All Nodes Distance K in Binary Tree
// Each test sketches the tree, highlights the target node,
// and lists the expected node values at distance k.

#include "cs/q/trees/nodes_distance_k/nodes_distance_k.hh"

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cs::q::trees {
namespace {

Node<int>* N(int v, Node<int>* l = nullptr,
             Node<int>* r = nullptr) {
  return new Node<int>(v, l, r);
}

void DeleteTree(Node<int>* root) {
  if (root == nullptr) return;
  DeleteTree(root->left);
  DeleteTree(root->right);
  delete root;
}

std::vector<int> Data(
    const std::vector<Node<int>*>& nodes) {
  std::vector<int> out;
  for (auto* n : nodes) out.push_back(n->value);
  return out;
}

// Leetcode example:
TEST(AllNodesDistanceKGPT, ExampleDistanceTwo) {
  Node<int>* root = N(3);
  root->left = N(5);
  root->right = N(1);
  root->left->left = N(6);
  root->left->right = N(2);
  root->left->right->left = N(7);
  root->left->right->right = N(4);
  root->right->left = N(0);
  root->right->right = N(8);

  auto nodes = AllNodesDistanceK(root, root->left, 2);
  EXPECT_THAT(Data(nodes),
              ::testing::UnorderedElementsAre(7, 4, 1));
  DeleteTree(root);
}

// Target is root, k=1: neighbors only.
//   1*
//  / \
// 2   3
TEST(AllNodesDistanceKGPT, TargetIsRootDistanceOne) {
  Node<int>* root = N(1, N(2), N(3));
  auto nodes = AllNodesDistanceK(root, root, 1);
  EXPECT_THAT(Data(nodes),
              ::testing::UnorderedElementsAre(2, 3));
  DeleteTree(root);
}

// Parent branch path:
//     5
//    / \
//   3*  8
//    \   \
//     4   10
// k=2 -> {8}
TEST(AllNodesDistanceKGPT, ParentBranchProvidesDistance) {
  Node<int>* root = N(5);
  root->left = N(3);
  root->right = N(8, nullptr, N(10));
  root->left->right = N(4);

  auto nodes = AllNodesDistanceK(root, root->left, 2);
  EXPECT_THAT(Data(nodes),
              ::testing::UnorderedElementsAre(8));
  DeleteTree(root);
}

// Right-skewed chain:
// 1
//  \
//   2*
//    \
//     3
//      \
//       4
// k=2 -> {4}
TEST(AllNodesDistanceKGPT, SkewedRightChain) {
  Node<int>* root = N(1);
  root->right = N(2);
  root->right->right = N(3);
  root->right->right->right = N(4);

  auto nodes = AllNodesDistanceK(root, root->right, 2);
  EXPECT_THAT(Data(nodes), ::testing::ElementsAre(4));
  DeleteTree(root);
}

// Leaf target:
//     9
//    / \
//   4   12
//        \
//        15*
// k=1 -> {12}
TEST(AllNodesDistanceKGPT, LeafTargetHasOnlyParent) {
  Node<int>* root = N(9, N(4), N(12, nullptr, N(15)));

  auto nodes =
      AllNodesDistanceK(root, root->right->right, 1);
  EXPECT_THAT(Data(nodes), ::testing::ElementsAre(12));
  DeleteTree(root);
}

// Duplicate values across the tree; identity not value:
//     1*
//    / \
//   2   1
//  /   /
// 1   2
// k=2 -> {1,2}
TEST(AllNodesDistanceKGPT, DuplicateValuesDoNotConflict) {
  Node<int>* root = N(1);
  root->left = N(2, N(1), nullptr);
  root->right = N(1, N(2), nullptr);

  auto nodes = AllNodesDistanceK(root, root, 2);
  EXPECT_THAT(Data(nodes),
              ::testing::UnorderedElementsAre(1, 2));
  DeleteTree(root);
}

// k=0 returns target only.
//   7
//  /
// 4*
TEST(AllNodesDistanceKGPT, ZeroDistanceReturnsTargetOnly) {
  Node<int>* root = N(7, N(4), nullptr);
  auto nodes = AllNodesDistanceK(root, root->left, 0);
  ASSERT_THAT(nodes, ::testing::SizeIs(1));
  EXPECT_EQ(nodes[0]->value, 4);
  DeleteTree(root);
}

// k larger than height yields empty.
//   2*
//  /
// 1
TEST(AllNodesDistanceKGPT, DistanceBeyondHeightIsEmpty) {
  Node<int>* root = N(2, N(1), nullptr);
  auto nodes = AllNodesDistanceK(root, root, 3);
  EXPECT_THAT(nodes, ::testing::IsEmpty());
  DeleteTree(root);
}

// Target missing from tree: should be robust (empty).
TEST(AllNodesDistanceKGPT, TargetNotInTreeReturnsEmpty) {
  Node<int>* root = N(1, N(2), N(3));
  Node<int> orphan(99);
  auto nodes = AllNodesDistanceK(root, &orphan, 1);
  EXPECT_THAT(nodes, ::testing::IsEmpty());
  DeleteTree(root);
}

TEST(AllNodesDistanceKGPT, NullRootOrTargetReturnsEmpty) {
  auto empty = AllNodesDistanceK<int>(nullptr, nullptr, 1);
  EXPECT_THAT(empty, ::testing::IsEmpty());
  Node<int> root(1);
  Node<int>* null_target = nullptr;
  auto empty_target =
      AllNodesDistanceK(&root, null_target, 1);
  EXPECT_THAT(empty_target, ::testing::IsEmpty());
}

}  // namespace
}  // namespace cs::q::trees

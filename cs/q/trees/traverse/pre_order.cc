// cs/q/trees/traverse/pre_order.cc
#ifndef CS_Q_TREES_TRAVERSE_PRE_ORDER_CC
#define CS_Q_TREES_TRAVERSE_PRE_ORDER_CC

#include <utility>
#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// Pre-order traversal (Node, Left, Right).
template <typename T>
void TraversePreOrder(Node<T>* root, std::vector<T>* out) {
  if (root == nullptr || out == nullptr) {
    return;
  }
  out->push_back(root->value);
  TraversePreOrder(root->left, out);
  TraversePreOrder(root->right, out);
}

template <typename T>
std::vector<T> TraversePreOrder(Node<T>* root) {
  std::vector<T> values;
  TraversePreOrder(root, &values);
  return values;
}

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_TRAVERSE_PRE_ORDER_CC

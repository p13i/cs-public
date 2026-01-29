// cs/q/trees/traverse/in_order.cc
#ifndef CS_Q_TREES_TRAVERSE_IN_ORDER_CC
#define CS_Q_TREES_TRAVERSE_IN_ORDER_CC

#include <utility>
#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// In-order traversal (Left, Node, Right).
template <typename T>
void TraverseInOrder(Node<T>* root, std::vector<T>* out) {
  if (root == nullptr || out == nullptr) {
    return;
  }
  TraverseInOrder(root->left, out);
  out->push_back(root->value);
  TraverseInOrder(root->right, out);
}

template <typename T>
std::vector<T> TraverseInOrder(Node<T>* root) {
  std::vector<T> values;
  TraverseInOrder(root, &values);
  return values;
}

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_TRAVERSE_IN_ORDER_CC

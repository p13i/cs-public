// cs/q/trees/traverse/post_order.cc
#ifndef CS_Q_TREES_TRAVERSE_POST_ORDER_CC
#define CS_Q_TREES_TRAVERSE_POST_ORDER_CC

#include <utility>
#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// Post-order traversal (Left, Right, Node).
template <typename T>
void TraversePostOrder(Node<T>* root, std::vector<T>* out) {
  if (root == nullptr || out == nullptr) {
    return;
  }
  TraversePostOrder(root->left, out);
  TraversePostOrder(root->right, out);
  out->push_back(root->value);
}

template <typename T>
std::vector<T> TraversePostOrder(Node<T>* root) {
  std::vector<T> values;
  TraversePostOrder(root, &values);
  return values;
}

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_TRAVERSE_POST_ORDER_CC

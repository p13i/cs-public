// cs/q/trees/iterate/in_order.cc
#include <cassert>

#include "cs/q/trees/iterate.hh"

namespace cs::q::trees {

// Start cursor at first element of in-order traversal by
// setting up the stack to that point.
template <typename T>
InOrderTreeIterator<T>::InOrderTreeIterator(
    const Node<T>* root)
    : TreeIterator<T>(root) {
  const Node<T>* node = this->root_;
  this->root_ = nullptr;
  while (node != nullptr) {
    stack_.PushFront(node);
    node = node->left;
  }
}

template <typename T>
bool InOrderTreeIterator<T>::HasNext() const {
  return stack_.Size() > 0;
};

template <typename T>
T InOrderTreeIterator<T>::Next() {
  // Caller should check HasNext() before calling Next().
  assert(HasNext());

  // Hold onto this node's value.
  const Node<T>* curr_node = stack_.PopFront().value();
  const T value = curr_node->value;

  // Look for this node's next value in the left-most node
  // of the right subtree.
  const Node<T>* right = curr_node->right;
  while (right != nullptr) {
    stack_.PushFront(right);
    right = right->left;
  }

  return value;
};

// Explicit instantiation for common use in tests.
template class InOrderTreeIterator<int>;

}  // namespace cs::q::trees

// cs/q/trees/iterate/pre_order.cc
#include <assert.h>

#include "cs/q/trees/iterate.hh"

namespace cs::q::trees {

template <typename T>
PreOrderTreeIterator<T>::PreOrderTreeIterator(
    const Node<T>* root)
    : TreeIterator<T>(root) {
  if (root != nullptr) {
    stack_.PushFront(root);
  }
}

template <typename T>
bool PreOrderTreeIterator<T>::HasNext() const {
  return stack_.Size() > 0;
};

template <typename T>
T PreOrderTreeIterator<T>::Next() {
  assert(HasNext());

  const Node<T>* curr_node = stack_.PopFront().value();
  if (curr_node->right != nullptr) {
    stack_.PushFront(curr_node->right);
  }
  if (curr_node->left != nullptr) {
    stack_.PushFront(curr_node->left);
  }

  return curr_node->value;
}

// Explicit instantiation for common use in tests.
template class PreOrderTreeIterator<int>;

}  // namespace cs::q::trees

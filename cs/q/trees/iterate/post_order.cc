// cs/q/trees/iterate/post_order.cc
#include <cassert>
#include <utility>

#include "cs/q/trees/iterate.hh"

namespace cs::q::trees {

template <typename T>
PostOrderTreeIterator<T>::PostOrderTreeIterator(
    const Node<T>* root)
    : TreeIterator<T>(root) {
  if (root != nullptr) {
    // Mark as "not visited" because we haven't processed
    // the root's child nodes yet.
    stack_.PushFront(std::make_pair(root, false));
  }
}

template <typename T>
bool PostOrderTreeIterator<T>::HasNext() const {
  return stack_.Size() > 0;
};

template <typename T>
T PostOrderTreeIterator<T>::Next() {
  assert(HasNext());

  while (true) {
    auto [node, visited] = stack_.PopFront().value();
    // If we have already stacked up this node's children,
    // we are ready to return its value.
    if (visited) {
      return node->value;
    }

    // When this node is popped out next time, it's value
    // will be returned.
    stack_.PushFront(std::make_pair(node, true));

    // Add the left child last to make sure it's popped out
    // first.
    if (node->right != nullptr) {
      stack_.PushFront(std::make_pair(node->right, false));
    }
    if (node->left != nullptr) {
      stack_.PushFront(std::make_pair(node->left, false));
    }
  }
}

// Explicit instantiation for common use in tests.
template class PostOrderTreeIterator<int>;

}  // namespace cs::q::trees

// cs/q/trees/view/from_left.cc
#include <utility>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// Compute the left view of a binary tree using BFS.
// Template implementation lives in this .cc and is included
// by view.hh.
template <typename T>
queue::Queue<T> LeftViewBFS(Node<T>* root) {
  if (root == nullptr) {
    return queue::Queue<T>();
  }
  queue::Queue<T> leftView;
  queue::Queue<Node<T>*> current;
  current.PushBack(root);

  while (current.Size() > 0) {
    queue::Queue<Node<T>*> next;
    bool first = true;

    while (current.Size() > 0) {
      Node<T>* parent = current.PopFront().value();
      if (first) {
        leftView.PushBack(parent->value);
        first = false;
      }
      if (parent->left != nullptr) {
        next.PushBack(parent->left);
      }
      if (parent->right != nullptr) {
        next.PushBack(parent->right);
      }
    }
    current = std::move(next);
  }
  return leftView;
}

// Explicit instantiation for tested type.
template queue::Queue<int> LeftViewBFS<int>(
    Node<int>* root);

}  // namespace cs::q::trees

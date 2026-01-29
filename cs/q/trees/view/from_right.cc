// cs/q/trees/view/from_right.cc
// cs/q/trees/view_from_right.cc
#include <string>
#include <utility>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// Compute the right view of a binary tree using BFS.
template <typename T>
queue::Queue<T> RightViewBFS(Node<T>* root) {
  if (root == nullptr) {
    return queue::Queue<T>();
  }
  queue::Queue<T> rightView;
  queue::Queue<Node<T>*> current;
  current.PushBack(root);

  while (current.Size() > 0) {
    queue::Queue<Node<T>*> next;

    while (current.Size() > 0) {
      Node<T>* parent = current.PopFront().value();
      if (current.Size() == 0) {
        // Last node in level.
        rightView.PushBack(parent->value);
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
  return rightView;
}

// Explicit instantiations for tested types.
template queue::Queue<int> RightViewBFS<int>(
    Node<int>* root);
template queue::Queue<std::string>
RightViewBFS<std::string>(Node<std::string>* root);

}  // namespace cs::q::trees

// cs/q/trees/node.hh
#ifndef CS_Q_TREES_NODE_HH
#define CS_Q_TREES_NODE_HH

#include <utility>

namespace cs::q::trees {

template <typename T>
struct Node {
  T value;
  Node<T>* left;
  Node<T>* right;

  template <typename U>
  explicit Node(U&& val)
      : value(std::forward<U>(val)),
        left(nullptr),
        right(nullptr) {}
  explicit Node(T value, Node<T>* left, Node<T>* right)
      : value(value), left(left), right(right) {}
};
}  // namespace cs::q::trees

#endif  // CS_Q_TREES_NODE_HH

// cs/q/trees/traverse.hh
#ifndef CS_Q_TREES_TRAVERSE_HH
#define CS_Q_TREES_TRAVERSE_HH

#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

template <typename T>
std::vector<T> TraverseInOrder(Node<T>* root);

template <typename T>
std::vector<T> TraversePreOrder(Node<T>* root);

template <typename T>
std::vector<T> TraversePostOrder(Node<T>* root);

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_TRAVERSE_HH

// cs/q/trees/vertical_order/vertical_order.cc
#include "cs/q/trees/vertical_order/vertical_order.hh"

namespace cs::q::trees {

template <typename T>
std::vector<std::vector<T>> TraverseVertically(
    Node<T>* /*root*/) {
  return {};
}

// Explicit instantiation for int.
template std::vector<std::vector<int>>
TraverseVertically<int>(Node<int>* root);

}  // namespace cs::q::trees

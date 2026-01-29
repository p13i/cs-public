// cs/q/trees/vertical_order/vertical_order.hh
#ifndef CS_Q_TREES_VERTICAL_ORDER_VERTICAL_ORDER_HH
#define CS_Q_TREES_VERTICAL_ORDER_VERTICAL_ORDER_HH

#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

template <typename T>
std::vector<std::vector<T>> TraverseVertically(
    Node<T>* root);

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_VERTICAL_ORDER_VERTICAL_ORDER_HH

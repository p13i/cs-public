// cs/q/trees/nodes_distance_k/nodes_distance_k.hh
// cs/q/trees/nodes_distance_k/nodes_distance_k.hh
// cs/q/trees/nodes_distance_k.hh
#ifndef CS_Q_TREES_NODES_DISTANCE_K_NODES_DISTANCE_K_HH
#define CS_Q_TREES_NODES_DISTANCE_K_NODES_DISTANCE_K_HH

#include <vector>

#include "cs/q/trees/node.hh"

namespace cs::q::trees {

template <typename T>
std::vector<Node<T>*> AllNodesDistanceK(
    Node<T>* root, Node<T>* target, const unsigned int k);

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_NODES_DISTANCE_K_NODES_DISTANCE_K_HH

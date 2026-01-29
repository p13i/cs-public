// cs/q/trees/view.hh
#ifndef CS_Q_TREES_VIEW_HH
#define CS_Q_TREES_VIEW_HH

#include <set>
#include <tuple>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/node.hh"

namespace cs::q::trees {

template <typename T>
queue::Queue<T> LeftViewBFS(Node<T>* root);

template <typename T>
queue::Queue<T> RightViewBFS(Node<T>* root);

template <typename T>
queue::Queue<T> TopViewBFS(Node<T>* root);

}  // namespace cs::q::trees

#endif  // CS_Q_TREES_VIEW_HH

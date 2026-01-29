// cs/q/trees/view/from_top.cc
// cs/q/trees/view_from_top.cc
#include <set>
#include <string>
#include <tuple>
#include <utility>

#include "cs/q/queue/queue.hh"
#include "cs/q/trees/node.hh"

namespace cs::q::trees {

// Compute the top view of a binary tree using BFS and
// horizontal ranks.
template <typename T>
queue::Queue<T> TopViewBFS(Node<T>* root) {
  queue::Queue<T> topView;
  if (root == nullptr) {
    return topView;
  }

  std::set<int> visited_ranks;
  queue::Queue<std::tuple<int, Node<T>*>> ranked_nodes;
  ranked_nodes.PushBack(std::tuple<int, Node<T>*>{0, root});

  while (ranked_nodes.Size() > 0) {
    auto front = ranked_nodes.PopFront();
    if (!front.has_value()) {
      break;  // LCOV_EXCL_LINE
    }
    const auto& [rank, node] = std::move(front.value());

    // If this rank wasn't seen, add it to the top view.
    if (visited_ranks.find(rank) == visited_ranks.end()) {
      visited_ranks.insert(rank);
      if (rank < 0) {
        topView.PushFront(node->value);
      } else {
        topView.PushBack(node->value);
      }
    }
    if (node->left != nullptr) {
      ranked_nodes.PushBack(
          std::tuple<int, Node<T>*>{rank - 1, node->left});
    }
    if (node->right != nullptr) {
      ranked_nodes.PushBack(
          std::tuple<int, Node<T>*>{rank + 1, node->right});
    }
  }

  return topView;
}

// Explicit instantiations for tested types.
template queue::Queue<int> TopViewBFS<int>(Node<int>* root);
template queue::Queue<std::string> TopViewBFS<std::string>(
    Node<std::string>* root);

}  // namespace cs::q::trees

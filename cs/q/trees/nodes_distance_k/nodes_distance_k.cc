// cs/q/trees/nodes_distance_k/nodes_distance_k.cc
#include "cs/q/trees/nodes_distance_k/nodes_distance_k.hh"

#include <map>
#include <set>
#include <vector>

#include "cs/q/queue/queue.hh"
#include "cs/q/stack/stack.hh"

#define SET_HAS_KEY(s, k) ((s).find(k) != (s).end())
#define MAP_HAS_KEY(m, k) ((m).find(k) != (m).end())

namespace {
using ::cs::q::stack::Stack;
using ::cs::q::trees::Node;

template <typename T>
bool CollectPath(Node<T>* node, Node<T>* target,
                 Stack<Node<T>*>& path) {
  if (node == nullptr) {
    return false;
  }
  path.PushFront(node);
  if (node == target) {
    return true;
  }
  if (CollectPath(node->left, target, path)) {
    return true;
  }
  if (CollectPath(node->right, target, path)) {
    return true;
  }
  path.PopFront();
  return false;
}

template <typename T>
std::map<Node<T>*, Node<T>*> BuildLookup(
    Node<T>* root, Node<T>* target, const unsigned int k) {
  Stack<Node<T>*> stack;
  if (!CollectPath(root, target, stack)) return {};

  // Ignore elements not in the top k of the stack.
  unsigned int i = 0;
  // Build a lookup from each element to its parent up the
  // stack.
  std::map<Node<T>*, Node<T>*> lookup;

  while (i < k && stack.Size() >= 2) {
    Node<T>* child = stack.PopFront().value();
    Node<T>* parent = stack.PopFront().value();
    lookup[child] = parent;
    stack.PushFront(parent);
    i++;
  }

  return lookup;
}

}  // namespace

namespace cs::q::trees {

template <typename T>
std::vector<Node<T>*> AllNodesDistanceK(
    Node<T>* root, Node<T>* target, const unsigned int k) {
  // Check arguments.
  if (root == nullptr || target == nullptr) {
    return {};
  }

  if (k == 0) {
    return {target};
  }

  // Find the k-th parent of target, building a node lookup
  // structure for the parents.
  std::map<Node<T>*, Node<T>*> lookup =
      BuildLookup(root, target, k);

  // Do a three-way depth-limited BFS.
  queue::Queue<Node<T>*> queue;
  queue.PushBack(target);
  unsigned int i = 0;
  std::set<Node<T>*> visited;
  visited.insert(target);
  while (i < k) {
    queue::Queue<Node<T>*> children;
    while (queue.Size() > 0) {
      Node<T>* node = queue.PopFront().value();
      // Add left child if we haven't already seen it.
      if (node->left != nullptr) {
        if (!SET_HAS_KEY(visited, node->left)) {
          children.PushBack(node->left);
          visited.insert(node->left);
        }
      }
      // Add right child if we haven't already seen it.
      if (node->right != nullptr) {
        if (!SET_HAS_KEY(visited, node->right)) {
          children.PushBack(node->right);
          visited.insert(node->right);
        }
      }
      if (MAP_HAS_KEY(lookup, node) &&
          !SET_HAS_KEY(visited, lookup[node])) {
        children.PushBack(lookup[node]);
        visited.insert(lookup[node]);
      }
    }
    queue = std::move(children);
    i++;
  }

  std::vector<Node<T>*> results;
  while (queue.Size() > 0) {
    results.push_back(queue.PopFront().value());
  }
  return results;
}

// Explicit instantiation for tested types.
template std::vector<Node<int>*> AllNodesDistanceK(
    Node<int>* /*root*/, Node<int>* /*target*/,
    const unsigned int /*k*/);

}  // namespace cs::q::trees

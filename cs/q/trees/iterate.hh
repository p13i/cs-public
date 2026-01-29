// cs/q/trees/iterate.hh
#ifndef CS_Q_TREES_ITERATE_HH
#define CS_Q_TREES_ITERATE_HH

#include <utility>

#include "cs/q/stack/stack.hh"
#include "cs/q/trees/node.hh"

namespace cs::q::trees {

template <typename T>
class TreeIterator {
 public:
  explicit TreeIterator(const Node<T>* root)
      : root_(root) {}
  virtual bool HasNext() const { return false; }
  virtual T Next() { return T(); }

 protected:
  const Node<T>* root_;
};

// Leetcode 173: Binary Search Tree Iterator.
// Stack-based approach:
// [https://www.youtube.com/watch?v=RXy5RzGF5wo]
template <typename T>
class InOrderTreeIterator : public TreeIterator<T> {
 public:
  // Can pre-populate the stack for the first Next call.
  explicit InOrderTreeIterator(const Node<T>* root);
  // Should be O(1).
  bool HasNext() const;
  // Should be at most O(h) where h is the depth of the
  // tree.
  T Next();

 protected:
  cs::q::stack::Stack<const Node<T>*> stack_;
};

// In-order binary tree traversal using the Morris In-Order
// Traversal algorithm:
// [https://www.youtube.com/watch?v=wGXB9OWhPTg]
template <typename T>
class MorrisInOrderTreeIterator : public TreeIterator<T> {
 public:
  bool HasNext() const;
  T Next();
};

template <typename T>
class PostOrderTreeIterator : public TreeIterator<T> {
 public:
  explicit PostOrderTreeIterator(const Node<T>* root);
  bool HasNext() const;
  T Next();

 protected:
  cs::q::stack::Stack<std::pair<const Node<T>*, bool>>
      stack_;
};

template <typename T>
class PreOrderTreeIterator : public TreeIterator<T> {
 public:
  explicit PreOrderTreeIterator(const Node<T>* root);
  bool HasNext() const;
  T Next();

 protected:
  cs::q::stack::Stack<const Node<T>*> stack_;
};
};  // namespace cs::q::trees

#endif  // CS_Q_TREES_ITERATE_HH

// cs/q/stack/stack.hh
#ifndef CS_Q_STACK_STACK_HH
#define CS_Q_STACK_STACK_HH

#include <cstddef>
#include <optional>

namespace cs::q::stack {

template <typename T>
struct Node {
  T value;
  Node<T>* next;
};

template <typename T>
class SinglyLinkedList {
 public:
  SinglyLinkedList() : _head(nullptr), _size(0) {}
  SinglyLinkedList(const SinglyLinkedList&) = delete;
  SinglyLinkedList& operator=(const SinglyLinkedList&) =
      delete;

  template <typename U>
  void PushFront(U&& value) {
    Node<T>* node = new Node<T>();
    node->value =
        std::forward<U>(value);  // works for copy & move
    node->next = _head;
    _head = node;
    ++_size;
  }

  std::optional<T> PeekFront() {
    if (_size == 0) return std::nullopt;
    return _head->value;  // requires T copyable
  }

  std::optional<T> PopFront() {
    if (_size == 0) return std::nullopt;
    T value = std::move(_head->value);  // **move**
    Node<T>* next = _head->next;
    delete _head;
    _head = next;
    --_size;
    return value;
  }

  size_t Size() const { return _size; }

  ~SinglyLinkedList() {
    while (_head != nullptr) {
      Node<T>* next = _head->next;
      delete _head;
      _head = next;
    }
  }

 private:
  Node<T>* _head;
  size_t _size;
};

template <typename T>
class Stack {
 public:
  Stack() = default;

  template <typename U>
  void PushFront(U&& value) {
    _list.PushFront(std::forward<U>(value));
  }
  std::optional<T> PopFront() { return _list.PopFront(); }
  std::optional<T> PeekFront() { return _list.PeekFront(); }
  size_t Size() const { return _list.Size(); }

 private:
  SinglyLinkedList<T> _list;
};

}  // namespace cs::q::stack

#endif  // CS_Q_STACK_STACK_HH
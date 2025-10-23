#ifndef CS_Q_QUEUE_QUEUE_HH
#define CS_Q_QUEUE_QUEUE_HH

#include <stdio.h>

#include <cstddef>
#include <optional>
#include <utility>

namespace cs::q::queue {

template <typename T>
struct Node {
  T value;
  Node<T>* next;
  Node<T>* prev;

  template <typename U>
  explicit Node(U&& val)
      : value(std::forward<U>(val)),
        next(nullptr),
        prev(nullptr) {}
};

template <typename T>
class DoublyLinkedList {
 public:
  DoublyLinkedList()
      : _head(nullptr), _tail(nullptr), _size(0) {}
  DoublyLinkedList(const DoublyLinkedList&) = delete;
  DoublyLinkedList& operator=(const DoublyLinkedList&) =
      delete;

  DoublyLinkedList(DoublyLinkedList&& other) noexcept
      : _head(other._head),
        _tail(other._tail),
        _size(other._size) {
    other._head = nullptr;
    other._tail = nullptr;
    other._size = 0;
  }

  DoublyLinkedList& operator=(
      DoublyLinkedList&& other) noexcept {
    if (this != &other) {
      Clear();
      _head = other._head;
      _tail = other._tail;
      _size = other._size;
      other._head = nullptr;
      other._tail = nullptr;
      other._size = 0;
    }
    return *this;
  }

  template <typename U>
  void PushFront(U&& value) {
    Node<T>* node = new Node<T>(std::forward<U>(value));
    node->prev = nullptr;
    node->next = _head;

    if (_head != nullptr) {
      _head->prev = node;
    }
    _head = node;

    if (_tail == nullptr) {
      _tail = node;
    }

    _size++;
  }

  template <typename U>
  void PushBack(U&& value) {
    Node<T>* node = new Node<T>(std::forward<U>(value));
    node->prev = _tail;

    if (_tail) {
      _tail->next = node;
    }
    _tail = node;

    if (!_head) {
      _head = node;
    }
    _size++;
  }

  std::optional<T> PeekFront() const {
    if (_head == nullptr) {
      return std::nullopt;
    }
    return _head->value;
  }

  std::optional<T> PeekBack() const {
    if (_tail == nullptr) {
      return std::nullopt;
    }
    return _tail->value;
  }

  std::optional<T> PopFront() {
    if (_head == nullptr) {
      return std::nullopt;
    }
    Node<T>* node = _head;
    T value = std::move(node->value);
    _head = _head->next;
    if (_head) {
      _head->prev = nullptr;
    } else {
      _tail = nullptr;  // list became empty
    }

    delete node;
    _size--;
    return value;
  }

  std::optional<T> PopBack() {
    if (!_tail) {
      return std::nullopt;
    }
    Node<T>* node = _tail;
    T value = std::move(node->value);
    _tail = _tail->prev;
    if (_tail) {
      _tail->next = nullptr;
    } else {
      _head = nullptr;
    }

    delete node;
    _size--;
    return value;
  }

  size_t Size() const { return _size; }

  ~DoublyLinkedList() { Clear(); }

 private:
  void Clear() {
    while (_head) {
      Node<T>* next = _head->next;
      delete _head;
      _head = next;
    }
    _tail = nullptr;
    _size = 0;
  }

  Node<T>* _head;
  Node<T>* _tail;
  size_t _size;
};

template <typename T>
class Queue {
 public:
  Queue() = default;
  Queue(Queue&&) = default;
  Queue& operator=(Queue&&) = default;
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  template <typename U>
  void PushFront(U&& value) {
    _list.PushFront(std::forward<U>(value));
  }

  template <typename U>
  void PushBack(U&& value) {
    // printf("PushBack\n");
    _list.PushBack(std::forward<U>(value));
  }

  std::optional<T> PopFront() { return _list.PopFront(); }
  std::optional<T> PopBack() { return _list.PopBack(); }

  std::optional<T> PeekFront() const {
    return _list.PeekFront();
  }
  std::optional<T> PeekBack() const {
    return _list.PeekBack();
  }

  size_t Size() const { return _list.Size(); }

 private:
  DoublyLinkedList<T> _list;
};

}  // namespace cs::q::queue

#endif  // CS_Q_QUEUE_QUEUE_HH

// cs/q/heap/heap.hh
#ifndef CS_Q_HEAP_HEAP_HH
#define CS_Q_HEAP_HEAP_HH

#include <limits.h>
#include <stdlib.h>

#include <tuple>

namespace cs::q {
// Indexing helpers.
static inline int Left(int i) { return 2 * i; }

static inline int Right(int i) { return 2 * i + 1; }

static inline int Parent(int i) { return i / 2; }

template <typename T>
static inline void Swap(T* values, int i1, int i2) {
  T temp = values[i1];
  values[i1] = values[i2];
  values[i2] = temp;
}

template <typename T>
class MaxHeap {
 protected:
  size_t _size;
  size_t _capacity;
  std::pair<T, int>* _values;

 public:
  MaxHeap(size_t capacity) : _capacity(capacity) {
    this->_size = 0;
    this->_values = new std::pair<T, int>[_capacity + 1];
  }
  virtual bool Insert(T value, int priority) {
    if (Size() == Capacity()) {
      return false;
    }
    this->_size++;
    this->_values[Size()] = {value, priority};
    this->HeapifyUp(Size());
    return true;
  }
  std::pair<T, int> Peek() {
    if (Size() == 0) {
      return {T(), INT_MIN};
    }
    return this->_values[1];
  }
  std::pair<T, int> Pop() {
    if (Size() == 0) {
      return {T(), INT_MIN};
    }
    std::pair<T, int> max = this->Peek();
    this->_values[1] = this->_values[_size];
    this->_size--;
    this->HeapifyDown(1);
    return max;
  }
  size_t Size() { return _size; }
  size_t Capacity() { return _capacity; }

 private:
  static int Priority(std::pair<T, int> vp) {
    return std::get<1>(vp);
  }
  static T Value(std::pair<T, int> vp) {
    return std::get<0>(vp);
  }

  void HeapifyDown(size_t i) {
    if (i > Size()) {
      return;
    }
    size_t left = Left(i);
    size_t right = Right(i);
    size_t max = i;
    if (left <= Size() &&
        Priority(this->_values[left]) >
            Priority(this->_values[max])) {
      max = left;
    }
    if (right <= Size() &&
        Priority(this->_values[right]) >
            Priority(this->_values[max])) {
      max = right;
    }
    if (max != i) {
      Swap(this->_values, i, max);
      HeapifyDown(max);
    }
  }

  void HeapifyUp(size_t i) {
    if (i <= 1) {
      return;
    }
    size_t parent = Parent(i);
    if (parent > 0 && Priority(this->_values[i]) >
                          Priority(this->_values[parent])) {
      Swap(this->_values, i, parent);
      HeapifyUp(parent);
    }
  }
};

template <typename T>
class MinHeap : public MaxHeap<T> {
 public:
  MinHeap(size_t capacity) : MaxHeap<T>(capacity) {}
  bool Insert(T value, int priority) override {
    return MaxHeap<T>::Insert(value, -1 * priority);
  }
};
}  // namespace cs::q
#endif  // CS_Q_HEAP_HEAP_HH
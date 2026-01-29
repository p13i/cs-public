// cs/q/heap/heap.h
#ifndef CS_Q_HEAP_HEAP_H
#define CS_Q_HEAP_HEAP_H

#include <stdbool.h>

typedef struct Heap {
  int size;
  int capacity;
  int* values;
} Heap;

// Creates a new heap with the given, fixed capacity.
Heap* NewHeap(int capacity);
// Applies the heap property downward from the given root.
void HeapifyDown(Heap* heap, int i);
// Applies the heap property upward from the given child.
void HeapifyUp(Heap* heap, int i);
// Tries to insert a new value into the heap. Returns true
// if a value was inserted.
// O(log(n)).
bool Insert(Heap* heap, int value);
// Presents the max element in this heap without removing.
int PeekMax(Heap* heap);
// Removes and returns the max value in the heap.
// O(log(n)).
int PopMax(Heap* heap);
// Prints out a representation of the heap.
// void PrintHeap(Heap* heap);

#endif  // CS_Q_HEAP_HEAP_H
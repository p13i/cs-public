#ifndef CS_Q_KTH_LARGEST_VALUE_PRIORITY_QUEUE_H
#define CS_Q_KTH_LARGEST_VALUE_PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct PriorityQueue {
  // length of heap
  int k;
  int* heap;
} PriorityQueue;

// Initializes the given priority queue to the given k and
// heap values.
PriorityQueue* NewPriorityQueue(int k);

// Add a value to the heap if the given value is greater
// than the smallest element of the heap
bool Insert(PriorityQueue* pq, int value);

// Returns the maximum value stored in the heap.
int PeekMax(PriorityQueue* pq);

// Returns the minimum value stored in the heap.
int PeekMin(PriorityQueue* pq);

// Removes the smallest element from the given priority
// queue, returning its value.
int PopMin(PriorityQueue* pq);

// Pretty-prints the entire heap and the k value.
void Print(PriorityQueue* pq);

#endif  // CS_Q_KTH_LARGEST_VALUE_PRIORITY_QUEUE_H
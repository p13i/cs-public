#include "heap.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// Sources:
// https://www.cs.umd.edu/~meesh/351/mount/lectures/lect13-heapsort.pdf
// https://stackoverflow.com/questions/46900859/given-k-sorted-lists-of-up-to-n-elements-in-each-list-return-a-sorted-iterator
// https://www.geeksforgeeks.org/dsa/insertion-and-deletion-in-heaps/

// Indexing helpers.
int Left(int i) { return 2 * i; }

int Right(int i) { return 2 * i + 1; }

int Parent(int i) { return i / 2; }

void Swap(int* values, int i1, int i2) {
  printf("Swap(int* values, int i1=%d, int i2=%d)\n", i1,
         i2);
  int temp = values[i1];
  values[i1] = values[i2];
  values[i2] = temp;
}

Heap* NewHeap(int capacity) {
  Heap* heap = (Heap*)malloc(sizeof(Heap));
  if (heap == NULL) {
    return heap;
  }
  heap->size = 0;
  heap->capacity = capacity;
  // Add extra spot so we can use 1-indexing
  int* values =
      (int*)calloc(heap->capacity + 1, sizeof(int));
  if (values == NULL) {
    free(heap);
    return NULL;
  }
  heap->values = values;
  return heap;
}

void HeapifyDown(Heap* heap, int i) {
  if (i > heap->size) {
    return;
  }
  int left = Left(i);
  int right = Right(i);
  int max = i;
  if (left <= heap->size &&
      heap->values[left] > heap->values[max]) {
    max = left;
  }
  if (right <= heap->size &&
      heap->values[right] > heap->values[max]) {
    max = right;
  }
  if (max != i) {
    Swap(heap->values, i, max);
    HeapifyDown(heap, max);
  }
}

void HeapifyUp(Heap* heap, int i) {
  if (i <= 1) {
    return;
  }
  int parent = Parent(i);
  if (parent > 0 &&
      heap->values[i] > heap->values[parent]) {
    Swap(heap->values, i, parent);
    HeapifyUp(heap, parent);
  }
}

bool Insert(Heap* heap, int value) {
  if (heap->size == heap->capacity) {
    return false;
  }
  heap->size++;
  heap->values[heap->size] = value;
  HeapifyUp(heap, heap->size);
  return true;
}

int PeekMax(Heap* heap) {
  if (heap->size == 0) {
    return INT_MIN;
  }
  return heap->values[1];
}

int PopMax(Heap* heap) {
  if (heap->size == 0) {
    return INT_MIN;
  }
  int max = PeekMax(heap);
  // Move the last index to the beginning.
  heap->values[1] = heap->values[heap->size];
  heap->size--;
  // Bubble the new root down as needed.
  HeapifyDown(heap, 1);
  return max;
}
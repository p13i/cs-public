#include "priority_queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

PriorityQueue* NewPriorityQueue(int k) {
  printf("NewPriorityQueue(k=%d)\n", k);
  fflush(stdout);
  PriorityQueue* pq =
      (PriorityQueue*)malloc(sizeof(PriorityQueue));
  if (pq == NULL) {
    fprintf(stderr, "Unable to allocate PriorityQueue\n");
    fflush(stdout);
    return NULL;
  }
  printf("Allocated for PriorityQueue*\n");
  fflush(stdout);
  pq->k = k;
  int* heap = (int*)calloc(pq->k, sizeof(int));
  if (heap == NULL) {
    fprintf(stderr,
            "Unable to allocate heap of size k=%d\n",
            pq->k);
    fflush(stdout);
    return NULL;
  }
  printf("Zero-ed out heap\n");
  fflush(stdout);
  pq->heap = heap;
  return pq;
}

bool Insert(PriorityQueue* pq, int value) { return false; }

int PeekMax(PriorityQueue* pq) { return pq->heap[0]; }

int PeekMin(PriorityQueue* pq) { return __INT_MAX__; }

int PopMin(PriorityQueue* pq) { return __INT_MAX__; }

void Print(PriorityQueue* pq) {
  printf("PriorityQueue {\n  k = %d\n  heap=[", pq->k);
  fflush(stdout);
  bool first = true;
  for (int i = 0; i < pq->k; i++) {
    if (first) {
      first = false;
    } else {  // !first
      printf(", ");
      fflush(stdout);
    }
    fflush(stdout);
    printf("%d", pq->heap[i]);
    fflush(stdout);
  }
  printf("]\n}\n");
  fflush(stdout);
}

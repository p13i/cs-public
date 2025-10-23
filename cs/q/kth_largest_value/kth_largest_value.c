#include "kth_largest_value.h"

#include <stdio.h>

#include "priority_queue.h"

int KthLargestValue(int k, int length, int* values) {
  printf("I'm here 3!\n");
  fflush(stdout);
  printf("KthLargestValue()\n");
  fflush(stdout);
  PriorityQueue* pq = NewPriorityQueue(k);
  Print(pq);
  fflush(stdout);
  for (int i = 0; i < length; i++) {
    if (Insert(pq, values[i])) {
      printf("Inserted values[i]=%d into pq.\n", values[i]);
      fflush(stdout);
    } else {
      printf("Did not insert values[i]=%d into pq.\n",
             values[i]);
      fflush(stdout);
    }
    Print(pq);
    fflush(stdout);
  }
  fflush(stdout);
  return PeekMin(pq);
}

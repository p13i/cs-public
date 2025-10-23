# Leetcode 215: Find the k-th largest value in an array

https://leetcode.com/problems/kth-largest-element-in-an-array/

Given an integer array and an integer number k. Return the
k-th largest element in the array.

```
Examples:
array = [5, -3, 9, 1]
* k = 0 => return: 9
* k = 1 => return: 5
* k = 3 = return:* -3
```

## Potential solutions

Array is not sorted.

As we iterate through every element of the array, we want to
store a list of the k largest values in `k-array`. If we
come across an array value that is larger than the smallest
element of the saved `k-array`. To maintain `k-array` we
could:

1. Sort `k-array` and return the k-th to last element,
   taking at best `O(n * log(n))` time and potentially
   `O(log(n))` space.
2. Insert the new element into `k-array` but k-array can be
   a priority queue that keeps the highest `k` values. The
   priority queue could be managed with a heap data
   structure which represents sorted values in a complete
   binary tree. Space complexity is `O(k)` for the priority
   queue (which as a depth of `log(k)`). Time complexity is
   `O(n + n * log(k))`; the `n * log(k)` account for
   potentially having to insert a new value at every element
   of the priority queue (as would be the case in a sorted
   array).

### Heap/priority queue approach

The priority has an interface like:

```c
struct PriorityQueue {
    // length of heap
    int k;
    int* heap;
}

// Initializes the given priority queue to the given k and
// heap values.
void Init(PriorityQueue* pq, int k, int* heap);

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
```

The k-th largest value algorithm has an interface like:

```c
// Returns the k-th largest value amongst the given values.
int KthLargestValue(int k, int length, int* values);
```

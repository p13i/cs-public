#include "cs/q/search_big_array/search_big_array.h"

#include <stdio.h>

static int* ArrayReaderNums;
static int ArrayReaderLength;

// Source: https://p13i.io/bytes/binary_search/
int BinarySearch(int* array, int target, int start,
                 int end) {
  if (start > end) {
    return -1;
  }

  int middle = (start + end) / 2;

  if (array[middle] == target) {
    return middle;
  }

  if (target < array[middle]) {
    return BinarySearch(array, target, start, middle - 1);
  } else {
    return BinarySearch(array, target, middle + 1, end);
  }
}

int Search(int target) {
  int start = 0;
  int end = 1;
  int gap = 1;
  int x = ArrayReaderGet(start);
  while (x < __INT_MAX__) {
    if (target < x) {
      return -1;
    }
    if (target == x) {
      return start;
    }
    // Target is > x
    int upper = ArrayReaderGet(end);
    if (target < upper) {
      return BinarySearch(ArrayReaderNums, target, start,
                          end);
    }
    // Then target >= upper
    int oldStart = start;
    start = end;
    end = (oldStart + gap) + 1;
    gap *= 2;
    x = ArrayReaderGet(start);
  }
  return -1;
}

void InitArrayReader(int* nums, int length) {
  ArrayReaderNums = nums;
  ArrayReaderLength = length;
}

int ArrayReaderGet(int index) {
  if (index < 0) {
    return __INT_MAX__;
  }
  int length = ArrayReaderLength;
  if (index >= length) {
    return __INT_MAX__;
  }
  return ArrayReaderNums[index];
}

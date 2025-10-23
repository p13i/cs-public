#ifndef CS_Q_SEARCH_BIG_ARRAY_SEARCH_BIG_ARRAY_H
#define CS_Q_SEARCH_BIG_ARRAY_SEARCH_BIG_ARRAY_H

// Searches a strictly ascending stream numbers.
// Returns the index where the target was found, or -1 if
// the target was not found.
int Search(int target);
// Sets the stream used by ArrayReaderGet to an array value.
void InitArrayReader(int* nums, int length);
// Gets the value at a particular index, or INT_MAX
// (2147483647)
int ArrayReaderGet(int index);

#endif  // CS_Q_SEARCH_BIG_ARRAY_SEARCH_BIG_ARRAY_H
extern "C" {
#include "cs/q/search_big_array/search_big_array.h"
}

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::Gt;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::Not;
}  // namespace

namespace {  // use_usings
int* MakeArray(int length) {
  EXPECT_THAT(length, Gt(0));
  int* array = (int*)malloc(sizeof(int) * length);
  EXPECT_THAT(array, Not(Eq((int*)NULL)));
  return array;
}

TEST(InitArrayReader, Success) {
  int length = 3;
  int* array = MakeArray(length);
  InitArrayReader(array, length);
}

TEST(ArrayReaderGet, Success) {
  int length = 3;
  int* array = MakeArray(length);
  array[0] = -1;
  array[1] = 0;
  array[2] = 1;
  InitArrayReader(array, length);

  EXPECT_THAT(ArrayReaderGet(0), -1);
  EXPECT_THAT(ArrayReaderGet(1), 0);
  EXPECT_THAT(ArrayReaderGet(2), 1);
}

/*
Example 1:

```
Input: array = [-1,0,3,5,9,12], target = 9
Output: 4
Explanation: 9 exists in nums and its index is 4
```
*/
TEST(Search, LeetcodeExample1) {
  int length = 6;
  int* array = MakeArray(length);
  array[0] = -1;
  array[1] = 0;
  array[2] = 3;
  array[3] = 5;
  array[4] = 9;
  array[5] = 12;
  InitArrayReader(array, length);

  EXPECT_THAT(Search(9), Eq(4));
}

/*
Example 2:

```
Input: array = [-1,0,3,5,9,12], target = 9
Output: 4
Explanation: 9 exists in nums and its index is 4
```
*/
TEST(Search, LeetcodeExample2) {
  int length = 6;
  int* array = MakeArray(length);
  array[0] = -1;
  array[1] = 0;
  array[2] = 3;
  array[3] = 5;
  array[4] = 9;
  array[5] = 12;
  InitArrayReader(array, length);

  EXPECT_THAT(Search(2), Eq(-1));
}

// From ChatGPT:

// Helper to initialize array from std::vector
void InitArrayReaderFromVector(
    const std::vector<int>& vec) {
  InitArrayReader(const_cast<int*>(vec.data()),
                  static_cast<int>(vec.size()));
}

TEST(SearchInUnknownSizeArrayTest, TargetDoesNotExist) {
  std::vector<int> nums = {-1, 0, 3, 5, 9, 12};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(2), -1);
}

TEST(SearchInUnknownSizeArrayTest, TargetIsFirstElement) {
  std::vector<int> nums = {-5, 1, 4, 7, 10};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(-5), 0);
}

TEST(SearchInUnknownSizeArrayTest, TargetIsLastElement) {
  std::vector<int> nums = {-5, 1, 4, 7, 10};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(10), 4);
}

TEST(SearchInUnknownSizeArrayTest, SingleElementFound) {
  std::vector<int> nums = {42};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(42), 0);
}

TEST(SearchInUnknownSizeArrayTest, SingleElementNotFound) {
  std::vector<int> nums = {42};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(99), -1);
}

TEST(SearchInUnknownSizeArrayTest, EmptyArray) {
  std::vector<int> nums = {};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(5), -1);
}

TEST(SearchInUnknownSizeArrayTest, NegativeNumbers) {
  std::vector<int> nums = {-9999, -5000, -100, -1};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(-5000), 1);
}

TEST(SearchInUnknownSizeArrayTest, LargePositiveNumbers) {
  std::vector<int> nums = {5000, 7000, 9000, 9999};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(Search(9999), 3);
}

TEST(SearchInUnknownSizeArrayTest,
     ArrayReaderOutOfBoundsReturnsINTMAX) {
  std::vector<int> nums = {1, 3, 5};
  InitArrayReaderFromVector(nums);
  EXPECT_EQ(ArrayReaderGet(10), INT_MAX);
}

}  // namespace
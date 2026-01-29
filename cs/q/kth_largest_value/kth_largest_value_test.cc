// cs/q/kth_largest_value/kth_largest_value_test.cc
extern "C" {
#include "kth_largest_value.h"
}

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;
}  // namespace

namespace {
#if 0
TEST(KthLargestValue, BaseCase) {
  std::cout << "Hello." << std::endl;
  std::cout << "Hello." << std::endl;
  int length = 3;
  int values[] = {1, 2, 3};
  std::cout << "Hello." << std::endl;
  EXPECT_THAT(KthLargestValue(1, length, values), Eq(3));
  std::cout << "Hello." << std::endl;
}
#endif
}  // namespace
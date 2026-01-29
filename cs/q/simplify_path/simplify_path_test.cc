// cs/q/simplify_path/simplify_path_test.cc
extern "C" {
#include "cs/q/simplify_path/simplify_path.h"
}

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::Gt;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::Not;
}  // namespace

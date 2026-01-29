// cs/q/trees/iterate/in_order_test.cc
#include <stdio.h>

#include <iostream>

#include "cs/q/trees/iterate.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::Not;
using ::testing::StrEq;
}  // namespace

namespace {
// Test cases: null checks; bad args checks, base case test,
// test that has no left subtree (skipping ctor logic), test
// that only has a right subtree chain; test with a "hidden"
// subtree meaning that there is only a "thin" and "long"
// connection from the majority of the tree to another very
// large subtree (making sure that no data are ignored).
}  // namespace

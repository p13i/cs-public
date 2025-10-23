#include "cs/net/json/object.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::Result;
using ::cs::net::json::Object;
using ::cs::net::json::Type;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

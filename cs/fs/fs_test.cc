#include "cs/fs/fs.hh"

#include <stdio.h>

#include <filesystem>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::Result;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::StrEq;

class FileSystemTest : public ::testing::Test {};

TEST_F(FileSystemTest, WriteAndRead) {
  auto cwd = cs::fs::cwd();
  ASSERT_OK(cwd);
  std::string test_file = "test.txt";
  std::string path = cwd.value() + "/" + test_file;
  std::string test_contents = "test_contents";
  ASSERT_OK(cs::fs::write(path, test_contents));
  auto contents = cs::fs::read(path);
  ASSERT_OK(contents);
  ASSERT_THAT(contents.value(), StrEq(test_contents));
}

TEST_F(FileSystemTest, cwd) {
  auto path = cs::fs::cwd();
  ASSERT_THAT(path.ok(), IsTrue());
  ASSERT_THAT(path.value().size() > 1, IsTrue());
}

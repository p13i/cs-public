// cs/fs/fs_test.cc
#include "cs/fs/fs.hh"

#include <stdio.h>

#include <chrono>
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

namespace {
std::filesystem::path MakeTempDir() {
  auto base = std::filesystem::temp_directory_path();
  auto stamp = std::chrono::system_clock::now()
                   .time_since_epoch()
                   .count();
  std::filesystem::path path =
      base / ("cs_fs_test_" + std::to_string(stamp));
  std::filesystem::create_directories(path);
  return path;
}
}  // namespace

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

TEST_F(FileSystemTest, LsMissingDirectoryFails) {
  auto result = cs::fs::ls("/this/path/does/not/exist");
  EXPECT_THAT(result.ok(), IsFalse());
}

TEST_F(FileSystemTest, MkdirDirExistsAndIsDir) {
  auto dir = MakeTempDir();
  std::string dir_path = dir.string();
  ASSERT_OK(cs::fs::mkdir(dir_path + "/nested"));
  auto second = cs::fs::mkdir(dir_path + "/nested");
  EXPECT_THAT(second.ok(), IsFalse());
  EXPECT_THAT(cs::fs::dir_exists(dir_path + "/nested"),
              IsTrue());
  EXPECT_THAT(cs::fs::IsDir(dir_path + "/nested"),
              IsTrue());
  std::filesystem::remove_all(dir);
}

TEST_F(FileSystemTest, IsFileAndDelete) {
  auto dir = MakeTempDir();
  std::string file_path = (dir / "file.txt").string();
  ASSERT_OK(cs::fs::write(file_path, "content"));
  EXPECT_THAT(cs::fs::IsFile(file_path), IsTrue());
  ASSERT_OK(cs::fs::Delete(file_path));
  auto delete_result = cs::fs::Delete(file_path);
  EXPECT_THAT(delete_result.ok(), IsFalse());
  std::filesystem::remove_all(dir);
}

TEST_F(FileSystemTest, ReadAndWriteErrors) {
  auto dir = MakeTempDir();
  std::string missing_file = (dir / "missing.txt").string();
  auto read_result = cs::fs::read(missing_file);
  EXPECT_THAT(read_result.ok(), IsFalse());
  std::string bad_path =
      (dir / "missing_dir" / "file.txt").string();
  auto write_result = cs::fs::write(bad_path, "content");
  EXPECT_THAT(write_result.ok(), IsFalse());
  std::filesystem::remove_all(dir);
}

TEST_F(FileSystemTest, JoinTrimsTrailingSlashes) {
  auto joined = cs::fs::Join("root/", "path//", "file/");
  EXPECT_THAT(joined, StrEq("root/path/file"));
}

TEST_F(FileSystemTest, CwdFailsWhenDirectoryDeleted) {
  auto original = std::filesystem::current_path();
  auto dir = MakeTempDir();
  std::filesystem::current_path(dir);
  std::filesystem::remove_all(dir);
  auto result = cs::fs::cwd();
  EXPECT_THAT(result.ok(), IsFalse());
  std::filesystem::current_path(original);
}

TEST_F(FileSystemTest, MkdirFailsWhenParentIsFile) {
  auto dir = MakeTempDir();
  std::string file_path = (dir / "file.txt").string();
  ASSERT_OK(cs::fs::write(file_path, "content"));
  auto result = cs::fs::mkdir(file_path + "/nested");
  EXPECT_THAT(result.ok(), IsFalse());
  std::filesystem::remove_all(dir);
}

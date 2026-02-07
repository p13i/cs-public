// cs/apps/blob-service/blob_store_test.gpt.cc
#include <chrono>
#include <filesystem>
#include <string>

#include "cs/apps/blob-service/blob_store.gpt.hh"
#include "cs/fs/fs.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::fs::IsFile;
using ::cs::testing::WithValue;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
}  // namespace

namespace cs::apps::blob_service {
namespace {

std::string MakeTempBlobDir() {
  auto base = std::filesystem::temp_directory_path();
  auto stamp = std::chrono::system_clock::now()
                   .time_since_epoch()
                   .count();
  std::filesystem::path path =
      base / ("blob_store_test_" + std::to_string(stamp));
  std::filesystem::create_directories(path);
  return path.string();
}

class BlobStoreTest : public ::testing::Test {
 protected:
  void SetUp() override { blob_dir_ = MakeTempBlobDir(); }
  void TearDown() override {
    std::filesystem::remove_all(blob_dir_);
  }
  std::string blob_dir_;
};

TEST_F(BlobStoreTest, WriteThenReadReturnsSameBytes) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440000";
  std::string content = "hello blob";

  ASSERT_OK(store.Write(uuid, content));
  EXPECT_OK(store.Read(uuid),
            WithValue<std::string>(Eq(content)));
}

TEST_F(BlobStoreTest, ExistsReturnsFalseBeforeWrite) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440001";
  EXPECT_THAT(store.Exists(uuid), IsFalse());
}

TEST_F(BlobStoreTest, ExistsReturnsTrueAfterWrite) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440002";
  ASSERT_OK(store.Write(uuid, "x"));
  EXPECT_THAT(store.Exists(uuid), IsTrue());
}

TEST_F(BlobStoreTest, ReadMissingBlobFails) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440003";
  auto read_result = store.Read(uuid);
  EXPECT_THAT(read_result.ok(), IsFalse());
}

TEST_F(BlobStoreTest, WriteOverwritesExistingBlob) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440004";
  ASSERT_OK(store.Write(uuid, "first"));
  ASSERT_OK(store.Write(uuid, "second"));
  EXPECT_OK(store.Read(uuid),
            WithValue<std::string>(Eq("second")));
}

TEST_F(BlobStoreTest, FileHasBlobExtension) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440005";
  ASSERT_OK(store.Write(uuid, "x"));
  std::string path = blob_dir_ + "/" + uuid + ".blob";
  EXPECT_THAT(IsFile(path), IsTrue());
}

TEST_F(BlobStoreTest, WriteBinaryContentWithNulls) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440006";
  std::string content("a\0b\0c", 5);
  ASSERT_OK(store.Write(uuid, content));
  EXPECT_OK(store.Read(uuid),
            WithValue<std::string>(Eq(content)));
  EXPECT_THAT(store.Read(uuid).value().size(), Eq(5u));
}

TEST_F(BlobStoreTest, WriteEmptyContent) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-446655440007";
  std::string content;
  ASSERT_OK(store.Write(uuid, content));
  EXPECT_OK(store.Read(uuid),
            WithValue<std::string>(Eq("")));
}

TEST_F(BlobStoreTest, TwoDifferentUuidsTwoFiles) {
  BlobStore store(blob_dir_);
  std::string uuid1 =
      "550e8400-e29b-41d4-a716-446655440008";
  std::string uuid2 =
      "550e8400-e29b-41d4-a716-446655440009";
  ASSERT_OK(store.Write(uuid1, "one"));
  ASSERT_OK(store.Write(uuid2, "two"));
  EXPECT_OK(store.Read(uuid1),
            WithValue<std::string>(Eq("one")));
  EXPECT_OK(store.Read(uuid2),
            WithValue<std::string>(Eq("two")));
}

TEST_F(BlobStoreTest, AtomicWriteLeavesFinalFileNotTemp) {
  BlobStore store(blob_dir_);
  std::string uuid = "550e8400-e29b-41d4-a716-44665544000a";
  ASSERT_OK(store.Write(uuid, "atomic"));
  std::string final_path = blob_dir_ + "/" + uuid + ".blob";
  std::string temp_path = final_path + ".tmp";
  EXPECT_THAT(IsFile(final_path), IsTrue());
  EXPECT_THAT(IsFile(temp_path), IsFalse());
}

}  // namespace
}  // namespace cs::apps::blob_service

// cs/apps/blob-service/blob_store.gpt.cc
#include "cs/apps/blob-service/blob_store.gpt.hh"

#include <filesystem>
#include <string>

#include "cs/fs/fs.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::fs::Delete;
}  // namespace

namespace cs::apps::blob_service {

namespace {

constexpr char kBlobExtension[] = ".blob";

}  // namespace

std::string BlobStore::PathFor(
    const std::string& uuid) const {
  return cs::fs::Join(blob_dir_, uuid + kBlobExtension);
}

Result BlobStore::Write(const std::string& uuid,
                        const std::string& bytes) {
  std::string final_path = PathFor(uuid);
  std::filesystem::path p(final_path);
  std::string dir = p.parent_path().string();
  if (!cs::fs::dir_exists(dir)) {
    OK_OR_RET(cs::fs::mkdir(dir));
  }

  std::string temp_path = final_path + ".tmp";
  OK_OR_RET(cs::fs::write(temp_path, bytes));
  Result rename_result =
      cs::fs::rename(temp_path, final_path);
  if (!rename_result.ok()) {
    Delete(temp_path);
    return TRACE(rename_result);
  }
  return cs::Ok();
}

ResultOr<std::string> BlobStore::Read(
    const std::string& uuid) const {
  std::string path = PathFor(uuid);
  return cs::fs::read(path);
}

bool BlobStore::Exists(const std::string& uuid) const {
  return cs::fs::IsFile(PathFor(uuid));
}

}  // namespace cs::apps::blob_service

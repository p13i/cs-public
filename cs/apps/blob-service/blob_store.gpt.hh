// cs/apps/blob-service/blob_store.gpt.hh
#ifndef CS_APPS_BLOB_SERVICE_BLOB_STORE_GPT_HH
#define CS_APPS_BLOB_SERVICE_BLOB_STORE_GPT_HH

#include <string>

#include "cs/result.hh"

namespace cs::apps::blob_service {

// Reads and writes blob bytes under blob_dir by uuid.
// File path: {blob_dir}/{uuid}.blob. Writes are atomic
// (temp file + rename).
class BlobStore {
 public:
  explicit BlobStore(std::string blob_dir)
      : blob_dir_(std::move(blob_dir)) {}

  // Writes bytes for uuid. Atomic: write to temp then
  // rename.
  cs::Result Write(const std::string& uuid,
                   const std::string& bytes);

  // Reads bytes for uuid. Returns error if file missing.
  cs::ResultOr<std::string> Read(
      const std::string& uuid) const;

  // Returns true if blob file exists for uuid.
  bool Exists(const std::string& uuid) const;

 private:
  std::string PathFor(const std::string& uuid) const;

  std::string blob_dir_;
};

}  // namespace cs::apps::blob_service

#endif  // CS_APPS_BLOB_SERVICE_BLOB_STORE_GPT_HH

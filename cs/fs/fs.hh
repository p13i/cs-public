// cs/fs/fs.hh
#ifndef CS_FS_FS_HH
#define CS_FS_FS_HH

#include <sstream>
#include <string>

#include "cs/result.hh"

namespace cs::fs {

// Lists entries in the given directory.
cs::ResultOr<std::vector<std::string>> ls(std::string path);

// Reads the full contents of a file.
cs::ResultOr<std::string> read(std::string path);

// Writes contents to a file, overwriting if it exists.
cs::Result write(std::string path, std::string contents);

// Returns the current working directory path.
cs::ResultOr<std::string> cwd();

// Creates a directory (and parents if needed).
cs::Result mkdir(std::string path);

// Renames or moves a file or directory.
cs::Result rename(std::string old_path,
                  std::string new_path);

// Deletes a file or directory.
cs::Result Delete(std::string path);

// Returns true if the path exists and is a directory.
bool dir_exists(std::string path);

// Returns true if the path exists and is a directory.
bool IsDir(std::string path);

// Returns true if the path exists and is a regular file.
bool IsFile(std::string path);

// Joins path segments with "/", trimming trailing slashes
// from segments. Source:
// https://chatgpt.com/canvas/shared/67eb48720dc881918e7c87c05042cb39
template <typename... Args>
std::string Join(const Args&... args) {
  std::stringstream result;
  bool first = true;

  auto process = [&](const std::string& path) {
    std::string trimmed = path;
    while (!trimmed.empty() && trimmed.back() == '/') {
      trimmed.pop_back();
    }

    if (!first) {
      result << "/";
    }
    result << trimmed;
    first = false;
  };

  (process(args), ...);

  return result.str();
}
}  // namespace cs::fs

#endif  // CS_FS_FS_HH
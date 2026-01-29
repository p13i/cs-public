// cs/fs/fs.hh
#ifndef CS_FS_FS_HH
#define CS_FS_FS_HH

#include <sstream>
#include <string>

#include "cs/result.hh"

namespace cs::fs {
cs::ResultOr<std::vector<std::string>> ls(std::string path);
cs::ResultOr<std::string> read(std::string path);
cs::Result write(std::string path, std::string contents);
cs::ResultOr<std::string> cwd();
cs::Result mkdir(std::string path);
cs::Result Delete(std::string path);
bool dir_exists(std::string path);
bool IsDir(std::string path);
bool IsFile(std::string path);

// Source:
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
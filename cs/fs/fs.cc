#include "fs.hh"

#include <string.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using ::cs::Error;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ResultOr;

cs::ResultOr<std::vector<std::string>> cs::fs::ls(
    std::string path) {
  std::vector<std::string> files;
  try {
    for (const auto& entry :
         std::filesystem::directory_iterator(path)) {
      files.push_back(entry.path().string());
    }
  } catch (const std::filesystem::filesystem_error& e) {
    return TRACE(Error(e.what()));
  }
  return files;
}

ResultOr<std::string> cs::fs::read(std::string path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return TRACE(Error(
        "Failed to open file for reading. path=" + path +
        ", strerror(errno)=" + strerror(errno)));
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  if (file.fail()) {
    return TRACE(
        Error("Failed to read file. path=" + path +
              ", strerror(errno)=" + strerror(errno)));
  }

  return buffer.str();
}

Result cs::fs::write(std::string path,
                     std::string contents) {
  std::ofstream file(path);
  if (!file.is_open()) {
    return TRACE(Error(
        "Failed to open file for writing. path=" + path +
        ", strerror(errno)=" + strerror(errno)));
  }

  file << contents;
  if (file.fail()) {
    return TRACE(
        Error("Failed to write to file. path=" + path +
              ", strerror(errno)=" + strerror(errno)));
  }

  return Ok();
}

ResultOr<std::string> cs::fs::cwd() {
  try {
    std::filesystem::path cwd =
        std::filesystem::current_path();
    return std::string(cwd.c_str());
  } catch (const std::filesystem::filesystem_error& e) {
    return TRACE(Error(e.what()));
  }
}

Result cs::fs::mkdir(std::string path) {
  try {
    if (std::filesystem::create_directories(path)) {
      return Ok();
    } else {
      return TRACE(
          Error("Directory already exists or could not be "
                "created."));
    }
  } catch (const std::filesystem::filesystem_error& e) {
    return TRACE(Error(e.what()));
  }
}
bool cs::fs::dir_exists(std::string path) {
  return std::filesystem::is_directory(path) &&
         std::filesystem::exists(path);
}

#include "cs/www/app/context/context.hh"

#include <iostream>
#include <string>

namespace {
using ::cs::NotFoundError;
using ::cs::ResultOr;
}  // namespace

namespace cs::www::app {

Context& Context::GetInstance() {
  static Context instance;
  return instance;
}

ResultOr<std::string> Context::Read(std::string key) {
  if (GetInstance()._values.count(key) == 0) {
    return TRACE(
        NotFoundError("Key (" + key + ") not found."));
  }
  return GetInstance()._values[key];
}

void Context::Write(std::string key, std::string value) {
  GetInstance()._values[key] = value;
}

}  // namespace cs::www::app
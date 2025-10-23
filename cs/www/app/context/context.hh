#ifndef CS_WWW_APP_CONTEXT_CONTEXT_HH
#define CS_WWW_APP_CONTEXT_CONTEXT_HH

#include <map>
#include <string>

#include "cs/result.hh"  // Include the correct header for ResultOr

namespace cs::www::app {

class Context {
 public:
  std::map<std::string, std::string> _values;
  Context() = default;
  static Context& GetInstance();
  static cs::ResultOr<std::string> Read(std::string key);
  static void Write(std::string key, std::string value);
};

}  // namespace cs::www::app

#endif  // CS_WWW_APP_CONTEXT_CONTEXT_HH
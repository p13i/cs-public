#ifndef CS_LOG_HH
#define CS_LOG_HH

#include <stdio.h>

#include <iostream>
#include <map>
#include <ostream>
#include <string>

#include "cs/util/time.hh"

#define LOG_PRETTY_FUNCTION false
#if LOG_PRETTY_FUNCTION
#define LOG(level)                                         \
  std::cout << "["                                         \
            << cs::util::time::NowAsISO8601TimeUTC()       \
            << "] [" << #level << "] [" << __FILE__ << ":" \
            << __LINE__ << "] [" << __PRETTY_FUNCTION__    \
            << "] "
#else
#define LOG(level)                                         \
  std::cout << "["                                         \
            << cs::util::time::NowAsISO8601TimeUTC()       \
            << "] [" << #level << "] [" << __FILE__ << ":" \
            << __LINE__ << "] "
#endif  // LOG_PRETTY_FUNCTION

#define ENDL std::endl << std::flush

namespace cs::log {

struct StringMapStreamView {
  const std::map<std::string, std::string>& value;
};

inline std::ostream& operator<<(
    std::ostream& os, const StringMapStreamView& view) {
  os << "{";
  bool first = true;
  for (const auto& kv : view.value) {
    if (!first) {
      os << ", ";
    } else {
      first = false;
    }
    os << "<key=" << kv.first << ", value=" << kv.second
       << ">";
  }
  os << "}";
  return os;
}

}  // namespace cs::log

#define STREAM_STRING_MAP(map_expr) \
  ::cs::log::StringMapStreamView { map_expr }

#endif  // CS_LOG_HH

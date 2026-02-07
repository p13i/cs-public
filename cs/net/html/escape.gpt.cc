// cs/net/html/escape.gpt.cc
#include "cs/net/html/escape.gpt.hh"

#include <string>

namespace cs::net::html {

std::string EscapeForHtml(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if (c == '&') {
      out += "&amp;";
    } else if (c == '<') {
      out += "&lt;";
    } else if (c == '>') {
      out += "&gt;";
    } else if (c == '"') {
      out += "&quot;";
    } else if (c == '\'') {
      out += "&#39;";
    } else {
      out += c;
    }
  }
  return out;
}

}  // namespace cs::net::html

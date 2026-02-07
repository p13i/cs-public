// cs/net/html/escape.gpt.hh
#ifndef CS_NET_HTML_ESCAPE_GPT_HH
#define CS_NET_HTML_ESCAPE_GPT_HH

#include <string>

namespace cs::net::html {

// Escapes &, <, >, ", ' for safe HTML text content.
std::string EscapeForHtml(const std::string& s);

}  // namespace cs::net::html

#endif  // CS_NET_HTML_ESCAPE_GPT_HH

// cs/net/html/minify.hh
#ifndef CS_NET_HTML_MINIFY_HH
#define CS_NET_HTML_MINIFY_HH

#include <istream>
#include <ostream>
#include <sstream>
#include <string>

namespace cs::net::html::minify {

// MinifyHTML: collapses whitespace, minifies CSS in
// <style>, preserves content of script/style/pre/textarea,
// strips comments.

void MinifyHTML(std::istream& in, std::ostream& out);

std::string MinifyHTML(const std::string& html);

std::stringstream MinifyHTML(std::stringstream in);

}  // namespace cs::net::html::minify

#endif  // CS_NET_HTML_MINIFY_HH

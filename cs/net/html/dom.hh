// cs/net/html/dom.hh
#ifndef CS_NET_HTML_DOM_HH
#define CS_NET_HTML_DOM_HH

#include <map>
#include <string>

namespace cs::net::html::dom {

#define str std::string

#define DOM_MULTICHILD(tag)                \
  template <typename... T>                 \
  std::string tag(T... children) {         \
    return std::string("<" #tag ">") +     \
           (std::string(children) + ...) + \
           std::string("</" #tag ">");     \
  }

#define DOM_ONESTRCHILD(tag)                  \
  inline std::string tag(std::string text) {  \
    return std::string("<" #tag ">") + text + \
           std::string("</" #tag ">");        \
  }

inline str html(str head, str body) {
  return "<!DOCTYPE html><html>" + head + body + "</html>";
}

inline std::string nbsp() { return R"html(&nbsp;)html"; }

inline std::string copy() { return R"html(&copy;)html"; }

DOM_MULTICHILD(head);

DOM_ONESTRCHILD(title);

inline str style(str css) {
  return "<style>" + css + "</style>";
}

inline str script(str js) {
  return "<script type=\"text/javascript\">" + js +
         "</script>";
}

inline str meta(str name, str content) {
  return "<meta name=\"" + name + "\" content=\"" +
         content + "\">";
}

DOM_MULTICHILD(body);
DOM_MULTICHILD(div);
DOM_MULTICHILD(nav);
DOM_MULTICHILD(form);
DOM_MULTICHILD(p);
DOM_ONESTRCHILD(strong);
DOM_MULTICHILD(span);
DOM_ONESTRCHILD(h1);
DOM_ONESTRCHILD(h2);
DOM_ONESTRCHILD(h3);
DOM_ONESTRCHILD(code);

inline std::string a(std::string href, std::string child) {
  return "<a href=\"" + href + "\">" + child + "</a>";
}

inline std::string hr() { return "<hr/>"; }

inline std::string meta(
    std::map<std::string, std::string> attrs) {
  std::string tag = "<meta";
  for (const auto &attr : attrs) {
    tag += " " + attr.first + "=\"" + attr.second + "\"";
  }
  tag += "/>";
  return tag;
}

// Helper function to build attributes string from map
inline std::string BuildAttrs(
    std::map<std::string, std::string> attrs) {
  std::string result;
  for (const auto &attr : attrs) {
    result += " " + attr.first + "=\"" + attr.second + "\"";
  }
  return result;
}

// Self-closing input tag with attributes.
inline std::string input(
    std::map<std::string, std::string> attrs) {
  return std::string("<input") + BuildAttrs(attrs) + "/>";
}

// Convenience helper for checkbox inputs to ensure presence
// of type.
inline std::string checkbox(
    std::map<std::string, std::string> attrs) {
  attrs["type"] = "checkbox";
  return input(attrs);
}

// Macro for tags with attributes and multiple children
#define DOM_MULTICHILD_ATTRS(tag)                      \
  template <typename... T>                             \
  inline std::string tag(                              \
      std::map<std::string, std::string> attrs,        \
      T... children) {                                 \
    return std::string("<" #tag) + BuildAttrs(attrs) + \
           ">" + (std::string(children) + ...) +       \
           std::string("</" #tag ">");                 \
  }

// Macro for tags with attributes and single text child
#define DOM_ONESTRCHILD_ATTRS(tag)                     \
  inline std::string tag(                              \
      std::map<std::string, std::string> attrs,        \
      std::string text) {                              \
    return std::string("<" #tag) + BuildAttrs(attrs) + \
           ">" + text + "</" #tag ">";                 \
  }

// Generate overloaded functions with attributes support
DOM_MULTICHILD_ATTRS(div);
DOM_MULTICHILD_ATTRS(body);
DOM_MULTICHILD_ATTRS(span);
DOM_MULTICHILD_ATTRS(nav);
DOM_ONESTRCHILD_ATTRS(h1);
DOM_ONESTRCHILD_ATTRS(strong);
DOM_ONESTRCHILD_ATTRS(pre);

// Special case for a - takes href and child with optional
// attributes
inline std::string a(
    std::map<std::string, std::string> attrs,
    std::string href, std::string child) {
  return "<a href=\"" + href + "\"" + BuildAttrs(attrs) +
         ">" + child + "</a>";
}

// Special case for p - supports both single text and
// multiple children
inline std::string p(
    std::map<std::string, std::string> attrs,
    std::string text) {
  return std::string("<p") + BuildAttrs(attrs) + ">" +
         text + "</p>";
}
template <typename... T>
inline std::string p(
    std::map<std::string, std::string> attrs,
    T... children) {
  return std::string("<p") + BuildAttrs(attrs) + ">" +
         (std::string(children) + ...) +
         std::string("</p>");
}

// Special case for html - takes head and body
inline std::string html(
    std::map<std::string, std::string> attrs, str head,
    str body) {
  return "<!DOCTYPE html><html" + BuildAttrs(attrs) + ">" +
         head + body + "</html>";
}

#undef str

}  // namespace cs::net::html::dom

#endif  // CS_NET_HTML_DOM_HH

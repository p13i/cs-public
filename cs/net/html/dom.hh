#ifndef CS_NET_HTML_DOM_HH
#define CS_NET_HTML_DOM_HH

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

str html(str head, str body) {
  return "<!DOCTYPE html><html>" + head + body + "</html>";
}

std::string nbsp() { return R"html(&nbsp;)html"; }

std::string copy() { return R"html(&copy;)html"; }

DOM_MULTICHILD(head);

DOM_ONESTRCHILD(title);

str style(str css) { return "<style>" + css + "</style>"; }

str script(str js) {
  return "<script type=\"text/javascript\">" + js +
         "</script>";
}

str meta(str name, str content) {
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

std::string meta(std::map<std::string, std::string> attrs) {
  std::string tag = "<meta";
  for (const auto &attr : attrs) {
    tag += " " + attr.first + "=\"" + attr.second + "\"";
  }
  tag += "/>";
  return tag;
}

#undef str

}  // namespace cs::net::html::dom

#endif  // CS_NET_HTML_DOM_HH

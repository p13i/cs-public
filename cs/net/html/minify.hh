// cs/net/html/minify.hh
#ifndef CS_NET_HTML_MINIFY_HH
#define CS_NET_HTML_MINIFY_HH

#include <algorithm>
#include <cctype>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace cs::net::html::minify {

// ---------- small utils ----------
inline unsigned char uch(char c) {
  return static_cast<unsigned char>(c);
}
inline char to_lower_uc(unsigned char c) {
  return static_cast<char>(std::tolower(c));
}

inline std::string lower(std::string_view s) {
  std::string out;
  out.reserve(s.size());
  for (unsigned char c : s) out.push_back(to_lower_uc(c));
  return out;
}

inline bool istarts_with(std::string_view s, size_t pos,
                         std::string_view lit) {
  if (pos + lit.size() > s.size()) return false;
  for (size_t i = 0; i < lit.size(); ++i) {
    if (to_lower_uc((unsigned char)s[pos + i]) !=
        to_lower_uc((unsigned char)lit[i]))
      return false;
  }
  return true;
}

namespace detail {

// ---------- CSS minifier (safe, conservative) ----------
inline std::string MinifyCSS(std::string_view css) {
  const size_t n = css.size();
  std::string out;
  out.reserve(n);

  auto is_ws = [](char c) {
    return std::isspace(uch(c)) != 0;
  };

  bool in_string = false;
  char q = 0;
  bool esc = false;
  bool in_comment = false;
  bool had_ws = false;

  std::vector<std::string> fn_stack;
  std::string ident;

  auto push_fn_if_call = [&](size_t i) {
    size_t k = i;
    while (k < n && is_ws(css[k])) ++k;
    if (k < n && css[k] == '(') {
      std::string idl;
      idl.reserve(ident.size());
      for (char c : ident)
        idl.push_back((char)std::tolower(uch(c)));
      fn_stack.push_back(std::move(idl));
    }
  };

  auto skip_ws = [&](size_t& i) {
    while (i < n && is_ws(css[i])) ++i;
  };

  auto in_calc = [&]() -> bool {
    return !fn_stack.empty() && fn_stack.back() == "calc";
  };

  auto emit_trim_before = [&](char punct) {
    if (!out.empty() && out.back() == ' ') out.pop_back();
    out.push_back(punct);
  };

  for (size_t i = 0; i < n;) {
    char c = css[i];

    if (!in_string && !in_comment && c == '/' &&
        i + 1 < n && css[i + 1] == '*') {
      in_comment = true;
      i += 2;
      continue;
    }
    if (in_comment) {
      if (c == '*' && i + 1 < n && css[i + 1] == '/') {
        in_comment = false;
        i += 2;
      } else {
        ++i;
      }
      continue;
    }

    if (!in_string && (c == '"' || c == '\'')) {
      if (had_ws) {
        if (!out.empty()) {
          char p = out.back();
          if (std::isalnum(uch(p)) || p == ')' ||
              p == ']' || p == '-' || p == '_' || p == '*')
            out.push_back(' ');
        }
        had_ws = false;
      }
      in_string = true;
      q = c;
      esc = false;
      out.push_back(c);
      ++i;
      continue;
    }
    if (in_string) {
      out.push_back(c);
      if (esc) {
        esc = false;
        ++i;
        continue;
      }
      if (c == '\\') {
        esc = true;
        ++i;
        continue;
      }
      if (c == q) {
        in_string = false;
        q = 0;
      }
      ++i;
      continue;
    }

    if (std::isspace(uch(c))) {
      had_ws = true;
      ++i;
      continue;
    }

    if (std::isalpha(uch(c)) || c == '_' || c == '-') {
      if (had_ws) {
        if (!out.empty()) {
          char p = out.back();
          if (std::isalnum(uch(p)) || p == ')' ||
              p == ']' || p == '*' || p == '-' ||
              p == '_' || p == '.' || p == '#' || p == ':')
            out.push_back(' ');
        }
        had_ws = false;
      }
      ident.clear();
      size_t j = i;
      while (j < n && (std::isalnum(uch(css[j])) ||
                       css[j] == '-' || css[j] == '_'))
        ++j;
      ident.assign(css.substr(i, j - i));
      out.append(ident);
      push_fn_if_call(j);
      i = j;
      continue;
    }

    if (std::isdigit(uch(c)) ||
        (c == '.' && i + 1 < n &&
         std::isdigit(uch(css[i + 1])))) {
      if (had_ws) {
        if (!out.empty()) {
          char p = out.back();
          if (std::isalnum(uch(p)) || p == ')' ||
              p == ']' || p == '_' || p == '-' || p == '*')
            out.push_back(' ');
        }
        had_ws = false;
      }
      size_t j = i;
      if (css[j] == '+' || css[j] == '-') ++j;
      while (j < n &&
             (std::isdigit(uch(css[j])) || css[j] == '.'))
        ++j;
      while (j < n &&
             (std::isalpha(uch(css[j])) || css[j] == '%'))
        ++j;
      out.append(css.substr(i, j - i));
      i = j;
      continue;
    }

    if (c == '(') {
      emit_trim_before('(');
      ++i;
      had_ws = false;
      continue;
    }
    if (c == ')') {
      out.push_back(')');
      if (!fn_stack.empty()) fn_stack.pop_back();
      ++i;
      had_ws = false;
      continue;
    }

    if (c == ':' || c == ',') {
      emit_trim_before(c);
      ++i;
      skip_ws(i);
      continue;
    }
    if (c == ';') {
      out.push_back(';');
      ++i;
      skip_ws(i);
      continue;
    }
    if (c == '{') {
      emit_trim_before('{');
      ++i;
      skip_ws(i);
      continue;
    }
    if (c == '}') {
      if (!out.empty() && out.back() == ';') out.pop_back();
      out.push_back('}');
      ++i;
      skip_ws(i);
      continue;
    }

    if (c == '+' || c == '-') {
      if (in_calc()) {
        if (!out.empty() && out.back() != ' ' &&
            out.back() != '(')
          out.push_back(' ');
        out.push_back(c);
        ++i;
        skip_ws(i);
        if (i < n && css[i] != ')') out.push_back(' ');
      } else {
        if (had_ws && !out.empty() && out.back() != ' ')
          out.push_back(' ');
        had_ws = false;
        out.push_back(c);
        ++i;
      }
      continue;
    }

    if (c == '>' || c == '~' || c == '=' || c == '|' ||
        c == '^' || c == '$' || c == '*' || c == '#' ||
        c == '.' || c == '[' || c == ']' || c == '!' ||
        c == '@') {
      if (c != '[' && c != '!' && c != '@') {
        if (!out.empty() && out.back() == ' ')
          out.pop_back();
      }
      out.push_back(c);
      ++i;
      if (c != '!' && c != '@' && c != '[' && c != '#' &&
          c != '.') {
        skip_ws(i);
      }
      had_ws = false;
      continue;
    }

    if (had_ws) {
      if (!out.empty()) {
        char p = out.back();
        if (std::isalnum(uch(p)) || p == ')' || p == ']' ||
            p == '*' || p == '-' || p == '_' || p == '.' ||
            p == '#' || p == ':')
          out.push_back(' ');
      }
      had_ws = false;
    }
    out.push_back(c);
    ++i;
  }

  if (!out.empty() && out.back() == ' ') out.pop_back();
  return out;
}

// ---------- HTML core (internal) ----------
inline std::string MinifyHTMLImpl(std::string_view in) {
  const size_t n = in.size();
  std::string out;
  out.reserve(n);

  auto is_space = [](char c) {
    return std::isspace(uch(c)) != 0;
  };
  auto is_raw_tag = [](std::string_view tag_lower) {
    return tag_lower == "script" || tag_lower == "style" ||
           tag_lower == "pre" || tag_lower == "textarea";
  };

  size_t i = 0;
  bool last_space_emitted = false;
  std::string current_raw_tag;  // lowercase when active
  std::string style_buf;

  while (i < n) {
    char c = in[i];

    if (!current_raw_tag.empty()) {
      if (current_raw_tag == "style") {
        if (c == '<' && i + 2 < n && in[i + 1] == '/') {
          size_t j = i + 2;
          while (j < n && is_space(in[j])) ++j;
          size_t name_start = j;
          while (j < n && (std::isalnum(uch(in[j])) ||
                           in[j] == '-' || in[j] == ':'))
            ++j;
          std::string tag_name = lower(std::string_view(
              in.substr(name_start, j - name_start)));
          if (tag_name == "style") {
            out.append(detail::MinifyCSS(style_buf));
            style_buf.clear();
            size_t k = j;
            while (k < n && in[k] != '>') ++k;
            if (k < n) ++k;
            out.append(in.substr(i, k - i));
            i = k;
            current_raw_tag.clear();
            last_space_emitted = true;
            continue;
          }
        }
        style_buf.push_back(c);
        ++i;
        continue;
      }

      if (c == '<' && i + 2 < n && in[i + 1] == '/') {
        size_t j = i + 2;
        while (j < n && is_space(in[j])) ++j;
        size_t name_start = j;
        while (j < n && (std::isalnum(uch(in[j])) ||
                         in[j] == '-' || in[j] == ':'))
          ++j;
        std::string tag_name = lower(std::string_view(
            in.substr(name_start, j - name_start)));
        if (tag_name == current_raw_tag) {
          size_t k = j;
          while (k < n && in[k] != '>') ++k;
          if (k < n) ++k;
          out.append(in.substr(i, k - i));
          i = k;
          current_raw_tag.clear();
          last_space_emitted = true;
          continue;
        }
      }
      out.push_back(c);
      ++i;
      continue;
    }

    if (c != '<') {
      if (is_space(c)) {
        if (!last_space_emitted) {
          out.push_back(' ');
          last_space_emitted = true;
        }
        ++i;
      } else {
        out.push_back(c);
        last_space_emitted = false;
        ++i;
      }
      continue;
    }

    if (!out.empty() && out.back() == ' ') {
      out.pop_back();
      last_space_emitted = false;
    }

    if (i + 3 < n && in[i + 1] == '!' && in[i + 2] == '-' &&
        in[i + 3] == '-') {
      bool preserve = (i + 7 < n && in[i + 4] == '[' &&
                       istarts_with(in, i + 4, "[if"));
      size_t end = std::string(in).find("-->", i + 4);
      if (end == std::string::npos) return out;
      if (preserve) out.append(in.substr(i, end + 3 - i));
      i = end + 3;
      last_space_emitted = true;
      continue;
    }

    if (i + 1 < n && in[i + 1] == '?') {
      size_t end = std::string(in).find("?>", i + 2);
      if (end == std::string::npos) {
        out.append(in.substr(i));
        return out;
      }
      out.append(in.substr(i, end + 2 - i));
      i = end + 2;
      last_space_emitted = true;
      continue;
    }

    size_t j = i + 1;
    bool closing = false, self_closing = false;
    if (j < n && in[j] == '/') {
      closing = true;
      ++j;
    }

    size_t name_start = j;
    while (j < n && (std::isalnum(uch(in[j])) ||
                     in[j] == '-' || in[j] == ':'))
      ++j;
    std::string_view tag_name_sv =
        in.substr(name_start, j - name_start);
    std::string tag_name_lower = lower(tag_name_sv);

    out.push_back('<');
    if (closing) out.push_back('/');
    out.append(tag_name_sv.begin(), tag_name_sv.end());

    bool in_quote = false;
    char quote = 0;
    bool pending_space = false;

    while (j < n && in[j] != '>') {
      char d = in[j];
      if (in_quote) {
        out.push_back(d);
        if (d == quote) {
          in_quote = false;
          quote = 0;
        }
        ++j;
        continue;
      }
      if (d == '"' || d == '\'') {
        in_quote = true;
        quote = d;
        out.push_back(d);
        ++j;
        continue;
      }
      if ((unsigned char)d <= 0x20) {
        pending_space = true;
        ++j;
        continue;
      }
      if (d == '=') {
        if (!out.empty() && out.back() == ' ')
          out.pop_back();
        out.push_back('=');
        ++j;
        while (j < n && (unsigned char)in[j] <= 0x20) ++j;
        pending_space = false;
        continue;
      }
      if (d == '/' && j + 1 < n && in[j + 1] == '>') {
        if (!out.empty() && out.back() == ' ')
          out.pop_back();
        out.append("/>");
        j += 2;
        self_closing = true;
        break;
      }
      if (pending_space) {
        if (!out.empty() && out.back() != '<' &&
            out.back() != '/')
          out.push_back(' ');
        pending_space = false;
      }
      out.push_back(d);
      ++j;
    }

    if (j < n && in[j] == '>') {
      if (!out.empty() && out.back() == ' ') out.pop_back();
      out.push_back('>');
      ++j;
    }

    if (!closing && !self_closing &&
        is_raw_tag(tag_name_lower)) {
      current_raw_tag = tag_name_lower;
      if (current_raw_tag == "style") style_buf.clear();
    }

    i = j;
    last_space_emitted = true;
  }

  if (!out.empty() && out.back() == ' ') out.pop_back();
  return out;
}

}  // namespace detail

// ---------- Public API: all named MinifyHTML ----------

// istream -> ostream
inline void MinifyHTML(std::istream& in,
                       std::ostream& out) {
  std::ostringstream buf;
  buf << in.rdbuf();
  std::string minified = detail::MinifyHTMLImpl(buf.str());
  out.write(minified.data(),
            static_cast<std::streamsize>(minified.size()));
}

// std::string -> std::string
inline std::string MinifyHTML(const std::string& html) {
  return detail::MinifyHTMLImpl(html);
}

// std::stringstream -> std::stringstream
inline std::stringstream MinifyHTML(std::stringstream in) {
  std::ostringstream out;
  MinifyHTML(in, out);
  std::stringstream result;
  result.str(out.str());
  result.clear();
  return result;
}

}  // namespace cs::net::html::minify

#endif  // CS_NET_HTML_MINIFY_HH

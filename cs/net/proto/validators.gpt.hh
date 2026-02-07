// cs/net/proto/validators.gpt.hh
// cs/net/proto/validators.gpt.hh
// cs/net/proto/validators.hh
#ifndef CS_NET_PROTO_VALIDATORS_GPT_HH
#define CS_NET_PROTO_VALIDATORS_GPT_HH

#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "cs/result.hh"

namespace cs::net::proto::validation {

namespace detail {

struct Accum {
  bool ok = true;
  std::vector<std::string> parts;
};

inline void add_error(Accum& out, std::string path,
                      std::string, std::string msg) {
  out.ok = false;
  out.parts.push_back(std::move(path) + ": " +
                      std::move(msg));
}

}  // namespace detail

struct noop {
  template <class Msg>
  constexpr void operator()(const Msg&,
                            detail::Accum&) const {}
};

template <auto MemberPtr, const char* Name>
struct field {
  static constexpr auto ptr = MemberPtr;
  static constexpr const char* name = Name;
};

template <class Field>
struct required {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v == decltype(v){}) {
      detail::add_error(out, Field::name, "ERR_REQUIRED",
                        "value is required");
    }
  }
};

template <class Field>
struct email {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    auto at = v.find('@');
    if (at == std::string::npos || at == 0 ||
        at + 1 >= v.size()) {
      detail::add_error(out, Field::name, "ERR_EMAIL",
                        "invalid email");
    }
  }
};

template <class Field, auto N>
struct gt {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (!(v > N)) {
      detail::add_error(out, Field::name, "ERR_GT",
                        "must be greater");
    }
  }
};

template <class Field, auto N>
struct ge {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (!(v >= N)) {
      detail::add_error(out, Field::name, "ERR_GE",
                        "must be >= threshold");
    }
  }
};

template <class Field, auto N>
struct lt {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (!(v < N)) {
      detail::add_error(out, Field::name, "ERR_LT",
                        "must be less");
    }
  }
};

template <class Field, auto N>
struct le {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (!(v <= N)) {
      detail::add_error(out, Field::name, "ERR_LE",
                        "must be <= threshold");
    }
  }
};

template <class Field, auto N>
struct len_gt {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v.size() <= static_cast<size_t>(N)) {
      detail::add_error(out, Field::name, "ERR_LEN_GT",
                        "size too small");
    }
  }
};

template <class Field, auto N>
struct len_lt {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v.size() >= static_cast<size_t>(N)) {
      detail::add_error(out, Field::name, "ERR_LEN_LT",
                        "size too large");
    }
  }
};

template <class Field, auto... Allowed>
struct enum_in {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    bool ok = ((v == Allowed) || ...);
    if (!ok) {
      detail::add_error(out, Field::name, "ERR_ENUM_IN",
                        "value not allowed");
    }
  }
};

template <class Field, const char* Pattern>
struct matches {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    static const std::regex re(Pattern);
    if (!std::regex_match(v, re)) {
      detail::add_error(out, Field::name, "ERR_MATCHES",
                        "does not match pattern");
    }
  }
};

template <class Field>
struct oneof_set {
  template <class Msg>
  constexpr void operator()(const Msg&,
                            detail::Accum&) const {
    // Placeholder: depends on oneof representation; treat
    // as no-op.
  }
};

template <class T>
inline bool custom_validate(const char*, const T&,
                            std::string_view,
                            detail::Accum&) {
  return true;
}

template <class Field>
struct iso8601 {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    static const std::regex re(
        R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z$)");
    if (!std::regex_match(v, re)) {
      detail::add_error(out, Field::name, "ERR_ISO8601",
                        "invalid ISO8601 timestamp");
    }
  }
};

template <class Field>
struct uuid {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    static const std::regex re(
        R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)");
    if (!std::regex_match(v, re)) {
      detail::add_error(out, Field::name, "ERR_UUID",
                        "invalid UUID format");
    }
  }
};

template <class Field>
struct base64 {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    static const std::regex re(R"(^[A-Za-z0-9+/]*={0,2}$)");
    if (!std::regex_match(v, re)) {
      detail::add_error(out, Field::name, "ERR_BASE64",
                        "invalid base64 format");
    }
  }
};

template <class Field>
struct filename {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v.find('/') != std::string::npos ||
        v.find('\\') != std::string::npos ||
        v.find('\0') != std::string::npos) {
      detail::add_error(out, Field::name, "ERR_FILENAME",
                        "filename must not contain path "
                        "separators or null");
      return;
    }
    if (v == "." || v == "..") {
      detail::add_error(out, Field::name, "ERR_FILENAME",
                        "filename must not be . or ..");
    }
  }
};

template <class Field>
struct host {
  template <class Msg>
  void operator()(const Msg& msg,
                  detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v.empty()) {
      detail::add_error(out, Field::name, "ERR_HOST",
                        "host must be non-empty");
      return;
    }
    if (v.size() > 253) {
      detail::add_error(out, Field::name, "ERR_HOST",
                        "host too long");
      return;
    }
    static const std::regex re(
        R"(^[a-zA-Z0-9]([a-zA-Z0-9.-]*[a-zA-Z0-9])?$|^\[[0-9a-fA-F:.]+\]$)");
    if (!std::regex_match(v, re)) {
      detail::add_error(out, Field::name, "ERR_HOST",
                        "invalid host format");
    }
  }
};

template <class Field>
struct port {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    const auto& v = msg.*Field::ptr;
    if (v < 1 || v > 65535) {
      detail::add_error(out, Field::name, "ERR_PORT",
                        "port must be 1-65535");
    }
  }
};

template <class Field, const char* Token>
struct custom {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    custom_validate(Token, msg.*Field::ptr, Field::name,
                    out);
  }
};

template <class... Rules>
struct all {
  template <class Msg>
  constexpr void operator()(const Msg& msg,
                            detail::Accum& out) const {
    (Rules{}(msg, out), ...);
  }
};

template <class Msg, class Rule>
cs::Result Validate(const Msg& msg, Rule rule) {
  detail::Accum out{};
  rule(msg, out);
  if (out.ok) {
    return cs::Ok();
  }
  std::string message;
  for (size_t i = 0; i < out.parts.size(); ++i) {
    if (i > 0) {
      message += "; ";
    }
    message += out.parts[i];
  }
  return cs::ValidationError(message);
}

}  // namespace cs::net::proto::validation

#endif  // CS_NET_PROTO_VALIDATORS_GPT_HH

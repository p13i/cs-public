// cs/result.hh
#ifndef CS_RESULT_HH
#define CS_RESULT_HH

#include <stdio.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cs/log.hh"
#include "cs/util/fmt.hh"

#define TRACE(result)                                   \
  result.trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, \
               #result)

// Written by ChatGPT
// [https://chatgpt.com/share/67594ada-c448-800e-b9e0-a6900c8c2596]
#define ASSIGN_OR_RETURN(lhs, rhs)                   \
  auto UNIQUE_VAR(_result) = (rhs);                  \
  if (!UNIQUE_VAR(_result).ok()) {                   \
    LOG(ERROR) << "ASSIGN_OR_RETURN failed at\n["    \
               << __FILE__ << ":" << __LINE__ << " " \
               << __PRETTY_FUNCTION__ << "]\n"       \
               << #rhs                               \
               << "\nresult=" << UNIQUE_VAR(_result) \
               << "\n"                               \
               << ENDL;                              \
    return TRACE(UNIQUE_VAR(_result));               \
  }                                                  \
  lhs = UNIQUE_VAR(_result).value();

#define SET_OR_RET(lhs, rhs) ASSIGN_OR_RETURN(lhs, rhs)

#define OK_OR_RETURN(rhs)                                 \
  auto UNIQUE_VAR(_result) = (rhs);                       \
  if (!UNIQUE_VAR(_result).ok()) {                        \
    LOG(ERROR) << "OK_OR_RETURN failed at\n[" << __FILE__ \
               << ":" << __LINE__ << " "                  \
               << __PRETTY_FUNCTION__ << "]\n"            \
               << #rhs                                    \
               << "\nresult=" << UNIQUE_VAR(_result)      \
               << "\n"                                    \
               << ENDL;                                   \
    return TRACE(UNIQUE_VAR(_result));                    \
  }

#define OK_OR_RET(rhs) OK_OR_RETURN(rhs)

#define ASSERT_OK_AND_ASSIGN(lhs, rhs)  \
  auto UNIQUE_VAR(_result) = (rhs);     \
  ASSERT_THAT(UNIQUE_VAR(_result).ok(), \
              ::testing::IsTrue())      \
      << UNIQUE_VAR(_result).message(); \
  lhs = UNIQUE_VAR(_result).value();

// Helper macro to generate a unique variable name
#define UNIQUE_VAR(name) CONCATENATE(name, __LINE__)
#define CONCATENATE(x, y) CONCATENATE_IMPL(x, y)
#define CONCATENATE_IMPL(x, y) x##y

#define ASSERT_OK(arg)                  \
  auto UNIQUE_VAR(_result) = (arg);     \
  ASSERT_THAT(UNIQUE_VAR(_result).ok(), \
              ::testing::IsTrue());

#define ASSERT_OK_EXPECT_MATCHING(arg, matcher)    \
  auto UNIQUE_VAR(_result) = (arg);                \
  ASSERT_THAT(UNIQUE_VAR(_result).ok(), IsTrue()); \
  EXPECT_THAT(UNIQUE_VAR(_result).value(), matcher);

namespace cs {

static constexpr int kNotFound = 404;

class Result {
 public:
  static bool IsNotFound(const Result& result) {
    return result.code() == kNotFound;
  };

  Result(bool ok, std::string message)
      : Result(ok, message, ok ? 0 : 1) {}

  Result(bool ok, std::string message, int code)
      : _ok(ok), _code(code), _message(message) {}

  Result& trace(const char path[], unsigned int line,
                const char func[] = "",
                const char literal[] = "") {
    this->_traces.push_back(
        FMT("[%s:%d] [%s] %s", path, line, func, literal));
    return *this;
  }

  bool ok() const { return _ok; }

  std::string message() const { return _message; }

  int code() const { return _code; }

  const std::vector<std::string>& traces() const {
    return _traces;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const Result& result) {
    os << "Result(" << (result.ok() ? "OK" : "ERROR")
       << ", code=" << result.code()
       << ", message=" << result.message() << ")";
    if (!result.traces().empty()) {
      os << "\n<traces>\n";
      for (auto trace : result.traces()) {
        os << trace << "\n";
      }
      os << "</traces>";
    }
    return os;
  }

  std::string str() {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  static int Main(
      int argc, char** argv,
      std::function<cs::Result(std::vector<std::string>)>
          impl) {
    std::vector<std::string> args(argv, argv + argc);
    const Result result = impl(args);
    if (result.ok()) {
      LOG(INFO) << "Main impl succeeded with result: "
                << result << ENDL;
    } else {
      LOG(ERROR) << "Main impl failed with result: "
                 << result << ENDL;
    }
    return result.code();
  }

 private:
  bool _ok;
  int _code;
  std::string _message;
  std::vector<std::string> _traces;
};

class Ok : public Result {
 public:
  Ok() : Ok("") {}
  Ok(std::string message) : Result(true, message) {}
};

class Error : public Result {
 public:
  Error(const std::string& message) : Error(message, -1) {}
  Error(const std::string& message, int code)
      : Result(false, message, code) {}
};

class ValidationError : public Error {
 public:
  ValidationError(const std::string& message)
      : Error("Validation error: " + message) {}
};

class NotFoundError : public Error {
 public:
  NotFoundError(const std::string& message)
      : Error("Not found: " + message, kNotFound) {}
};

class InvalidArgument : public Error {
 public:
  InvalidArgument(const std::string& message)
      : Error("Invalid argument: " + message) {}
};

class PermissionDenied : public Error {
 public:
  PermissionDenied(const std::string& message)
      : Error("Permission denied: " + message) {}
};

// ResultOr class with a template parameter for data
template <typename Data>
class ResultOr : public Result {
 public:
  ResultOr(const Data& data)
      : Result(Ok()), _result(Ok()), _data(data) {}

  ResultOr(const Error& result)
      : Result(result), _result(result) {}

  ResultOr(const Result& result)
      : Result(result), _result(result) {}

  ResultOr(const Result& result, const Data& data)
      : Result(result), _result(result), _data(data) {}

  bool ok() const { return _result.ok(); }
  Data data() const { return _data; }
  Data value() const { return _data; }
  Result result() const { return _result; }
  Data value_or(Data default_value) const {
    if (ok()) {
      return _data;
    }
    LOG(WARNING) << "Returning default value in value_or."
                 << ENDL;
    return default_value;
  }

  template <typename Transform>
  auto then(Transform&& transform) const
      -> decltype(transform(std::declval<Data>())) {
    if (!this->ok()) {
      return *this;
    }
    return transform(this->value());
  }

  Data operator->() { return _data; }

  friend std::ostream& operator<<(
      std::ostream& os, const ResultOr<Data>& result) {
    os << "ResultOr(";
    if (result.ok()) {
      os << "OK";
    } else {
      os << "ERROR, code=" << result.code()
         << ", message=" << result.message();
    }
    os << ")";
    if (!result.traces().empty()) {
      os << "\n<traces>\n";
      for (auto trace : result.traces()) {
        os << trace << "\n";
      }
      os << "</traces>";
    }
    return os;
  }

 private:
  Result _result;
  Data _data;
};

}  // namespace cs

#endif  // CS_RESULT_HH

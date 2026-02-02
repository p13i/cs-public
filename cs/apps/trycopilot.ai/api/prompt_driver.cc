// cs/apps/trycopilot.ai/api/prompt_driver.cc
#include "cs/apps/trycopilot.ai/api/prompt_driver.hh"

#include <array>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "cs/log.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::ResultOr;
using ::cs::apps::trycopilotai::protos::PromptDriverRequest;
}  // namespace

namespace {  // impl
// Minimal shell escaping for single-quoted arguments.
std::string EscapeForSingleQuotes(const std::string& s) {
  std::string out;
  out.reserve(s.size() + 2);
  for (char c : s) {
    if (c == '\'') {
      out += "'\"'\"'";
    } else {
      out.push_back(c);
    }
  }
  return out;
}

ResultOr<std::pair<int, std::string>> RunCommand(
    const std::string& cmd) {
  LOG(DEBUG) << "Running command: " << cmd << ENDL;
  std::array<char, 256> buffer{};
  std::string output;
  std::unique_ptr<FILE, int (*)(FILE*)> pipe(
      popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    return Error("Failed to open pipe for command.");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get())) {
    output.append(buffer.data());
  }
  int rc = pclose(pipe.release());
  return std::make_pair(rc, output);
}

}  // namespace

namespace cs::apps::trycopilotai::api {

using ::Error;
using ::InvalidArgument;
using ::PromptDriverRequest;
using ::ResultOr;
using ::cs::apps::trycopilotai::protos::
    PromptDriverResponse;

IMPLEMENT_RPC(PromptDriverRPC, PromptDriverRequest,
              PromptDriverResponse) {
  if (request.msg.empty()) {
    return TRACE(InvalidArgument("msg must not be empty"));
  }

  std::string escaped = EscapeForSingleQuotes(request.msg);
  std::stringstream cmd;
  // Reuse existing Makefile target to hit trycopilot.ai
  // prompt.
  cmd << "MSG='" << escaped << "' make prompt 2>&1";

  auto run = RunCommand(cmd.str());
  PromptDriverResponse resp;
  if (!run.ok()) {
    resp.ok = false;
    resp.exit_code = -1;
    resp.error_message = run.message();
    resp.stdout_output = "";
    return resp;
  }

  resp.exit_code = run.value().first;
  resp.stdout_output = run.value().second;
  resp.ok = (resp.exit_code == 0);
  resp.error_message = resp.ok ? "" : "Non-zero exit code.";
  return resp;
}

}  // namespace cs::apps::trycopilotai::api

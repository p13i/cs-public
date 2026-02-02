// cs/apps/trycopilot.ai/api/prompt.cc
#include "cs/apps/trycopilot.ai/api/prompt.hh"

#include <cstdlib>
#include <map>
#include <optional>
#include <sstream>
#include <string>

#include "cs/ai/gpt/gpt_core.hh"
#include "cs/log.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"
#include "cs/util/context.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::FetchResponse;
using ::cs::InvalidArgument;
using ::cs::ResultOr;
using ::cs::apps::trycopilotai::protos::PromptRequest;
using ::cs::apps::trycopilotai::protos::PromptResponse;
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::Request;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseObject;
using ::cs::util::Context;
}  // namespace

namespace {  // impl
std::optional<std::string> ReadEnvOrContext(
    const std::string& key) {
  const char* env = std::getenv(key.c_str());
  if (env != nullptr && env[0] != '\0') {
    return std::string(env);
  }
  auto ctx = Context::Read(key);
  if (ctx.ok()) {
    return ctx.value();
  }
  return std::nullopt;
}

std::string ResolveChatApiUrl() {
  if (auto url = ReadEnvOrContext("OPENAI_API_URL")) {
    return *url;
  }
  if (auto url = ReadEnvOrContext("CHATGPT_API_URL")) {
    return *url;
  }
  if (auto base = ReadEnvOrContext("OPENAI_API_BASE")) {
    std::string trimmed = *base;
    while (!trimmed.empty() && trimmed.back() == '/') {
      trimmed.pop_back();
    }
    return trimmed + "/v1/chat/completions";
  }
  return "https://api.openai.com/v1/chat/completions";
}

std::string ResolveChatModel() {
  if (auto model = ReadEnvOrContext("OPENAI_MODEL")) {
    return *model;
  }
  if (auto model = ReadEnvOrContext("CHATGPT_MODEL")) {
    return *model;
  }
  return "gpt-4o-mini";
}

ResultOr<std::string> ResolveChatApiKey() {
  if (auto key = ReadEnvOrContext("OPENAI_API_KEY")) {
    return *key;
  }
  if (auto key = ReadEnvOrContext("CHATGPT_API_KEY")) {
    return *key;
  }
  return TRACE(
      Error("Missing OPENAI_API_KEY or CHATGPT_API_KEY for "
            "/prompt."));
}

ResultOr<std::string> ExtractChatMessage(
    const std::string& body) {
  SET_OR_RET(Object root, ParseObject(body));
  SET_OR_RET(Object choices_obj, root.get("choices"));
  auto choices = choices_obj.as(std::vector<Object>{});
  if (choices.empty()) {
    return TRACE(
        Error("Chat API response missing choices."));
  }

  const Object& first_choice = choices.front();
  auto message_or = first_choice.get("message");
  if (message_or.ok()) {
    auto content_or = message_or.value().get("content");
    if (content_or.ok()) {
      std::string content =
          content_or.value().as(std::string{});
      if (!content.empty()) {
        return content;
      }
    }
  }

  auto text_or = first_choice.get("text");
  if (text_or.ok()) {
    std::string text = text_or.value().as(std::string{});
    if (!text.empty()) {
      return text;
    }
  }

  return TRACE(
      Error("Chat API response did not include message "
            "content."));
}

std::string Truncate(const std::string& s, size_t max_len) {
  if (s.size() <= max_len) {
    return s;
  }
  return s.substr(0, max_len) + "...";
}

ResultOr<std::string> ExtractErrorMessage(
    const std::string& body) {
  SET_OR_RET(auto root, ParseObject(body));
  auto error_or = root.get("error");
  if (!error_or.ok()) {
    return TRACE(Error("No error field present."));
  }
  auto message_or = error_or.value().get("message");
  if (!message_or.ok()) {
    return TRACE(
        Error("No error.message present in response."));
  }
  return message_or.value().as(std::string{});
}

ResultOr<std::string> CallChatApi(
    const std::string& prompt, std::string* raw_body_out) {
  SET_OR_RET(std::string api_key, ResolveChatApiKey());
  const std::string api_url = ResolveChatApiUrl();
  const std::string model = ResolveChatModel();

  using ::Object;
  Object payload = Object::NewMap(
      {{"model", Object::NewString(model)},
       {"messages",
        Object::NewArray({Object::NewMap(
            {{"role", Object::NewString("user")},
             {"content", Object::NewString(prompt)}})})}});

  std::map<std::string, std::string> headers{
      {"Authorization", "Bearer " + api_key},
      {"Content-Type", kContentTypeApplicationJson}};

  SET_OR_RET(auto response,
             FetchResponse(api_url, "POST", headers,
                           payload.str()));
  if (raw_body_out != nullptr) {
    *raw_body_out = response.body();
  }

  if (response.status().code < 200 ||
      response.status().code >= 300) {
    std::stringstream ss;
    ss << "Chat API HTTP " << response.status().code << " "
       << response.status().name;
    auto parsed_error =
        ExtractErrorMessage(response.body());
    if (parsed_error.ok()) {
      ss << ": " << parsed_error.value();
    } else if (!response.body().empty()) {
      ss << ": " << Truncate(response.body(), 200);
    }
    return TRACE(Error(ss.str()));
  }

  return ExtractChatMessage(response.body());
}

void SetErrorMessage(PromptResponse* response,
                     const std::string& message) {
  if (response->error_message.empty()) {
    response->error_message = message;
    return;
  }
  response->error_message =
      response->error_message + " | " + message;
}
}  // namespace

namespace cs::apps::trycopilotai::api {
using ::Context;
using ::InvalidArgument;
using ::PromptRequest;
using ::PromptResponse;
using ::Request;

IMPLEMENT_RPC(PromptRPC, PromptRequest, PromptResponse) {
  PromptResponse response{};
  Request helper;
  response.msg = helper.UrlDecode(request.msg);
  response.exit_code = -1;
  response.ok = false;

  if (response.msg.empty()) {
    return TRACE(InvalidArgument(
        "msg query parameter is required."));
  }

  const std::string user_prompt = response.msg;

  std::string chat_raw_response;
  auto chat_completion =
      CallChatApi(user_prompt, &chat_raw_response);
  if (chat_completion.ok()) {
    response.msg = chat_completion.value();
    response.codex_msg = response.msg;
    response.stdout_output = chat_raw_response;
    response.exit_code = 0;
    response.ok = true;
    return response;
  }
  SetErrorMessage(&response, chat_completion.message());
  // ChatGPT call failed; surface the error.
  response.exit_code = 1;
  response.ok = false;
  response.codex_msg.clear();
  response.stdout_output = chat_raw_response;
  return response;
}

IMPLEMENT_RPC(LocalPromptRPC, PromptRequest,
              PromptResponse) {
  PromptResponse response{};
  Request helper;
  response.msg = helper.UrlDecode(request.msg);
  response.exit_code = -1;
  response.ok = false;

  if (response.msg.empty()) {
    return TRACE(InvalidArgument(
        "msg query parameter is required."));
  }

  if (!EnsureModelReady()) {
    response.error_message = "Local GPT model unavailable.";
    return response;
  }

  response.msg = GptEvaluate(response.msg);
  response.codex_msg = response.msg;
  response.stdout_output = "";
  response.exit_code = 0;
  response.ok = true;
  return response;
}

}  // namespace cs::apps::trycopilotai::api

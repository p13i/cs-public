// cs/apps/trycopilot.ai/api/prompt_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/prompt.hh"

#include "cs/apps/trycopilot.ai/protos/prompt.proto.hh"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::PromptRPC;
using ::cs::apps::trycopilotai::protos::PromptRequest;
using ::cs::apps::trycopilotai::protos::PromptResponse;
}  // namespace

namespace {

TEST(PromptRPCTest, EmptyMsgFails) {
  PromptRequest req;
  req.msg = "";
  PromptRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

}  // namespace

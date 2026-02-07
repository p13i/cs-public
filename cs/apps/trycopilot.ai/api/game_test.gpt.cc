// cs/apps/trycopilot.ai/api/game_test.gpt.cc
#include "cs/apps/trycopilot.ai/api/game.hh"

#include "cs/apps/trycopilot.ai/protos/game.proto.hh"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::trycopilotai::api::GetAnimationRPC;
using ::cs::apps::trycopilotai::protos::GetAnimationRequest;
using ::cs::apps::trycopilotai::protos::
    GetAnimationResponse;
}  // namespace

namespace {

TEST(GetAnimationRPCTest, InvalidWidthZero) {
  GetAnimationRequest req;
  req.width = 0;
  req.height = 64;
  req.num_frames = 1;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, InvalidWidthTooLarge) {
  GetAnimationRequest req;
  req.width = 2048;
  req.height = 64;
  req.num_frames = 1;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, InvalidHeightZero) {
  GetAnimationRequest req;
  req.width = 64;
  req.height = 0;
  req.num_frames = 1;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, InvalidHeightTooLarge) {
  GetAnimationRequest req;
  req.width = 64;
  req.height = 2048;
  req.num_frames = 1;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, InvalidNumFramesZero) {
  GetAnimationRequest req;
  req.width = 64;
  req.height = 64;
  req.num_frames = 0;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, InvalidNumFramesTooMany) {
  GetAnimationRequest req;
  req.width = 64;
  req.height = 64;
  req.num_frames = 100;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  EXPECT_FALSE(result.ok());
}

TEST(GetAnimationRPCTest, SmallValidRequest) {
  GetAnimationRequest req;
  req.width = 4;
  req.height = 4;
  req.num_frames = 1;
  GetAnimationRPC rpc;
  auto result = rpc.Process(req);
  ASSERT_TRUE(result.ok()) << result.message();
  EXPECT_EQ(result.value().data.size(), 1u);
}

}  // namespace

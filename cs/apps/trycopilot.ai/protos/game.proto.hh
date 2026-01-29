// cs/apps/trycopilot.ai/protos/game.proto.hh
// cs/apps/trycopilot.ai/protos/game.proto.hh
// cs/apps/trycopilot.ai/protos/game.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_GAME_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_GAME_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// Animation request payload.
DECLARE_PROTO(GetAnimationRequest) {
  // Frame width.
  int width;
  // Frame height.
  int height;
  // Current frame number.
  int frame_num;
  // Total frames.
  int num_frames;
  // Window width.
  int window_width;
  // Window height.
  int window_height;
  // Use threading flag.
  bool use_threads;
  // Max thread count.
  int max_thread_count;
};

// Animation response payload.
DECLARE_PROTO(GetAnimationResponse) {
  // Original request.
  GetAnimationRequest request;
  // Animation pixel data.
  std::vector<std::vector<std::vector<std::vector<int>>>>
      data;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_GAME_PROTO_HH

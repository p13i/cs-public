#ifndef CS_WWW_APP_PROTOS_GAME_PROTO_HH
#define CS_WWW_APP_PROTOS_GAME_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(GetAnimationRequest) {
  int width;
  int height;
  int frame_num;
  int num_frames;
  int window_width;
  int window_height;
  bool use_threads;
  int max_thread_count;
};

DECLARE_PROTO(GetAnimationResponse) {
  GetAnimationRequest request;
  std::vector<std::vector<std::vector<std::vector<int>>>>
      data;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_GAME_PROTO_HH

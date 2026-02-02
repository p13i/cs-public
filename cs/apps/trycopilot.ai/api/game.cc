// cs/apps/trycopilot.ai/api/game.cc
#include "cs/apps/trycopilot.ai/api/game.hh"

#include "cs/apps/trycopilot.ai/scene_animator.hh"
#include "cs/log.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::InvalidArgument;
using ::cs::apps::trycopilotai::SceneAnimator;
using ::cs::apps::trycopilotai::protos::GetAnimationRequest;
using ::cs::apps::trycopilotai::protos::
    GetAnimationResponse;
}  // namespace

namespace cs::apps::trycopilotai::api {

IMPLEMENT_RPC(GetAnimationRPC, GetAnimationRequest,
              GetAnimationResponse) {
  if (request.width < 1 || request.width > 1024) {
    return TRACE(
        InvalidArgument("Width must be at least 1 pixels "
                        "and at most 1024, not " +
                        std::to_string(request.width)));
  }

  if (request.height < 1 || request.height > 1024) {
    return TRACE(
        InvalidArgument("Height must be at least 1 pixels "
                        "and at most 1024, not " +
                        std::to_string(request.height)));
  }

  if (request.num_frames == 0 || request.num_frames > 72) {
    return TRACE(InvalidArgument(
        "Number of frames must be at least 1 and at most "
        "72, not " +
        std::to_string(request.num_frames)));
  }

  LOG(DEBUG) << "Request is valid: "
             << request.Serialize(/*indent=*/2) << ENDL;

  SceneAnimator animator(
      request.num_frames,
      std::make_tuple(request.width, request.height));
  std::vector<Film> frames = animator.render_all_frames(
      [&](unsigned int render_time_ms, unsigned int frame,
          unsigned int num_frames) {
        LOG(INFO) << "Rendered frame " << frame << " of "
                  << num_frames << " in " << render_time_ms
                  << " ms." << ENDL;
      });
  LOG(DEBUG) << "Rendered frames." << ENDL;

  GetAnimationResponse response;
  response.request = request;
  // Initialize the four dimensions.

  response.data.resize(request.num_frames);
  for (auto& frame : response.data) {
    frame.resize(request.width);
    for (auto& column : frame) {
      column.resize(request.height);
      for (auto& pixel : column) {
        pixel.resize(4);
      }
    }
  }

  LOG(DEBUG) << "Allocated response data." << ENDL;

  for (unsigned int i = 0; i < frames.size(); i++) {
    for (unsigned int x = 0; x < frames[i].width; x++) {
      // Copy pixels from film to frame
      for (unsigned int y = 0; y < frames[i].height; y++) {
        const auto& pixel = frames[i].pixels[x][y];
        // LOG(DEBUG) << "Pixel at (" << x << ", " << y
        //            << "): " << pixel << ENDL;
        response.data[i][x][y][0] =
            static_cast<int>(pixel.r);
        response.data[i][x][y][1] =
            static_cast<int>(pixel.g);
        response.data[i][x][y][2] =
            static_cast<int>(pixel.b);
        response.data[i][x][y][3] =
            static_cast<int>(pixel.a);
      }
    }
  }

  return response;
}

}  // namespace cs::apps::trycopilotai::api
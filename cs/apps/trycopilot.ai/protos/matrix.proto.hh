// cs/apps/trycopilot.ai/protos/matrix.proto.hh
// cs/apps/trycopilot.ai/protos/matrix.proto.hh
// cs/apps/trycopilot.ai/protos/matrix.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_MATRIX_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_MATRIX_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// 2D matrix payload.
DECLARE_PROTO(Matrix2D) {
  // Matrix shape dims.
  std::vector<int> shape;
  // Matrix values.
  std::vector<std::vector<float>> data;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_MATRIX_PROTO_HH
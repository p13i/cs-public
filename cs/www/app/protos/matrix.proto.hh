#ifndef CS_WWW_APP_PROTOS_MATRIX_PROTO_HH
#define CS_WWW_APP_PROTOS_MATRIX_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::www::app::protos {

DECLARE_PROTO(Matrix2D) {
  std::vector<int> shape;
  std::vector<std::vector<float>> data;
};

}  // namespace cs::www::app::protos

#endif  // CS_WWW_APP_PROTOS_MATRIX_PROTO_HH
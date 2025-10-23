#ifndef CS_AI_PROTOS_MNIST_DATA_PROTO_HH
#define CS_AI_PROTOS_MNIST_DATA_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::ai::protos {

DECLARE_PROTO(MNISTData) {
  std::vector<std::vector<float>> train_images;
  std::vector<int> train_labels;
  std::vector<std::vector<float>> test_images;
  std::vector<int> test_labels;
};

}  // namespace cs::ai::protos

#endif  // CS_AI_PROTOS_MNIST_DATA_PROTO_HH

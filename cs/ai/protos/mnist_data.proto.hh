// cs/ai/protos/mnist_data.proto.hh
#ifndef CS_AI_PROTOS_MNIST_DATA_PROTO_HH
#define CS_AI_PROTOS_MNIST_DATA_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::ai::protos {

// MNIST dataset payload.
DECLARE_PROTO(MNISTData) {
  // Training images.
  std::vector<std::vector<float>> train_images;
  // Training labels.
  std::vector<int> train_labels;
  // Test images.
  std::vector<std::vector<float>> test_images;
  // Test labels.
  std::vector<int> test_labels;
};

}  // namespace cs::ai::protos

#endif  // CS_AI_PROTOS_MNIST_DATA_PROTO_HH

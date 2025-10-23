#ifndef CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH
#define CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::ai::protos {

DECLARE_PROTO(NeuralModel) {
  int input_size;
  int hidden_size;
  int output_size;
  int epochs;
  float learning_rate;
  std::vector<std::vector<float>> weights1;
  std::vector<std::vector<float>> weights2;
  std::vector<float> biases1;
  std::vector<float> biases2;
  std::vector<float> z1;
  std::vector<float> a1;
  std::vector<float> z2;
  std::vector<float> a2;
};

}  // namespace cs::ai::protos

#endif  // CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH

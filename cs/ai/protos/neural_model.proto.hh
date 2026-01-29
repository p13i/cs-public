// cs/ai/protos/neural_model.proto.hh
#ifndef CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH
#define CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::ai::protos {

// Neural network model state.
DECLARE_PROTO(NeuralModel) {
  // Input dimension.
  int input_size;
  // Hidden layer size.
  int hidden_size;
  // Output dimension.
  int output_size;
  // Training epochs.
  int epochs;
  // Learning rate.
  float learning_rate;
  // Input weight matrix.
  std::vector<std::vector<float>> weights1;
  // Hidden weight matrix.
  std::vector<std::vector<float>> weights2;
  // First layer biases.
  std::vector<float> biases1;
  // Second layer biases.
  std::vector<float> biases2;
  // Layer1 pre-activation.
  std::vector<float> z1;
  // Layer1 activation.
  std::vector<float> a1;
  // Layer2 pre-activation.
  std::vector<float> z2;
  // Layer2 activation.
  std::vector<float> a2;
};

}  // namespace cs::ai::protos

#endif  // CS_AI_PROTOS_NEURAL_MODEL_PROTO_HH

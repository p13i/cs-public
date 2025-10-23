#ifndef CS_AI_NEURAL_NET_HH
#define CS_AI_NEURAL_NET_HH

#include <vector>

namespace cs::ai {

// Neural network class
class NeuralNet {
 public:
  NeuralNet(unsigned int input_size,
            unsigned int hidden_size,
            unsigned int output_size)
      : input_size(input_size),
        hidden_size(hidden_size),
        output_size(output_size) {
    init_weights();
  }

  std::vector<float> forward(
      const std::vector<float>& input);

  void train(const std::vector<std::vector<float>>& inputs,
             const std::vector<std::vector<float>>& labels,
             unsigned int epochs = 5, float lr = 0.1);

  float test(
      const std::vector<std::vector<float>>& test_inputs,
      const std::vector<int>& test_labels);

  float evaluate(
      const std::vector<std::vector<float>>& inputs,
      const std::vector<int>& true_labels,
      bool verbose = false);

  unsigned int input_size, hidden_size, output_size;
  std::vector<std::vector<float>> weights1, weights2;
  std::vector<float> biases1, biases2;
  std::vector<float> z1, a1, z2, a2;

  void init_weights();
};

}  // namespace cs::ai

#endif  // CS_AI_NEURAL_NET_HH
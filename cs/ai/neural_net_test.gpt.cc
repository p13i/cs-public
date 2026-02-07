// cs/ai/neural_net_test.gpt.cc
#include "cs/ai/neural_net.hh"

#include <vector>

#include "gtest/gtest.h"

namespace cs::ai {
namespace {

TEST(NeuralNetTest, ConstructAndForward) {
  NeuralNet net(2, 4, 2);
  std::vector<float> input = {1.0f, 0.5f};
  auto output = net.forward(input);
  ASSERT_EQ(output.size(), 2u);
  // Output should be in [0,1] range (sigmoid).
  for (auto v : output) {
    EXPECT_GE(v, 0.0f);
    EXPECT_LE(v, 1.0f);
  }
}

TEST(NeuralNetTest, ForwardDeterministic) {
  NeuralNet net(3, 5, 2);
  std::vector<float> input = {0.1f, 0.2f, 0.3f};
  auto out1 = net.forward(input);
  auto out2 = net.forward(input);
  ASSERT_EQ(out1.size(), out2.size());
  for (size_t i = 0; i < out1.size(); ++i) {
    EXPECT_FLOAT_EQ(out1[i], out2[i]);
  }
}

TEST(NeuralNetTest, TrainReducesLoss) {
  NeuralNet net(2, 8, 2);
  // Simple XOR-like data (not perfectly separable
  // with one hidden layer, but loss should decrease).
  std::vector<std::vector<float>> inputs = {{0.0f, 0.0f},
                                            {1.0f, 0.0f},
                                            {0.0f, 1.0f},
                                            {1.0f, 1.0f}};
  std::vector<std::vector<float>> labels = {{1.0f, 0.0f},
                                            {0.0f, 1.0f},
                                            {0.0f, 1.0f},
                                            {1.0f, 0.0f}};

  // Compute initial loss.
  float loss_before = 0.0f;
  for (size_t i = 0; i < inputs.size(); ++i) {
    auto out = net.forward(inputs[i]);
    for (size_t j = 0; j < out.size(); ++j) {
      float diff = out[j] - labels[i][j];
      loss_before += diff * diff;
    }
  }

  net.train(inputs, labels, /*epochs=*/50, /*lr=*/0.5f);

  float loss_after = 0.0f;
  for (size_t i = 0; i < inputs.size(); ++i) {
    auto out = net.forward(inputs[i]);
    for (size_t j = 0; j < out.size(); ++j) {
      float diff = out[j] - labels[i][j];
      loss_after += diff * diff;
    }
  }

  EXPECT_LT(loss_after, loss_before);
}

}  // namespace
}  // namespace cs::ai

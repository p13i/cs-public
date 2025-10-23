#include "cs/ai/neural_net.hh"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cs/log.hh"

namespace {
// Sigmoid activation
float sigmoid(float x) {
  return 1.0 / (1.0 + std::exp(-x));
}

float d_sigmoid(float x) {
  float s = sigmoid(x);
  return s * (1 - s);
}

int argmax(const std::vector<float>& v) {
  int idx = 0;
  float max_val = v[0];
  for (unsigned int i = 1; i < v.size(); ++i) {
    if (v[i] > max_val) {
      max_val = v[i];
      idx = i;
    }
  }
  return idx;
}

}  // namespace

namespace cs::ai {
// Neural network class

std::vector<float> NeuralNet::forward(
    const std::vector<float>& input) {
  // Hidden layer
  z1.resize(hidden_size);
  a1.resize(hidden_size);
  for (unsigned int i = 0; i < hidden_size; ++i) {
    z1[i] = biases1[i];
    for (unsigned int j = 0; j < input_size; ++j)
      z1[i] += weights1[i][j] * input[j];
    a1[i] = sigmoid(z1[i]);
  }

  // Output layer
  z2.resize(output_size);
  a2.resize(output_size);
  for (unsigned int i = 0; i < output_size; ++i) {
    z2[i] = biases2[i];
    for (unsigned int j = 0; j < hidden_size; ++j)
      z2[i] += weights2[i][j] * a1[j];
    a2[i] = sigmoid(z2[i]);
  }
  return a2;
}

void NeuralNet::train(
    const std::vector<std::vector<float>>& inputs,
    const std::vector<std::vector<float>>& labels,
    unsigned int epochs, float lr) {
  for (unsigned int epoch = 0; epoch < epochs; ++epoch) {
    LOG(DEBUG) << "Training epoch " << epoch << ENDL;
    unsigned int correct = 0;
    for (unsigned int idx = 0; idx < inputs.size(); ++idx) {
      const auto& x = inputs[idx];
      const auto& y = labels[idx];

      // Forward pass
      auto output = forward(x);

      // Accuracy count
      if (argmax(output) == argmax(y)) ++correct;

      // Backward pass (output layer)
      std::vector<float> delta2(output_size);
      for (unsigned int i = 0; i < output_size; ++i)
        delta2[i] = (output[i] - y[i]) * d_sigmoid(z2[i]);

      // Backward pass (hidden layer)
      std::vector<float> delta1(hidden_size, 0.0);
      for (unsigned int i = 0; i < hidden_size; ++i)
        for (unsigned int j = 0; j < output_size; ++j)
          delta1[i] += delta2[j] * weights2[j][i];
      for (unsigned int i = 0; i < hidden_size; ++i)
        delta1[i] *= d_sigmoid(z1[i]);

      // Update weights and biases
      for (unsigned int i = 0; i < output_size; ++i) {
        for (unsigned int j = 0; j < hidden_size; ++j)
          weights2[i][j] -= lr * delta2[i] * a1[j];
        biases2[i] -= lr * delta2[i];
      }

      for (unsigned int i = 0; i < hidden_size; ++i) {
        for (unsigned int j = 0; j < input_size; ++j)
          weights1[i][j] -= lr * delta1[i] * x[j];
        biases1[i] -= lr * delta1[i];
      }
    }
    LOG(DEBUG) << "Epoch " << epoch + 1 << " Accuracy: "
               << (100.0 * correct / inputs.size()) << "%"
               << ENDL;
  }
}

float NeuralNet::test(
    const std::vector<std::vector<float>>& test_inputs,
    const std::vector<int>& test_labels) {
  unsigned int correct = 0;
  for (unsigned int i = 0; i < test_inputs.size(); ++i) {
    auto output = forward(test_inputs[i]);
    if (argmax(output) == test_labels[i]) ++correct;
  }
  float accuracy = 100.0 * correct / test_inputs.size();
  LOG(DEBUG) << "Test Accuracy: " << accuracy << "%"
             << ENDL;
  return accuracy;
}

float NeuralNet::evaluate(
    const std::vector<std::vector<float>>& inputs,
    const std::vector<int>& true_labels, bool verbose) {
  if (inputs.size() != true_labels.size())
    throw std::runtime_error(
        "Mismatched input and label sizes");

  unsigned int correct = 0;
  for (unsigned int i = 0; i < inputs.size(); ++i) {
    auto output = forward(inputs[i]);
    int predicted = argmax(output);
    if (predicted == true_labels[i]) ++correct;

    if (verbose) {
      LOG(DEBUG) << "Sample " << i
                 << ": Predicted = " << predicted
                 << ", Actual = " << true_labels[i] << ""
                 << ENDL;
    }
  }

  float accuracy = 100.0 * correct / inputs.size();
  LOG(DEBUG) << "Evaluation Accuracy: " << accuracy << "%"
             << ENDL;
  return accuracy;
}

void NeuralNet::init_weights() {
  std::mt19937 rng(42);
  std::uniform_real_distribution<float> dist(-0.1, 0.1);

  weights1.resize(hidden_size,
                  std::vector<float>(input_size));
  weights2.resize(output_size,
                  std::vector<float>(hidden_size));
  biases1.resize(hidden_size);
  biases2.resize(output_size);

  for (auto& row : weights1)
    for (auto& val : row) val = dist(rng);
  for (auto& row : weights2)
    for (auto& val : row) val = dist(rng);
  for (auto& b : biases1) b = dist(rng);
  for (auto& b : biases2) b = dist(rng);
}

}  // namespace cs::ai
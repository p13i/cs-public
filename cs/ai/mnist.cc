// cs/ai/mnist.cc
// Mostly generated with ChatGPT.

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

#include "cs/ai/neural_net.hh"
#include "cs/ai/protos/mnist_data.proto.hh"
#include "cs/ai/protos/neural_model.proto.hh"
#include "cs/fs/fs.hh"

namespace {  // use_usings
using ::cs::ai::NeuralNet;
using ::cs::ai::protos::MNISTData;
using ::cs::ai::protos::NeuralModel;
}  // namespace

// Struct to hold all MNIST datasets

// Helper to read big-endian 32-bit integers
uint32_t read_be(std::ifstream& f) {
  uint32_t r = 0;
  for (int i = 0; i < 4; ++i) {
    r = (r << 8) | f.get();
  };
  return r;
}

// Main function to download and load MNIST data
MNISTData download_and_load_mnist() {
  const std::string BASE_URL =
      "https://raw.githubusercontent.com/fgnt/mnist/"
      "master/";

  const std::vector<std::string> FILENAMES = {
      "train-images-idx3-ubyte.gz",
      "train-labels-idx1-ubyte.gz",
      "t10k-images-idx3-ubyte.gz",
      "t10k-labels-idx1-ubyte.gz"};

  LOG(DEBUG) << "Downloading MNIST files..." << ENDL;
  for (const auto& fn : FILENAMES) {
    std::string url = BASE_URL + fn;
    std::string cmd = "curl -O " + url;
    if (std::system(cmd.c_str()) != 0)
      throw std::runtime_error("Download failed: " +
                               std::string(url));
  }

  LOG(DEBUG) << "Decompressing .gz files..." << ENDL;
  if (std::system("gunzip -f *.gz") != 0)
    throw std::runtime_error("Decompression failed");

  // Load image file
  auto load_images = [](const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
      throw std::runtime_error("Can't open: " + filename);

    if (read_be(file) != 2051)
      throw std::runtime_error("Invalid image file");
    uint32_t count = read_be(file), rows = read_be(file),
             cols = read_be(file);

    std::vector<std::vector<float>> images(
        count, std::vector<float>(rows * cols));
    for (uint32_t i = 0; i < count; ++i)
      for (uint32_t j = 0; j < rows * cols; ++j)
        images[i][j] =
            static_cast<unsigned char>(file.get()) / 255.0;
    return images;
  };

  // Load label file
  auto load_labels = [](const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
      throw std::runtime_error("Can't open: " + filename);

    if (read_be(file) != 2049)
      throw std::runtime_error("Invalid label file");
    uint32_t count = read_be(file);

    std::vector<int> labels(count);
    for (uint32_t i = 0; i < count; ++i)
      labels[i] = static_cast<unsigned char>(file.get());
    return labels;
  };

  LOG(DEBUG) << "Parsing MNIST files..." << ENDL;
  MNISTData data;
  data.train_images =
      load_images("train-images-idx3-ubyte");
  data.train_labels =
      load_labels("train-labels-idx1-ubyte");
  data.test_images = load_images("t10k-images-idx3-ubyte");
  data.test_labels = load_labels("t10k-labels-idx1-ubyte");

  LOG(DEBUG) << "Loaded " << data.train_images.size()
             << " training samples, "
             << data.test_images.size() << " test samples."
             << ENDL;

  // Include first 3 entries of each vector in the data
  // view that will be printed.
#define VECTOR_SUBSET(v)                \
  std::vector<decltype(v)::value_type>( \
      v.begin(), v.size() > 3 ? v.begin() + 3 : v.end())

  MNISTData data_view;
  data_view.train_images = VECTOR_SUBSET(data.train_images);
  data_view.train_labels = VECTOR_SUBSET(data.train_labels);
  data_view.test_images = VECTOR_SUBSET(data.test_images);
  data_view.test_labels = VECTOR_SUBSET(data.test_labels);

#undef VECTOR_SUBSET

  LOG(DEBUG) << "MNIST data (subset of first 3 samples): "
             << data_view.Serialize() << ENDL;

  return data;
}

// One-hot encode label (0–9)
std::vector<float> one_hot(int label) {
  std::vector<float> v(10, 0.0);
  v[label] = 1.0;
  return v;
}

std::vector<std::vector<float>> one_hot_many(
    std::vector<int> labels) {
  std::vector<std::vector<float>> result;
  for (const auto& i : labels) {
    result.push_back(one_hot(i));
  }
  return result;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0]
              << " <path_to_save_model>" << std::endl;
    return 1;
  }

  auto mnist_data = download_and_load_mnist();
  std::vector<std::vector<float>> images =
      mnist_data.train_images;
  std::vector<std::vector<float>> labels =
      one_hot_many(mnist_data.train_labels);

  NeuralModel model;
  model.input_size = 784;
  model.hidden_size = 64;
  model.output_size = 10;
  model.epochs = 5;
  model.learning_rate = 0.1;

  NeuralNet net(model.input_size, model.hidden_size,
                model.output_size);
  net.train(images, labels, model.epochs,
            model.learning_rate);

  model.weights1 = net.weights1;
  model.weights2 = net.weights2;
  model.biases1 = net.biases1;
  model.biases2 = net.biases2;
  model.z1 = net.z1;
  model.a1 = net.a1;
  model.z2 = net.z2;
  model.a2 = net.a2;

  std::string path = argv[1];

  LOG(DEBUG) << "Trained model: "
             << model.Serialize().substr(0, 100) << "..."
             << ENDL;

  cs::fs::write(path, model.Serialize());
  LOG(DEBUG) << "Saved model to " << path << ENDL;

  float accuracy = net.evaluate(mnist_data.test_images,
                                mnist_data.test_labels,
                                /*verbose=*/false);

  LOG(DEBUG) << "Final accuracy: " << accuracy << ENDL;

  return 0;
}

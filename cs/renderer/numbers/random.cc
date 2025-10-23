#include "random.hh"

#include <random>

float cs::renderer::math::randomFloat() {
  return std::rand() / static_cast<float>(RAND_MAX);
};

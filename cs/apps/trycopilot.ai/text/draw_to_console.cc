// cs/apps/trycopilot.ai/text/draw_to_console.cc
#include <iostream>

#include "cs/apps/trycopilot.ai/text/fonts/mono.hh"

namespace {  // use_usings
using ::cs::apps::trycopilotai::text::fonts::Mono;
}  // namespace

int main() {
  // Print each character and it's associated mono font
  // drawn from an array
  for (unsigned int i = 0; i < sizeof(Mono); i++) {
    // clang-format off
    std::cout << (char)(i + 'A') << " (index=" << i
              << "):" << "\n";
    // clang-format on
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        int flag = Mono[i][y][x];
        if (flag) {
          std::cout << "o";
        } else {
          std::cout << " ";
        }
        std::cout << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
}

// cs/labs/cpp/hello_world_test.gpt.cc
// cs/labs/cpp/hello_world_test.cc
#include <cassert>

#include "hello.h"

int main() {
  auto msg = SayHello("World");
  assert(msg == "Hello World!");
  return 0;
}

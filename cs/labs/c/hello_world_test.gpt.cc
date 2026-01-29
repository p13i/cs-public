// cs/labs/c/hello_world_test.gpt.cc
// cs/labs/c/hello_world_test.cc
#include "hello_world.h"

#include <cassert>
#include <cstring>

int main() {
  const char *got = say_hello();
  assert(std::strcmp(got, "Hello World!") == 0);
  return 0;
}

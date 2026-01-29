// cs/apps/trycopilot.ai/text/fonts/mono.cc
#include "cs/apps/trycopilot.ai/text/fonts/mono.hh"

bool cs::apps::trycopilotai::text::fonts::
    SampleCharacterPixel(char ascii_code, int x, int y) {
  if (ascii_code >= 'A' && ascii_code <= 'Z') {
    return Mono[ascii_code - 'A'][y][x] == 1;
  } else if (ascii_code == ' ') {
    return false;
  } else {
    return false;
  }
};

#include "cs/www/app/text/fonts/mono.hh"

bool cs::www::app::text::fonts::SampleCharacterPixel(
    char ascii_code, int x, int y) {
  if (ascii_code >= 'A' && ascii_code <= 'Z') {
    return Mono[ascii_code - 'A'][y][x] == 1;
  } else if (ascii_code == ' ') {
    return false;
  } else {
    return false;
  }
};

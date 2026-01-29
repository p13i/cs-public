// cs/renderer/pixel.hh
#ifndef CS_RENDERER_PIXEL_HH
#define CS_RENDERER_PIXEL_HH

#include <stdint.h>

#include <iostream>
#include <tuple>

#include "cs/log.hh"

namespace cs::renderer {

struct Pixel {
  uint8_t r, g, b, a;
  Pixel() : Pixel(0, 0, 0, 0) {}
  Pixel(uint8_t red, uint8_t green, uint8_t blue,
        uint8_t alpha)
      : r(red), g(green), b(blue), a(alpha) {}

  bool operator==(const Pixel& other) const {
    return r == other.r && g == other.g && b == other.b &&
           a == other.a;
  }

  std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
  as_tuple() {
    return std::make_tuple(r, g, b, a);
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const Pixel& px) {
    return os << "Pixel(" << static_cast<unsigned int>(px.r)
              << ", " << static_cast<unsigned int>(px.g)
              << ", " << static_cast<unsigned int>(px.b)
              << ", " << static_cast<unsigned int>(px.a)
              << ")";
  }

  int operator[](size_t i) {
    if (i == 0) {
      return r;
    } else if (i == 1) {
      return g;
    } else if (i == 2) {
      return b;
    } else if (i == 3) {
      return a;
    }

#define INVARIANT(msg)       \
  LOG(ERROR) << msg << ENDL; \
  exit(1);

    INVARIANT("Unknown index" + std::to_string(i) +
              " for Pixel operator[]");
  }
};

}  // namespace cs::renderer

#endif  // CS_RENDERER_PIXEL_HH

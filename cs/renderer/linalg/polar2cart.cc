// cs/renderer/linalg/polar2cart.cc
#include "cs/renderer/linalg/polar2cart.hh"

Point3 cs::renderer::linalg::polar2cart(float r,
                                        float theta,
                                        float phi) {
  return {/*x=*/r * sinf(theta) * cosf(phi),
          /*y=*/r * sinf(theta) * sinf(phi),
          /*z=*/r * cosf(theta)};
};

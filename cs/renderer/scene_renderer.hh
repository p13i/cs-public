#ifndef CS_RENDERER_SCENE_RENDERER_HH
#define CS_RENDERER_SCENE_RENDERER_HH

#include <stdio.h>

#include <tuple>
#include <vector>

#include "cs/renderer/film.hh"
#include "cs/renderer/geo/dist.hh"
#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/linalg/transform.hh"
#include "cs/renderer/numbers/clamp.hh"
#include "cs/renderer/numbers/map_value.hh"
#include "cs/renderer/scene.hh"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;
using r3 = ::cs::renderer::geo::Ray3;
using ::cs::numbers::clamp;
using ::cs::numbers::map_value;
using ::cs::renderer::Film;
using ::cs::renderer::geo::dist;
using ::cs::renderer::linalg::Transform;

namespace cs::renderer {

struct Camera {
  Transform w2c_;
  unsigned int pixels_per_unit_;
  float focal_length_;
  Film film_;

  Camera(Transform w2c, unsigned int pixels_per_unit,
         float focal_length, Film film)
      : w2c_(w2c),
        pixels_per_unit_(pixels_per_unit),
        focal_length_(focal_length),
        film_(film) {}

  friend std::ostream& operator<<(std::ostream& os,
                                  const Camera& camera) {
    return os << "Camera(pixels_per_unit="
              << camera.pixels_per_unit_
              << ", focal_length=" << camera.focal_length_
              << ", film dimensions: <"
              << std::get<0>(camera.film_.dimensions())
              << ", "
              << std::get<1>(camera.film_.dimensions())
              << ">)";
  }
};

class SceneRenderer {
 public:
  Camera camera_;
  Scene scene_;

  SceneRenderer(Camera camera, Scene scene)
      : camera_(camera), scene_(scene) {}

  Film render();

  struct RenderContext {
    std::size_t width;
    std::size_t height;
    float x_units;
    float y_units;
    Transform c2w;
    p3 focal_point_in_world;
  };

  void RenderPixel(std::size_t film_x, std::size_t film_y,
                   const RenderContext& ctx) const;
};
}  // namespace cs::renderer

#endif  // CS_RENDERER_SCENE_RENDERER_HH
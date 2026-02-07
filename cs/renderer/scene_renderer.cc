// cs/renderer/scene_renderer.cc
#include "cs/renderer/scene_renderer.hh"

#include <cstdint>
#include <thread>

#include "cs/renderer/film.hh"
#include "cs/renderer/geo/dot.hh"
#include "cs/renderer/geo/point2.hh"
#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/ray3.h"
#include "cs/renderer/geo/vector3.h"
#include "cs/renderer/linalg/transform.hh"
#include "cs/renderer/numbers/map_value.hh"
#include "cs/util/parallel.hh"

namespace {  // use_usings
using ::cs::renderer::geo::Point2;
using ::cs::util::ParallelForChunks;
}  // namespace

namespace cs::renderer {

void SceneRenderer::RenderPixel(
    std::size_t film_x, std::size_t film_y,
    const RenderContext& ctx) const {
  // Map pixel coordinate -> film units (centered),
  // preserving original mapping.
  const Point2 film_point = map_value(
      Point2(static_cast<float>(film_x),
             static_cast<float>(film_y)),
      Point2(0.f, 0.f),
      Point2(static_cast<float>(ctx.width),
             static_cast<float>(ctx.height)),
      Point2(-1.f * ctx.x_units / 2.f, ctx.y_units / 2.f),
      Point2(1.f * ctx.x_units / 2.f, -ctx.y_units / 2.f));

  // Film plane point in world and primary camera ray
  const p3 film_point_in_world =
      ctx.c2w(p3(film_point.x, film_point.y, 0.f)).value();
  const r3 ray(ctx.focal_point_in_world,
               film_point_in_world);

  p3 intersection_point;
  v3 normal;

  if (scene_.intersected_by(ray, &intersection_point,
                            &normal)) {
    // Luminance hack identical to your original
    const v3 to_eye = (ctx.c2w(p3(0.f, 0.f, 0.f)).value() -
                       intersection_point)
                          .unit();
    const float unit_dot_prod = dot(to_eye, normal.unit());
    const float luminance = map_value<float>(
        unit_dot_prod, 0.f, 1.f, 0.f, 255.f);

    camera_.film_.pixels[film_x][film_y] =
        Pixel(static_cast<std::uint8_t>(luminance),
              static_cast<std::uint8_t>(luminance),
              static_cast<std::uint8_t>(luminance), 255);
  } else {
    // Miss: write transparent black
    camera_.film_.pixels[film_x][film_y] =
        Pixel(0, 0, 0, 0);
  }
}

// ------------------------
// Frame render entry point
// ------------------------
Film SceneRenderer::render() {
  // Unpack dimensions
  auto [width_i, height_i] = camera_.film_.dimensions();
  const std::size_t width =
      static_cast<std::size_t>(width_i);
  const std::size_t height =
      static_cast<std::size_t>(height_i);

  const float x_units = width_i / camera_.pixels_per_unit_;
  const float y_units = height_i / camera_.pixels_per_unit_;

  const Transform w2c = camera_.w2c_;
  const Transform c2w = w2c.inverse();

  const p3 focal_point_in_world =
      c2w(p3(0.f, 0.f, -1.f * camera_.focal_length_))
          .value();

  // Build immutable per-frame context (avoid recomputing
  // per pixel)
  const RenderContext ctx{
      width,   height, x_units,
      y_units, c2w,    focal_point_in_world,
  };

  // Concurrency (keeps behavior similar to your code)
  const std::size_t maxc =
      std::thread::hardware_concurrency();

  // Parallelize by row-chunks; inside each chunk, do a
  // tight inner loop on x. Chunk size of 4 rows is a good
  // starting point for cache locality.
  ParallelForChunks(
      /*begin=*/
      0,
      /*end=*/height,
      /*chunk_size=*/4,
      /*body=*/
      [&](std::size_t y0, std::size_t y1) {
        for (std::size_t y = y0; y < y1; ++y) {
          for (std::size_t x = 0; x < width; ++x) {
            this->RenderPixel(x, y, ctx);
          }
        }
      },
      /*max_threads=*/static_cast<std::uint32_t>(maxc));

  return camera_.film_;
}

}  // namespace cs::renderer

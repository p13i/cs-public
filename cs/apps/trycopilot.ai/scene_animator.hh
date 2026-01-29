// cs/apps/trycopilot.ai/scene_animator.hh
// cs/apps/trycopilot.ai/scene_animator.hh
// cs/apps/trycopilot.ai/scene_animator.hh
#ifndef CS_APPS_TRYCOPILOT_AI_SCENE_ANIMATOR_HH
#define CS_APPS_TRYCOPILOT_AI_SCENE_ANIMATOR_HH

#include <stdio.h>

#include <functional>
#include <tuple>
#include <vector>

#include "cs/apps/trycopilot.ai/text/fonts/mono.hh"
#include "cs/renderer/film.hh"
#include "cs/renderer/linalg/transform.hh"
#include "cs/renderer/numbers/map_value.hh"
#include "cs/renderer/scene.hh"
#include "cs/renderer/scene_renderer.hh"
#include "cs/renderer/shapes/plane.hh"
#include "cs/renderer/shapes/shape.hh"
#include "cs/renderer/shapes/sphere.hh"
#include "cs/util/timeit.hh"

using ::cs::renderer::Camera;
using ::cs::renderer::Film;
using ::cs::renderer::Scene;
using ::cs::renderer::SceneRenderer;
using ::cs::renderer::shapes::Plane;
using ::cs::renderer::shapes::Shape;
using ::cs::renderer::shapes::Sphere;
using ::cs::util::timeit;
using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;
using r3 = ::cs::renderer::geo::Ray3;
using ::cs::apps::trycopilotai::text::fonts::Mono;
using ::cs::numbers::map_value;
using ::cs::renderer::linalg::Transform;
using ::cs::renderer::linalg::transforms::LookAt;
using ::cs::renderer::linalg::transforms::Translate;

namespace cs::apps::trycopilotai {

struct SceneAnimator {
  size_t num_frames_;
  std::tuple<unsigned int, unsigned int> film_dimensions_;
  SceneAnimator(size_t num_frames,
                std::tuple<unsigned int, unsigned int>
                    film_dimensions)
      : num_frames_(num_frames),
        film_dimensions_(film_dimensions) {}

  std::vector<Film> render_all_frames(
      std::function<void(unsigned int, unsigned int,
                         unsigned int)>
          on_frame_rendered_cb = nullptr) {
    std::vector<Film> frames(num_frames_);

    // Setup scene
    std::vector<Shape*> shapes{
        // Unit sphere at the origin
        new Sphere(p3(0, 0, 0), 0.5),
        // One smaller sphere at x=-1
        new Sphere(p3(-1, 0, 0), 0.25),
        // One even smaller sphere at y=1
        new Sphere(p3(0, 1, 0), 0.125),
        // One even smaller sphere at z=-1
        new Sphere(p3(0, 0, -1), 0.0625),
        // new Sphere(p3(2, 0, 0), 0.5),
        // new Sphere(p3(0, 2, 0), 0.25),
        // new Sphere(p3(0, -1, 0), 0.25),
        // new Plane(p3(1, 1, 1).unit(), -5),
    };

    Scene scene(shapes);

    for (size_t i = 0; i < num_frames_; i++) {
      // Setup camera
      unsigned int pixels_per_unit =
          std::min(std::get<0>(film_dimensions_),
                   std::get<1>(film_dimensions_)) /
          2;
      p3 pos(map_value<float>(i, 0, num_frames_, -25, 5), 0,
             map_value<float>(i, 0, num_frames_, -10, -5));
      p3 look(0, 0, 0);
      p3 up(0, 1, 0);
      Transform w2c = LookAt(pos, look, up);
      float focal_length = 5;
      Camera camera(w2c, pixels_per_unit, focal_length,
                    Film(film_dimensions_));

      // Setup renderer
      SceneRenderer renderer(camera, scene);

      Film film;
      // Measure the render time
      [[maybe_unused]]
      unsigned int render_time_ms =
          timeit([&film, &renderer]() {
            film = renderer.render();
          });

      // Draw some text on the film
      [[maybe_unused]] auto [width, height] =
          film_dimensions_;
      int xStart = 16;
      int yStart = 16;
      DrawString(&film, &xStart, yStart,
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 2);

      frames[i] = film;

      if (on_frame_rendered_cb) {
        on_frame_rendered_cb(render_time_ms, i + 1,
                             num_frames_);
      }
    }

    // De-allocate shapes
    for (Shape* shape : shapes) {
      delete shape;
    }

    return frames;
  }

  void DrawString(Film* film, int* xStart, int yStart,
                  std::string str, int scale = 1) {
    for (char ch : str) {
      for (unsigned int x = 0; x < 8; x++) {
        for (unsigned int y = 0; y < 8; y++) {
          bool value = cs::apps::trycopilotai::text::fonts::
              SampleCharacterPixel(ch, x, y);
          char rgba = value ? 255 : 0;
          unsigned int film_x = *xStart + x;
          unsigned int film_y = yStart + y;
          if (film_x < 0 || film_x >= film->width ||
              film_y < 0 || film_y >= film->height) {
            continue;
          }
          film->pixels[film_x][film_y] =
              renderer::Pixel(rgba, rgba, rgba, rgba);
        }
      }
      // Move the x start position
      *xStart += 8;
    }
  }
};

}  // namespace cs::apps::trycopilotai

#endif  // CS_APPS_TRYCOPILOT_AI_SCENE_ANIMATOR_HH
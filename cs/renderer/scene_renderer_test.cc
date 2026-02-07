// cs/renderer/scene_renderer_test.cc
#include "cs/renderer/scene_renderer.hh"

#include "cs/renderer/film.hh"
#include "cs/renderer/scene.hh"
#include "cs/renderer/shapes/plane.hh"
#include "cs/renderer/shapes/sphere.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::renderer::Camera;
using ::cs::renderer::Film;
using ::cs::renderer::Pixel;
using ::cs::renderer::Scene;
using ::cs::renderer::SceneRenderer;
using ::cs::renderer::linalg::transforms::LookAt;
using ::cs::renderer::shapes::Plane;
using ::cs::renderer::shapes::Sphere;
}  // namespace

TEST(SceneRenderer, RenderPixelHitAndMiss) {
  Film film(std::tuple<unsigned int, unsigned int>(2, 2));
  Camera camera(Transform(), 1, 1.0f, film);

  std::vector<Shape*> shapes;
  shapes.push_back(new Sphere(p3(0, 0, 0), 0.5f));
  Scene scene(shapes);
  SceneRenderer renderer(camera, scene);

  const Transform c2w = camera.w2c_.inverse();
  const p3 focal_point =
      c2w(p3(0.f, 0.f, -1.f * camera.focal_length_))
          .value();
  SceneRenderer::RenderContext ctx{
      2, 2, 2.0f, 2.0f, c2w, focal_point,
  };

  renderer.RenderPixel(1, 1, ctx);
  EXPECT_EQ(camera.film_.pixels[1][1].a, 255);

  Scene empty_scene(std::vector<Shape*>{});
  SceneRenderer empty_renderer(camera, empty_scene);
  empty_renderer.RenderPixel(1, 1, ctx);
  EXPECT_EQ(camera.film_.pixels[1][1].a, 0);

  for (auto* shape : shapes) {
    delete shape;
  }
}

TEST(SceneRenderer, RenderReturnsFilm) {
  Film film(std::tuple<unsigned int, unsigned int>(1, 1));
  Camera camera(Transform(), 1, 1.0f, film);
  Scene scene(std::vector<Shape*>{});
  SceneRenderer renderer(camera, scene);
  Film rendered = renderer.render();
  auto [width, height] = rendered.dimensions();
  EXPECT_EQ(width, 1u);
  EXPECT_EQ(height, 1u);
}

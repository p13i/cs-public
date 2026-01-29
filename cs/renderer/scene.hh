// cs/renderer/scene.hh
#ifndef CS_RENDERER_SCENE_HH
#define CS_RENDERER_SCENE_HH

#include <vector>

#include "cs/renderer/shapes/shape.hh"

using ::cs::renderer::shapes::Shape;

namespace cs::renderer {
class Scene {
 public:
  std::vector<Shape*> shapes_;
  Scene() : Scene(std::vector<Shape*>()) {}
  Scene(std::vector<Shape*> shapes) : shapes_(shapes) {}
  bool intersected_by(const r3 ray, p3* at_point,
                      v3* at_normal) const;
};
}  // namespace cs::renderer

#endif  // CS_RENDERER_SCENE_HH
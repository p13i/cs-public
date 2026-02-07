// cs/renderer/geo/cross.cc
#include "cs/renderer/geo/cross.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"

namespace {  // use_usings
using ::cs::renderer::geo::Point3;
using ::cs::renderer::geo::Vector3;
}  // namespace

Vector3 cs::renderer::geo::cross(Vector3 uno, Vector3 dos) {
  // Make the vector have unit magnitude
  const Vector3 a = uno.unit();
  const Vector3 b = dos.unit();

  float cx = a.y * b.z - a.z * b.y;
  float cy = a.z * b.x - a.x * b.z;
  float cz = a.x * b.y - a.y * b.x;

  return Vector3(Point3(cx, cy, cz));
}

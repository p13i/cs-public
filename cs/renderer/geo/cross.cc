// cs/renderer/geo/cross.cc
#include "cs/renderer/geo/cross.hh"

#include "cs/renderer/geo/point3.h"
#include "cs/renderer/geo/vector3.h"

using p3 = ::cs::renderer::geo::Point3;
using v3 = ::cs::renderer::geo::Vector3;

v3 cs::renderer::geo::cross(v3 uno, v3 dos) {
  // Make the vector have unit magnitude
  const v3 a = uno.unit();
  const v3 b = dos.unit();

  float cx = a.y * b.z - a.z * b.y;
  float cy = a.z * b.x - a.x * b.z;
  float cz = a.x * b.y - a.y * b.x;

  return v3(p3(cx, cy, cz));
}

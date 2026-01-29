// cs/renderer/geo/point2.hh
#ifndef CS_RENDERER_GEO_POINT2_HH
#define CS_RENDERER_GEO_POINT2_HH

#include <cmath>
#include <iostream>

#include "cs/renderer/precision/floats.hh"

using ::cs::renderer::precision::FloatsNear;

namespace cs::renderer::geo {

class Point2 {
 public:
  Point2() : Point2(0, 0) {}

  Point2(float xy) : Point2(xy, xy) {}

  Point2(float x, float y) : x(x), y(y) {}

  Point2 unit() const {
    Point2 this_ = *this;
    return this_ /
           sqrtf(this_.x * this_.x + this_.y * this_.y);
  }

  Point2 operator+(const Point2& other) const {
    return Point2(x + other.x, y + other.y);
  }

  bool operator==(const Point2& other) const {
    return FloatsNear(x, other.x) && FloatsNear(y, other.y);
  }

  bool operator<(const Point2& other) const {
    return x < other.x && y < other.y;
  }

  bool operator<=(const Point2& other) const {
    return x <= other.x && y <= other.y;
  }

  Point2 operator-(Point2 other) const {
    return {x - other.x, y - other.y};
  }

  Point2 operator*(float scalar) const {
    return {x * scalar, y * scalar};
  }

  Point2 operator*(Point2 other) const {
    return {x * other.x, y * other.y};
  }

  Point2 operator/(float scalar) const {
    return {x / scalar, y / scalar};
  }

  Point2 operator/(Point2 other) const {
    return {x / other.x, y / other.y};
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const Point2& point) {
    return os << "Point2(" << point.x << ", " << point.y
              << ")";
  }

  float x, y;
};

}  // namespace cs::renderer::geo

#endif  // CS_RENDERER_GEO_POINT2_HH
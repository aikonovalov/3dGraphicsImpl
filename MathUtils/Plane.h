#pragma once

#include <optional>
#include <queue>
#include "Point4.h"
#include "Triangle.h"

namespace Linear {

struct OffsetedVector {
  Vector4 begin;
  Vector4 end;
};

class Plane {
  using ElemType = Point4::ElemType;

public:
  Plane();

  Plane(const Point4& normal, ElemType shift);
  Plane(const Point4& point1, const Point4& point2, const Point4& point3);

  ElemType GetDistance(const Point4& point);
  std::optional<Point4> GetIntersectWithVector(const OffsetedVector& vector);

  void ClipThrough(std::queue<Linear::Triangle>& clip_pool);

  void Transform(const TransformMatrix4x4& transform_matrix,
                 ElemType new_shift);

private:
  Point4 normal_;
  ElemType shift_;
};

}  // namespace Linear

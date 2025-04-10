#pragma once

#include <optional>
#include <queue>
#include "Matrix.h"
#include "Point4.h"
#include "Triangle.h"

namespace Linear {

struct OffsetedVector {
  Vector4 begin;
  Vector4 end;
};

struct IntersectionResult {
  bool has_intersection;
  Point4 intersection;
};

class Plane {
  using ElemType = Point4::ElemType;

public:
  Plane();

  Plane(const Point4& normal, ElemType shift);
  Plane(const Point4& point1, const Point4& point2, const Point4& point3);

  ElemType GetDistance(const Point4& point);
  IntersectionResult GetIntersectWithVector(const OffsetedVector& vector);
  bool IsTriangleFaceTo(const Triangle& triangle);

  void ClipThrough(std::queue<Linear::Triangle>& clip_pool);

  void Transform(const TransformMatrix4x4& transform_matrix,
                 ElemType new_shift);

private:
  static constexpr ElemType kEPS = 1e-6;

  Point4 normal_;
  ElemType shift_;
};

}  // namespace Linear

#include "Plane.h"
#include <cstddef>
#include "Point4.h"

namespace Linear {

Plane::Plane() : normal_(0, 0, 0, 1), shift_(ElemType()) {
}

Plane::Plane(const Point4& normal, ElemType shift = ElemType())
    : normal_(normal), shift_(shift) {
}

Plane::Plane(const Point4& point1, const Point4& point2, const Point4& point3) {
  Point4 v1 = point2 - point1;
  Point4 v2 = point3 - point1;
  normal_ = Normalize(CrossProduct(v1, v2));
  shift_ = -DotProduct(point1, normal_);
}

ElemType Plane::GetDistance(const Point4& point) const {
  return DotProduct(normal_, point) + shift_;
}

Point4 Plane::GetNormal() const {
  return normal_;
}

IntersectionResult Plane::GetIntersectWithVector(
    const OffsetedVector& vector) const {
  ElemType dist1 = this->GetDistance(vector.begin);
  ElemType dist2 = this->GetDistance(vector.end);

  if (dist1 * dist2 > 0) {
    return IntersectionResult{.has_intersection = false};
  }
  ElemType lambda = dist1 / (dist1 - dist2);
  Vector4 result = vector.begin + (vector.end - vector.begin) * lambda;
  return IntersectionResult{.has_intersection = true, .intersection = result};
}

void Plane::Transform(const TransformMatrix4x4& transform_matrix,
                      ElemType new_shift) {
  normal_ = transform_matrix * normal_;
  shift_ = new_shift;
}

}  // namespace Linear

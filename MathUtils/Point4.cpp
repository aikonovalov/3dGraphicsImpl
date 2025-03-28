#include "Point4.h"

namespace Linear {

bool IsEmpty(const Point4& point) {
  for (Index i = 0; i < point.GetHeight(); ++i) {
    if (point(i) != 0) {
      return false;
    }
  }

  return true;
}

Point4 CrossProduct(const Point4& lhs, const Point4& rhs) {
  ElemType cx = lhs(1) * rhs(2) - lhs(2) * rhs(1);
  ElemType cy = lhs(2) * rhs(0) - lhs(0) * rhs(2);
  ElemType cz = lhs(0) * rhs(1) - lhs(1) * rhs(0);
  return Point4(cx, cy, cz);
}

Point4 Normalize(const Point4& point) {
  ElemType length = std::sqrt(point(0) * point(0) + point(1) * point(1) +
                              point(2) * point(2));
  return Point4(point(0) / length, point(1) / length, point(2) / length);
}

ElemType DotProduct(const Point4& lhs, const Point4& rhs) {
  return lhs(0) * rhs(0) + lhs(1) * rhs(1) + lhs(2) * rhs(2);
}

Point4 Transform(const TransformMatrix4x4& transform_matrix,
                 const Point4& point) {
  return transform_matrix * point;
}

}  // namespace Linear

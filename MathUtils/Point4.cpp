#include "Point4.h"

namespace Linear {

Point4 Cross(const Point4& lhs, const Point4& rhs) {
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

ElemType DotProd(const Point4& lhs, const Point4& rhs) {
  return lhs(0) * rhs(0) + lhs(1) * rhs(1) + lhs(2) * rhs(2);
}

Point4 Transform(const Matrix<Height{4}, Width{4}>& transform_matrix,
                 const Point4& point) {
  return transform_matrix * point;
}

}  // namespace Linear

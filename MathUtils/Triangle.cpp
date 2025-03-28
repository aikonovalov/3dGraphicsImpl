#include "Triangle.h"
#include <cassert>
#include "Matrix.h"
#include "Point4.h"

namespace Linear {

Triangle::Triangle(const Matrix<Detail::Height{4}, Detail::Width{3}>& matrix)
    : points_({matrix.GetColumn(0), matrix.GetColumn(1), matrix.GetColumn(2)}) {
  assert(CollinearCheck(points_[0], points_[1], points_[2]) &&
         "To make triangle, his vertices must not lay on one straight line");
}

Triangle::Triangle(const Point4& a, const Point4& b, const Point4& c)
    : points_({a, b, c}) {
  assert(CollinearCheck(a, b, c) &&
         "To make triangle, his vertices must not lay on one straight line");
}

Point4 Triangle::GetPoint(Index index) const {
  assert(index > 0 && index < 3 && "Index must be an integer from 0 to 2");
  return points_[index];
}

Point4 Triangle::GetNormal() const {
  Point4 vec1 = GetPoint(1) - GetPoint(0);
  Point4 vec2 = GetPoint(2) - GetPoint(0);

  Point4 n = CrossProduct(vec1, vec2);
  return Normalize(n);
}

void Triangle::Transform(const TransformMatrix4x4& transform_matrix) {
  for (auto& point : points_) {
    point = transform_matrix * point;
  }
}

}  // namespace Linear

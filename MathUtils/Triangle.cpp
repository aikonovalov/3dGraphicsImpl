#include "Triangle.h"
#include <cassert>
#include "Matrix.h"
#include "Point4.h"

namespace Linear {

Triangle::Triangle() = default;

Triangle::Triangle(const Matrix<Detail::Height{4}, Detail::Width{3}>& matrix)
    : points_({matrix.GetColumn(0), matrix.GetColumn(1), matrix.GetColumn(2)}) {
}

Triangle::Triangle(const Point4& a, const Point4& b, const Point4& c)
    : points_({a, b, c}) {
}

Point4& Triangle::operator()(Index index) {
  assert(index >= 0 && index < 3 && "Index must be an integer from 0 to 2");
  return points_[index];
}

Point4 Triangle::operator()(Index index) const {
  assert(index >= 0 && index < 3 && "Index must be an integer from 0 to 2");
  return points_[index];
}

Point4 Triangle::GetNormal() const {
  Point4 vec1 = (*this)(1) - (*this)(0);
  Point4 vec2 = (*this)(2) - (*this)(0);

  Point4 n = CrossProduct(vec1, vec2);
  return Normalize(n);
}

Point4 Triangle::GetPointByBarycentric(const Point4& barycentric_point) const {
  Point4 result_point;
  for (Index i = 0; i < 3; ++i) {
    result_point += barycentric_point(i) * points_[i];
  }

  return result_point;
}

ElemType Triangle::GetAreaXYProjection() const {
  return ((*this)(1)(0) - (*this)(0)(0)) * ((*this)(2)(1) - (*this)(0)(1)) -
         ((*this)(1)(1) - (*this)(0)(1)) * ((*this)(2)(0) - (*this)(0)(0));
}

void Triangle::OffsetCoords(const Point4& offset) {
  for (Index i = 0; i < 3; ++i) {
    (*this)(i) += offset;
  }
}

void Triangle::Transform(const TransformMatrix4x4& transform_matrix) {
  for (auto& point : points_) {
    point = transform_matrix * point;
  }
}

}  // namespace Linear

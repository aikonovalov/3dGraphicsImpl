#include "Triangle.h"

namespace Linear {

Triangle::Triangle() {
  for (LengthType i = 0; i < 3; ++i) {
    points_.At(3, i) = 1;
  }
}

Triangle::Triangle(const Triangle& other) : points_(other.points_) {}

Triangle::Triangle(const Matrix<4, 3>& matrix) {
  if (matrix.GetHeight() != 4 || matrix.GetWidth() != 3) {
    throw std::invalid_argument(
        "There's only matrices 4*3 for creating triangles");
  }
  points_ = matrix;
}

Triangle::Triangle(const Point3d& a, const Point3d& b, const Point3d& c)
    : points_(0) {
  for (LengthType i = 0; i < a.GetSize(); ++i) {
    points_.At(i, 0) = a.At(i);
    points_.At(i, 1) = b.At(i);
    points_.At(i, 2) = c.At(i);
  }
}

Point3d Triangle::GetPoint(LengthType index) const {
  Point3d result;
  for (LengthType i = 0; i < 3; ++i) {
    result.At(i) = points_.At(i, index);
  }
  return result;
}

Point3d Triangle::Normal() const {

  Point3d vec1 = GetPoint(1) - GetPoint(0);
  Point3d vec2 = GetPoint(2) - GetPoint(0);

  Point3d n = vec1.Cross(vec2);
  return n.Normalize();
}

void Triangle::Transform(const Matrix<4, 4>& matrix) {
  points_ = matrix * points_;
}

}  // namespace Linear

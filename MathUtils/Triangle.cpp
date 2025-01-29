#include "Triangle.h"
#include <cassert>
#include "Point3d.h"

namespace Linear {

Triangle::Triangle() {
  for (LengthType i = 0; i < 3; ++i) {
    points_.At(3, i) = 1;
  }
}

Triangle::Triangle(const Triangle& other) : points_(other.points_) {}

Triangle::Triangle(const Matrix<4, 3>& matrix) {
  points_ = matrix;
}

Triangle::Triangle(const Point3d& a, const Point3d& b, const Point3d& c)
    : points_(0) {
  {
    Point3d ab = b - a;
    Point3d ac = c - a;
    Point3d collinear_check = ac.Cross(ab);
    assert((collinear_check.At(0) != 0 || collinear_check.At(1) != 0 ||
            collinear_check.At(2) != 0) &&
           "To make triangle, his vertices must not lay on one straight line");
  }
  for (LengthType i = 0; i < a.GetSize(); ++i) {
    points_.At(i, 0) = a.At(i);
    points_.At(i, 1) = b.At(i);
    points_.At(i, 2) = c.At(i);
  }
}

Point3d Triangle::GetPoint(LengthType index) const {
  assert((index < 3) && "Invalid index in Triangle's GetPoint method");
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

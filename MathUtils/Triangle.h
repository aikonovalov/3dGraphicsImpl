#pragma once

#include <stdexcept>
#include "Point3d.h"

namespace Linear {

template <typename ElemType,
          typename = typename std::enable_if<
              std::is_arithmetic<ElemType>::value, ElemType>::type>
class Triangle {
  using LengthType = typename Matrix<ElemType>::LengthType;

public:
  Triangle() : points_(4, 3) {
    for (LengthType i = 0; i < 3; ++i) {
      points_.At(3, i) = 1;
    }
  }

  Triangle(const Matrix<ElemType>& matrix) {
    if (matrix.GetHeight() != 4 || matrix.GetWidth() != 3) {
      throw std::invalid_argument(
          "There's only matrices 4*3 for creating triangles");
    }
    points_ = matrix;
  }

  Triangle(const Point3d<ElemType>& a, const Point3d<ElemType>& b,
           const Point3d<ElemType>& c)
      : points_(4, 3) {
    for (LengthType i = 0; i < a.GetSize(); ++i) {
      points_.At(i, 0) = a.At(i);
      points_.At(i, 1) = b.At(i);
      points_.At(i, 2) = c.At(i);
    }
  }

  Point3d<ElemType> GetPoint(LengthType index) {
    Point3d<ElemType> result;
    for (LengthType i = 0; i < 3; ++i) {
      result.At(i) = points_.At(i, index);
    }
    return result;
  }

  Point3d<ElemType> Normal() const {

    Point3d<ElemType> vec1 = GetPoint(1) - GetPoint(0);
    Point3d<ElemType> vec2 = GetPoint(2) - GetPoint(0);

    Point3d<ElemType> n = vec1.Cross(vec2);
    return n.Normalize();
  }

  Triangle Transform(const Matrix<ElemType>& matrix) const {
    if (matrix.GetHeight() != 4 || matrix.GetWidth() != 4) {
      throw std::invalid_argument("Matrix must be 4x4 to transforma");
    }
    return Triangle<ElemType>(matrix * points_);
  }

private:
  Matrix<ElemType> points_;
};

}  // namespace Linear
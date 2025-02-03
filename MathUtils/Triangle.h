#pragma once

#include <stdexcept>
#include "Point3d.h"

namespace Linear {

class Triangle {
  using ElemType = Matrix<4, 3>::ElemType;
  using LengthType = Matrix<4, 3>::LengthType;

public:
  Triangle();

  Triangle(const Triangle&);

  Triangle(const Matrix<4, 3>&);

  Triangle(const Point3d&, const Point3d&, const Point3d&);

  Point3d GetPoint(LengthType) const;

  Point3d Normal() const;

  void Transform(const Matrix<4, 4>&);

private:
  Matrix<4, 3> points_;
};

}  // namespace Linear

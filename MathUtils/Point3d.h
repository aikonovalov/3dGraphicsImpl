#pragma once

#include "Matrix.h"

namespace Linear {

class Point3d {
public:
  using ElemType = Matrix<4, 1>::ElemType;
  using LengthType = int;

  Point3d();

  Point3d(const Point3d&);
  Point3d(Point3d&&);
  Point3d& operator=(const Point3d&);

  Point3d(ElemType, ElemType, ElemType);

  template <typename T1, typename T2, typename T3>
  Point3d(T1 x, T2 y, T3 z)
      : matrix_(Matrix<4, 1>::MakeVector(static_cast<ElemType>(x),
                                         static_cast<ElemType>(y),
                                         static_cast<ElemType>(z), 1)) {}

  Point3d operator+() const;
  Point3d operator-() const;
  Point3d operator+(const Point3d&) const;
  Point3d operator-(const Point3d&) const;
  Point3d operator*(const ElemType&) const;
  Point3d& operator+=(const Point3d&);
  Point3d& operator-=(const Point3d&);
  Point3d& operator*=(const ElemType);

  ElemType X() const;
  ElemType Y() const;
  ElemType Z() const;

  void SetX(ElemType);
  void SetY(ElemType);
  void SetZ(ElemType);

  ElemType& At(LengthType);
  ElemType At(const LengthType) const;

  LengthType GetSize() const;

  Point3d Cross(const Point3d&) const;

  Point3d Normalize() const;

  ElemType DotProd(const Point3d&) const;

  void Transform(const Matrix<4, 4>&);

private:
  Matrix<4, 1> matrix_;
};

}  // namespace Linear
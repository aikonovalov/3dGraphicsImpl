#pragma once

#include "Matrix.h"

namespace Linear {

template <typename ElemType,
          typename = typename std::enable_if<
              std::is_arithmetic<ElemType>::value, ElemType>::type>
class Point3d {
public:
  using LengthType = int;

  Point3d() : matrix_(Matrix<ElemType>::MakeVector({0, 0, 0, 1})) {}

  Point3d(ElemType x, ElemType y, ElemType z)
      : matrix_(Matrix<ElemType>::MakeVector({x, y, z, 1})) {}

  Point3d(const Point3d<ElemType>& other) : matrix_(other.matrix_) {}

  Point3d<ElemType> operator+() const { return *this; }
  Point3d<ElemType> operator-() const {
    Point3d<ElemType> result(*this);
    for (LengthType i = 0; i < GetSize(); ++i) {
      result.At(i) = -result.At(i);
    }
    return result;
  }
  Point3d<ElemType> operator+(const Point3d<ElemType>& other) const {
    if (matrix_.GetSize() != other.matrix_.GetSize()) {
      throw std::runtime_error("Dimensions mismatch for operator+");
    }
    Point3d<ElemType> result(*this);
    for (LengthType i = 0; i < other.GetSize(); ++i) {
      result.At(i) += other.At(i);
    }
    return result;
  }
  Point3d<ElemType> operator-(const Point3d<ElemType>& other) const {
    if (matrix_.GetSize() != other.matrix_.GetSize()) {
      throw std::runtime_error("Dimensions mismatch for operator+");
    }
    Point3d<ElemType> result(*this);
    for (LengthType i = 0; i < other.GetSize(); ++i) {
      result.At(i) -= other.At(i);
    }
    return result;
  }
  Point3d<ElemType> operator*(const ElemType& num) const {
    Point3d<ElemType> result(*this);
    for (LengthType i = 0; i < result.GetSize(); ++i) {
      result.At(i) *= num;
    }
    return result;
  }
  Point3d<ElemType>& operator+=(const Point3d<ElemType>& other) {
    if (matrix_.GetSize() != other.matrix_.GetSize()) {
      throw std::runtime_error("Dimensions mismatch for operator+");
    }
    for (LengthType i = 0; i < other.GetSize(); ++i) {
      At(i) += other.At(i);
    }
    return *this;
  }
  Point3d<ElemType>& operator-=(const Point3d<ElemType>& other) {
    if (matrix_.GetSize() != other.matrix_.GetSize()) {
      throw std::runtime_error("Dimensions mismatch for operator+");
    }
    for (LengthType i = 0; i < other.GetSize(); ++i) {
      At(i) -= other.At(i);
    }
    return *this;
  }
  Point3d<ElemType>& operator*=(const ElemType num) {
    for (LengthType i = 0; i < GetSize(); ++i) {
      At(i) *= num;
    }
    return *this;
  }

  ElemType X() const { return matrix_.At(0, 0); }
  ElemType Y() const { return matrix_.At(1, 0); }
  ElemType Z() const { return matrix_.At(2, 0); }

  void SetX(ElemType x) { matrix_.At(0, 0) = x; }
  void SetY(ElemType y) { matrix_.At(1, 0) = y; }
  void SetZ(ElemType z) { matrix_.At(2, 0) = z; }

  ElemType& At(LengthType index) { return matrix_.At(index, 0); }
  ElemType At(const LengthType index) const { return matrix_.At(index, 0); }

  LengthType GetSize() const { return matrix_.GetHeight(); }

  Point3d<ElemType> Cross(const Point3d<ElemType>& other) const {
    ElemType cx = Y() * other.Z() - Z() * other.Y();
    ElemType cy = Z() * other.X() - X() * other.Z();
    ElemType cz = X() * other.Y() - Y() * other.X();
    return Point3d<ElemType>(cx, cy, cz);
  }

  Point3d<ElemType> Normalize() const {
    ElemType length = std::sqrt(X() * X() + Y() * Y() + Z() * Z());
    return Point3d<ElemType>(X() / length, Y() / length, Z() / length);
  }

  ElemType DotProd(const Point3d<ElemType>& other) const {
    if (GetSize() != other.GetSize()) {
      throw std::runtime_error("Point3ds' dimensions mismatch for dot product");
    }
    return X() * other.X() + Y() * other.Y() + Z() * other.Z();
  }

private:
  Matrix<ElemType> matrix_;
};

}  // namespace Linear
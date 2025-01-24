#include "Point3d.h"

namespace Linear {

Point3d::Point3d() : matrix_(Matrix<4, 1>::MakeVector(0, 0, 0, 1)) {}

Point3d::Point3d(const Point3d& other) : matrix_(other.matrix_) {}

Point3d::Point3d(Point3d&& other) : matrix_(std::move(other.matrix_)) {}

Point3d& Point3d::operator=(const Point3d& other) {
  matrix_ = other.matrix_;
  return *this;
}

Point3d::Point3d(ElemType x, ElemType y, ElemType z)
    : matrix_(Matrix<4, 1>::MakeVector(x, y, z, 1)) {}

Point3d Point3d::operator+() const {
  return *this;
}

Point3d Point3d::operator-() const {
  Point3d result(*this);
  for (LengthType i = 0; i < GetSize(); ++i) {
    result.At(i) = -result.At(i);
  }
  return result;
}

Point3d Point3d::operator+(const Point3d& other) const {
  if (matrix_.GetSize() != other.matrix_.GetSize()) {
    throw std::runtime_error("Dimensions mismatch for operator+");
  }
  Point3d result(*this);
  for (LengthType i = 0; i < other.GetSize(); ++i) {
    result.At(i) += other.At(i);
  }
  return result;
}

Point3d Point3d::operator-(const Point3d& other) const {
  if (matrix_.GetSize() != other.matrix_.GetSize()) {
    throw std::runtime_error("Dimensions mismatch for operator-");
  }
  Point3d result(*this);
  for (LengthType i = 0; i < other.GetSize(); ++i) {
    result.At(i) -= other.At(i);
  }
  return result;
}

Point3d Point3d::operator*(const ElemType& num) const {
  Point3d result(*this);
  for (LengthType i = 0; i < result.GetSize(); ++i) {
    result.At(i) *= num;
  }
  return result;
}

Point3d& Point3d::operator+=(const Point3d& other) {
  if (matrix_.GetSize() != other.matrix_.GetSize()) {
    throw std::runtime_error("Dimensions mismatch for operator+=");
  }
  for (LengthType i = 0; i < other.GetSize(); ++i) {
    At(i) += other.At(i);
  }
  return *this;
}

Point3d& Point3d::operator-=(const Point3d& other) {
  if (matrix_.GetSize() != other.matrix_.GetSize()) {
    throw std::runtime_error("Dimensions mismatch for operator-=");
  }
  for (LengthType i = 0; i < other.GetSize(); ++i) {
    At(i) -= other.At(i);
  }
  return *this;
}

Point3d& Point3d::operator*=(const ElemType num) {
  for (LengthType i = 0; i < GetSize(); ++i) {
    At(i) *= num;
  }
  return *this;
}

Point3d::ElemType Point3d::X() const {
  return matrix_.At(0, 0);
}
Point3d::ElemType Point3d::Y() const {
  return matrix_.At(1, 0);
}
Point3d::ElemType Point3d::Z() const {
  return matrix_.At(2, 0);
}

void Point3d::SetX(ElemType x) {
  matrix_.At(0, 0) = x;
}
void Point3d::SetY(ElemType y) {
  matrix_.At(1, 0) = y;
}
void Point3d::SetZ(ElemType z) {
  matrix_.At(2, 0) = z;
}

Point3d::ElemType& Point3d::At(LengthType index) {
  return matrix_.At(index, 0);
}
Point3d::ElemType Point3d::At(const LengthType index) const {
  return matrix_.At(index, 0);
}

Point3d::LengthType Point3d::GetSize() const {
  return matrix_.GetHeight();
}

Point3d Point3d::Cross(const Point3d& other) const {
  ElemType cx = Y() * other.Z() - Z() * other.Y();
  ElemType cy = Z() * other.X() - X() * other.Z();
  ElemType cz = X() * other.Y() - Y() * other.X();
  return Point3d(cx, cy, cz);
}

Point3d Point3d::Normalize() const {
  ElemType length = std::sqrt(X() * X() + Y() * Y() + Z() * Z());
  return Point3d(X() / length, Y() / length, Z() / length);
}

Point3d::ElemType Point3d::DotProd(const Point3d& other) const {
  if (GetSize() != other.GetSize()) {
    throw std::runtime_error("Point3ds' dimensions mismatch for dot product");
  }
  return X() * other.X() + Y() * other.Y() + Z() * other.Z();
}

void Point3d::Transform(const Matrix<4, 4>& matrix) {
  matrix_ = matrix * matrix_;
}

}  // namespace Linear
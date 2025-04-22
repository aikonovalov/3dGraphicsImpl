#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace Linear {

namespace Detail {
enum Height : int;
enum Width : int;

constexpr Height heightFromWidth(Width elem) {
  return static_cast<Height>(elem);
}

constexpr Width widthFromHeight(Height elem) {
  return static_cast<Width>(elem);
}

}  // namespace Detail
using ElemType = double;
using Index = int;

template <Detail::Height height, Detail::Width width>
class Matrix {
  static_assert(height > 0 && width > 0,
                "Matrix dimensions must be greater than zero");

  using Height = Detail::Height;
  using Width = Detail::Width;

public:
  using Index = Linear::Index;
  using ElemType = Linear::ElemType;

  static constexpr std::underlying_type_t<Height> GetHeight() {
    return height;
  }
  static constexpr std::underlying_type_t<Width> GetWidth() {
    return width;
  }

  Matrix(ElemType elem = default_elem)
      : elements_(FillMatrix<GetHeight() * GetWidth()>(elem)) {
  }

  template <typename... Args>
  Matrix(Args... args) {
    static_assert(GetHeight() * GetWidth() == sizeof...(args),
                  "Wrong arguments count");
    Index index = 0;
    (((*this)(index / GetWidth(), index % GetWidth()) = args, ++index), ...);
  }

  Matrix(std::initializer_list<ElemType> list) {
    assert(list.size() == GetHeight() * GetWidth() && "Argument size mismatch");
    auto it = list.begin();
    for (Index i = 0; i < GetHeight() * GetWidth(); ++i, ++it) {
      elements_[i] = *it;
    }
  }

  Matrix<Detail::heightFromWidth(width), Detail::widthFromHeight(height)>
  Transpose() const {
    Matrix<Detail::heightFromWidth(width), Detail::widthFromHeight(height)>
        result;
    for (Index i = 0; i < GetHeight(); ++i) {
      for (Index j = 0; j < GetWidth(); ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  Matrix& SelfTranspose()
    requires(GetWidth() == GetHeight())
  {
    for (Index i = 0; i < GetHeight(); ++i) {
      for (Index j = i + 1; j < GetWidth(); ++j) {
        std::swap((*this)(i, j), (*this)(j, i));
      }
    }

    return *this;
  }

  ElemType& operator()(Index row, Index column) {
    assert(row >= 0 && row < height && column >= 0 && column < width &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return elements_[row * width + column];
  }
  const ElemType& operator()(Index row, Index column) const {
    assert(row >= 0 && row < height && column >= 0 && column < width &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return elements_[row * width + column];
  }

  ElemType& operator()(Index index)
    requires(GetWidth() == 1)
  {
    assert(index >= 0 && index < height &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return elements_[index];
  }
  const ElemType& operator()(Index index) const
    requires(GetWidth() == 1)
  {
    assert(index >= 0 && index < height &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return elements_[index];
  }

  Matrix operator+(const Matrix& other) const {
    Matrix result(*this);

    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      result.elements_[i] += other.elements_[i];
    }

    return result;
  }
  Matrix operator-(const Matrix& other) const {
    Matrix result(*this);

    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      result.elements_[i] -= other.elements_[i];
    }
    return result;
  }

  template <Width otherWidth>
  Matrix<height, otherWidth> operator*(
      const Matrix<Detail::heightFromWidth(width), otherWidth>& other) const {
    Matrix<height, otherWidth> result;
    for (Index k = 0; k < GetWidth(); ++k) {
      for (Index i = 0; i < GetHeight(); ++i) {
        for (Index j = 0; j < result.GetWidth(); ++j) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

  Matrix& operator*=(const ElemType num) {
    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      elements_[i] *= num;
    }
    return *this;
  }

  Matrix operator*(const ElemType num) const {
    Matrix result(*this);
    result *= num;
    return result;
  }

  friend Matrix operator*(const ElemType num, const Matrix& matrix) {
    return matrix * num;
  }

  Matrix& operator+=(const Matrix& other) {
    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      elements_[i] += other.elements_[i];
    }
    return *this;
  }
  Matrix& operator-=(const Matrix& other) {
    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      elements_[i] -= other.elements_[i];
    }
    return *this;
  }

  Matrix& operator*=(
      const Matrix<Detail::heightFromWidth(width), width>& other) {
    Matrix tmp{*this};

    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      elements_[i] = 0;
    }

    for (Index k = 0; k < GetWidth(); ++k) {
      for (Index i = 0; i < GetHeight(); ++i) {
        for (Index j = 0; j < GetWidth(); ++j) {
          (*this)(i, j) += tmp(i, k) * other(k, j);
        }
      }
    }

    return *this;
  }

  bool operator==(const Matrix& other) const {
    for (Index i = 0; i < GetHeight() * GetWidth(); ++i) {
      if (elements_[i] != other.elements_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Matrix& other) const {
    return !(*this == other);
  }

  Matrix<Height{1}, width> GetRow(Index index) const {
    assert(index >= 0 && index < GetHeight() && "Index out of range");
    Matrix<Height{1}, width> row;
    for (Index i = 0; i < GetWidth(); ++i) {
      row(0, i) = (*this)(index, i);
    }

    return row;
  }

  Matrix<height, Width{1}> GetColumn(Index index) const {
    assert(index >= 0 && index < GetWidth() && "Index out of range");
    Matrix<height, Width{1}> col;
    for (Index i = 0; i < GetHeight(); ++i) {
      col(i) = (*this)(i, index);
    }

    return col;
  }

  enum From : Index;
  enum To : Index;
  void AddToRow(From from, To to, ElemType lambda) {
    assert(from >= 0 && from < GetHeight() && "\"From\" index out of range");
    assert(to >= 0 && to < GetHeight() && "\"To\" index out of range");
    for (Index i = 0; i < GetWidth(); ++i) {
      (*this)(to, i) += (*this)(from, i) * lambda;
    }
  }

  void SwapRows(From from, To to) {
    assert(from >= 0 && from < GetHeight() && "\"From\" index out of range");
    assert(to >= 0 && to < GetHeight() && "\"To\" index out of range");
    for (Index i = 0; i < GetWidth(); ++i) {
      std::swap((*this)(to, i), (*this)(from, i));
    }
  }

  void MultiplyRow(Index row, ElemType lambda) {
    assert(row >= 0 && row < GetHeight() && "Index out of range");
    for (Index i = 0; i < GetWidth(); ++i) {
      (*this)(row, i) *= lambda;
    }
  }

  static Matrix Eye()
    requires(GetHeight() == GetWidth())
  {
    Matrix result{};
    for (Index i = 0; i < result.GetHeight(); ++i) {
      result(i, i) = 1.0;
    }
    return result;
  }

  template <typename... Args>
  static Matrix Diagonal(Args... list)
    requires(GetHeight() == GetWidth())
  {
    static_assert(height == sizeof...(list));
    Matrix result{};

    Index index = 0;
    ((result(index, index) = list, ++index), ...);
    return result;
  }

  template <typename... Args>
  static Matrix MakeVector(Args... list)
    requires(GetWidth() == 1)
  {
    static_assert(height == sizeof...(list));
    Matrix result{};

    Index index = 0;
    ((result(index, 0) = list, ++index), ...);
    return result;
  }

  static Matrix<Height{4}, Width{4}> MakeRotation(
      const std::array<ElemType, 4>& axis, ElemType angle)
    requires(GetHeight() == 4 && GetWidth() == 4)
  {
    Matrix result = Matrix::Eye();
    ElemType cosinus = std::cos(angle);
    ElemType sinus = std::sin(angle);
    result(0, 0) = cosinus + axis[0] * axis[0] * (1 - cosinus);
    result(0, 1) = axis[0] * axis[1] * (1 - cosinus) - axis[2] * sinus;
    result(0, 2) = axis[0] * axis[2] * (1 - cosinus) + axis[1] * sinus;
    result(1, 0) = result(0, 1) + 2 * axis[2] * sinus;
    result(1, 1) = cosinus + axis[1] * axis[1] * (1 - cosinus);
    result(1, 2) = axis[1] * axis[2] * (1 - cosinus) - axis[0] * sinus;
    result(2, 0) = result(0, 2) - 2 * axis[1] * sinus;
    result(2, 1) = result(1, 2) + 2 * axis[0] * sinus;
    result(2, 2) = cosinus + axis[2] * axis[2] * (1 - cosinus);

    return result;
  }

  static Matrix<Height{4}, Width{4}> MakeRotationX(ElemType angle)
    requires(GetHeight() == 4 && GetWidth() == 4)
  {
    std::array<ElemType, 4> axis = {1, 0, 0, 0};
    return MakeRotation(axis, angle);
  }

  static Matrix<Height{4}, Width{4}> MakeRotationY(ElemType angle)
    requires(GetHeight() == 4 && GetWidth() == 4)
  {
    std::array<ElemType, 4> axis = {0, 1, 0, 0};
    return MakeRotation(axis, angle);
  }

  static Matrix<Height{4}, Width{4}> MakeRotationZ(ElemType angle)
    requires(GetHeight() == 4 && GetWidth() == 4)
  {
    std::array<ElemType, 4> axis = {0, 0, 1, 0};
    return MakeRotation(axis, angle);
  }

protected:
  template <int Length>
  static constexpr std::array<ElemType, Length> FillMatrix(ElemType elem) {
    std::array<ElemType, Length> matrix;
    matrix.fill(elem);
    return matrix;
  }

  static constexpr const ElemType default_elem = 0;
  std::array<ElemType, height * width> elements_;
};

}  // namespace Linear

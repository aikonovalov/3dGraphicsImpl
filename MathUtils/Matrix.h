#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace Linear {

template <size_t Height, size_t Width>
class Matrix {
public:
  using LengthType = int;
  using ElemType = double;
  static constexpr const ElemType default_elem = 0;

  Matrix(ElemType elem = default_elem) {
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        At(i, j) = elem;
      }
    }
  }

  Matrix(const std::vector<std::vector<ElemType>>& other) {
    assert((other.size() == Height &&
            (other.size() > 0 && other[0].size() == Width)) &&
           "Input dimensions do not match matrix size.");
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        At(i, j) = other[i][j];
      }
    }
  }

  Matrix<Width, Height> Transpose() const {
    Matrix<Width, Height> result;
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        result.At(j, i) = this->At(i, j);
      }
    }
    return result;
  }
  Matrix& SelfTranspose() {
    *this = this->Transpose();
    return *this;
  }

  LengthType GetHeight() const { return Height; }
  const LengthType GetWidth() const { return Width; }
  std::pair<LengthType, LengthType> GetSize() const {
    return std::make_pair(GetHeight(), GetWidth());
  }
  ElemType& At(LengthType row, LengthType col) {
    assert(row < Height && col < Width &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return matrix_[row * Width + col];
  }
  const ElemType& At(LengthType row, LengthType col) const {
    assert(row < Height && col < Width &&
           "In Matrix's \"At\" method : arg' size mismatch");
    return matrix_[row * Width + col];
  }

  Matrix<Height, Width> operator+(const Matrix<Height, Width>& other) const {
    assert((GetSize() == other.GetSize()) &&
           "Matrix size mismatch in operator+");
    Matrix<Height, Width> result(*this);

    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        result.At(i, j) += other.At(i, j);
      }
    }

    return result;
  }
  Matrix<Height, Width> operator-(const Matrix<Height, Width>& other) const {
    assert((GetSize() == other.GetSize()) &&
           "Matrix size mismatch in operator-");

    Matrix<Height, Width> result(*this);

    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        result.At(i, j) -= other.At(i, j);
      }
    }
    return result;
  }
  template <size_t K>
  Matrix<Height, K> operator*(const Matrix<Width, K>& other) const {
    Matrix<Height, K> result;
    for (LengthType k = 0; k < GetWidth(); ++k) {
      for (LengthType i = 0; i < result.GetHeight(); ++i) {
        for (LengthType j = 0; j < result.GetWidth(); ++j) {
          result.At(i, j) += this->At(i, k) * other.At(k, j);
        }
      }
    }
    return result;
  }

  Matrix<Height, Width> operator*(const ElemType num) const {
    Matrix<Height, Width> result(*this);

    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      for (LengthType j = 0; j < result.GetWidth(); ++j) {
        result.At(i, j) *= num;
      }
    }
    return result;
  }

  Matrix<Height, Width>& operator+=(const Matrix<Height, Width>& other) {
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        this->At(i, j) += other.At(i, j);
      }
    }
    return *this;
  }
  Matrix<Height, Width>& operator-=(const Matrix<Height, Width>& other) {
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        this->At(i, j) -= other.At(i, j);
      }
    }
    return *this;
  }

  template <size_t K>
  Matrix<Height, K>& operator*=(const Matrix<Width, K>& other) {
    *this = *this * other;
    return *this;
  }

  void FirstTypeTransform(LengthType row_from, LengthType row_to,
                          ElemType lambda) {
    for (LengthType i = 0; i < this->GetWidth(); ++i) {
      this->At(row_to, i) += this->At(row_from, i) * lambda;
    }
  }

  void SecondTypeTransform(LengthType row_from, LengthType row_to) {
    for (LengthType i = 0; i < this->GetWidth(); ++i) {
      std::swap(this->At(row_to, i), this->At(row_from, i));
    }
  }

  void ThirdTypeTransform(LengthType row, ElemType lambda) {
    for (LengthType i = 0; i < this->GetWidth(); ++i) {
      this->At(row, i) *= lambda;
    }
  }

  Matrix<Height, Height> Inv() {
    // Gaussian algorithm impl
    assert(this->GetHeight() == this->GetWidth() && this->GetHeight() > 0 &&
           "Matrix shape mismath for inv op");

    Matrix<Height, Height> curr(*this);
    Matrix<Height, Width> result(this->GetHeight(), this->GetWidth());
    for (LengthType col = 0; col < result.GetWidth(); ++col) {
      result.At(col, col) = 1;
    }

    for (LengthType col = 0; col < curr.GetWidth(); ++col) {
      if (curr.At(col, col) == 0) {
        bool correct = false;
        for (LengthType row = col + 1; row < curr.GetHeight(); ++row) {
          if (curr.At(row, col) != 0) {
            curr.SecondTypeTransform(row, col);
            result.SecondTypeTransform(row, col);
            correct = true;
            break;
          }
        }
        if (!correct) {
          throw std::runtime_error("Matrix doesn't have inverse");
        }
      }

      if (curr.At(col, col) != 1) {
        ElemType lambda = 1.0 / curr.At(col, col);
        curr.ThirdTypeTransform(col, lambda);
        result.ThirdTypeTransform(col, lambda);
      }

      for (LengthType row = 0; row < curr.GetHeight(); ++row) {
        if (row == col) {
          continue;
        }

        if (curr.At(row, col) != 0) {
          ElemType lambda = -curr.At(row, col);
          curr.FirstTypeTransform(col, row, lambda);
          result.FirstTypeTransform(col, row, lambda);
        }
      }
    }

    return result;
  }

  ElemType Det() {
    assert(this->GetHeight() == this->GetWidth() && this->GetHeight() >= 0 &&
           "\"Det\" op is defined only for square matrices");

    Matrix<Height, Width> curr(*this);
    ElemType result = 1;

    for (LengthType col = 0; col < curr.GetWidth(); ++col) {
      if (curr.At(col, col) == 0) {
        bool correct = false;
        for (LengthType row = col + 1; row < curr.GetHeight(); ++row) {
          if (curr.At(row, col) != 0) {
            curr.SecondTypeTransform(row, col);
            result *= -1;
            correct = true;
            break;
          }
        }
        if (!correct) {
          result *= 0;
          return result;
        }
      }

      if (curr.At(col, col) != 1) {
        ElemType lambda = 1.0 / curr.At(col, col);
        curr.ThirdTypeTransform(col, lambda);
        result /= lambda;
      }

      for (LengthType row = 0; row < curr.GetHeight(); ++row) {
        if (row == col) {
          continue;
        }

        if (curr.At(row, col) != 0) {
          ElemType lambda = -curr.At(row, col);
          curr.FirstTypeTransform(col, row, lambda);
        }
      }
    }

    return result;
  }

  static Matrix<Height, Height> Eye() {
    Matrix<Height, Height> result{};
    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      result.At(i, i) = 1.0;
    }
    return result;
  }

  static Matrix<Height, Height> Diag(const std::vector<ElemType>& list) {
    Matrix<Height, Height> result(list.size(), list.size());
    for (LengthType i = 0; i < list.size(); ++i) {
      result.At(i, i) = list[i];
    }
    return result;
  }

  template <typename T1, typename T2, typename T3, typename T4>
  static Matrix<4, 1> MakeVector(T1 x, T2 y, T3 z, T4 v) {
    Matrix<4, 1> result{};
    result.At(0, 0) = x;
    result.At(1, 0) = y;
    result.At(2, 0) = z;
    result.At(3, 0) = v;
    return result;
  }

  static Matrix<4, 4> MakeRotationMatrix(ElemType angle_x = 0,
                                         ElemType angle_y = 0,
                                         ElemType angle_z = 0) {
    return RotateX(angle_x) * RotateY(angle_y) * RotateZ(angle_z);
  }

protected:
  static Matrix<4, 4> RotateX(ElemType angle) {
    Matrix<4, 4> result{};

    result.At(result.GetHeight() - 1, result.GetWidth() - 1) = 1;
    result.At(0, 0) = 1;

    result.At(1, 1) = cos(angle);
    result.At(1, 2) = -sin(angle);
    result.At(2, 1) = -result.At(1, 2);
    result.At(2, 2) = result.At(1, 1);

    return result;
  }
  static Matrix<4, 4> RotateY(ElemType angle) {
    Matrix<4, 4> result{};

    result.At(result.GetHeight() - 1, result.GetWidth() - 1) = 1;
    result.At(1, 1) = 1;

    result.At(0, 0) = cos(angle);
    result.At(0, 2) = sin(angle);
    result.At(2, 0) = -result.At(0, 2);
    result.At(2, 2) = result.At(0, 0);

    return result;
  }
  static Matrix<4, 4> RotateZ(ElemType angle) {
    Matrix<4, 4> result{};

    result.At(result.GetHeight() - 1, result.GetWidth() - 1) = 1;
    result.At(2, 2) = 1;

    result.At(0, 0) = cos(angle);
    result.At(0, 1) = -sin(angle);
    result.At(1, 0) = -result.At(0, 1);
    result.At(1, 1) = result.At(0, 0);

    return result;
  }

  std::array<ElemType, Height * Width> matrix_;
};

}  // namespace Linear

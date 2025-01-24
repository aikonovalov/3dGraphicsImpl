#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace Linear {

template <int N, int M>
class Matrix {
public:
  using LengthType = int;
  using ElemType = double;
  static constexpr const ElemType default_elem = 0;

  Matrix(ElemType elem = default_elem) : height_(N), width_(M) {
    if (this->height_ < 0 || this->width_ < 0) {
      throw std::invalid_argument("Matrix dimensions must be non-negative");
    }
    for (LengthType i = 0; i < GetHeight(); ++i) {
      for (LengthType j = 0; j < GetWidth(); ++j) {
        At(i, j) = elem;
      }
    }
  }

  Matrix(const std::vector<std::vector<ElemType>>& other)
      : height_(N), width_(M) {
    if (other.size() != N || (other.size() > 0 && other[0].size() != M)) {
      throw std::invalid_argument("Input dimensions do not match matrix size.");
    }
    for (LengthType i = 0; i < N; ++i) {
      for (LengthType j = 0; j < M; ++j) {
        At(i, j) = other[i][j];
      }
    }
  }

  Matrix(const Matrix<N, M>& other)
      : height_(N), width_(M), matrix_(other.matrix_) {}
  Matrix(Matrix&& other) noexcept
      : height_(std::move(other.height_)),
        width_(std::move(other.width_)),
        matrix_(std::move(other.matrix_)) {}
  Matrix& operator=(const Matrix& other) {
    this->height_ = other.GetHeight();
    this->width_ = other.GetWidth();
    this->matrix_ = other.matrix_;
    return *this;
  }
  Matrix& operator=(Matrix&& other) noexcept {
    this->height_ = std::move(other.height_);
    this->width_ = std::move(other.width_);
    this->matrix_ = std::move(other.matrix_);
    return *this;
  }

  ~Matrix() = default;

  Matrix<N, M> Transpose() const {
    Matrix<N, M> result;
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        result.At(j, i) = this->At(i, j);
      }
    }
    return result;
  }
  Matrix& Transpose() {
    *this = this->Transpose();
    return *this;
  }

  LengthType GetHeight() { return this->height_; }
  const LengthType& GetHeight() const { return this->height_; }
  LengthType GetWidth() { return this->width_; }
  const LengthType& GetWidth() const { return this->width_; }
  std::pair<LengthType, LengthType> GetSize() const {
    return std::make_pair(this->height_, this->width_);
  }
  ElemType& At(LengthType row, LengthType col) {
    if (row >= this->height_ || col >= this->width_) {
      throw std::invalid_argument(
          "In Matrix's \"At\" method : arg' size mismatch");
    }
    return matrix_[row * this->width_ + col];
  }
  const ElemType& At(LengthType row, LengthType col) const {
    if (row >= this->height_ || col >= this->width_) {
      throw std::invalid_argument(
          "In Matrix's \"At\" method : arg' size mismatch");
    }
    return matrix_[row * this->width_ + col];
  }

  Matrix<N, M> operator+(const Matrix<N, M>& other) const {
    if (GetSize() != other.GetSize()) {
      throw std::invalid_argument("Matrix size mismatch in operator+");
    }
    Matrix<N, M> result(*this);

    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        result.At(i, j) += other.At(i, j);
      }
    }

    return result;
  }
  Matrix<N, M> operator-(const Matrix<N, M>& other) const {
    if (GetSize() != other.GetSize()) {
      throw std::invalid_argument("Matrix size mismatch in operator-");
    }
    Matrix<N, M> result(*this);

    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        result.At(i, j) -= other.At(i, j);
      }
    }
    return result;
  }
  template <int K>
  Matrix<N, K> operator*(const Matrix<M, K>& other) const {
    if (this->GetWidth() != other.GetHeight()) {
      throw std::invalid_argument("Matrix size mismatch in operator*");
    }
    Matrix<N, K> result;

    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      for (LengthType j = 0; j < result.GetWidth(); ++j) {
        for (LengthType k = 0; k < this->width_; ++k) {
          result.At(i, j) += this->At(i, k) * other.At(k, j);
        }
      }
    }
    return result;
  }

  Matrix<N, M> operator*(const ElemType num) const {
    Matrix<N, M> result(*this);

    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      for (LengthType j = 0; j < result.GetWidth(); ++j) {
        result.At(i, j) *= num;
      }
    }
    return result;
  }

  Matrix<N, M>& operator+=(const Matrix<N, M>& other) {
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        this->At(i, j) += other.At(i, j);
      }
    }
    return *this;
  }
  Matrix<N, M>& operator-=(const Matrix<N, M>& other) {
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        this->At(i, j) -= other.At(i, j);
      }
    }
    return *this;
  }
  template <int K>
  Matrix<N, K>& operator*=(const Matrix<M, K>& other) {
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

  Matrix<N, M> Inv() {
    // Gaussian algorithm impl
    if (this->GetHeight() != this->GetWidth() || this->GetHeight() == 0) {
      throw std::runtime_error("Matrix shape mismath for inv op");
    }

    Matrix<N, M> curr(*this);
    Matrix<N, M> result(this->GetHeight(), this->GetWidth());
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
    if (this->GetHeight() != this->GetWidth() || this->GetHeight() == 0) {
      throw std::runtime_error(
          "\"Det\" op is defined only for square matrices");
    }

    Matrix<N, M> curr(*this);
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

  static Matrix<N, N> Eye() {
    Matrix<N, N> result{};
    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      result.At(i, i) = 1.0;
    }
    return result;
  }

  template <int Length>
  static Matrix<Length, Length> Diag(const std::vector<ElemType>& list) {
    Matrix<Length, Length> result(list.size(), list.size());
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

  LengthType height_;
  LengthType width_;
  std::array<ElemType, N * M> matrix_;
};

}  // namespace Linear
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace Linear {

template <typename ElemType,
          typename = typename std::enable_if<
              std::is_arithmetic<ElemType>::value, ElemType>::type>
class Matrix {
protected:
public:
  using LengthType = int;
  static constexpr const ElemType default_elem = 0;

  Matrix() : width_(0), height_(0), matrix_() {}

  Matrix(LengthType height, LengthType width, ElemType elem = default_elem)
      : height_(height),
        width_(width),
        matrix_(this->height_ * this->width_, elem) {
    if (this->height_ < 0 || this->width_ < 0) {
      throw std::invalid_argument("Matrix dimensions must be non-negative");
    }
  }
  Matrix(const std::vector<ElemType>& other, LengthType width = 1)
      : height_((width == 0) ? 0 : (other.size() / width)),
        width_((other.size() == 0) ? 0 : width),
        matrix_(other) {
    if (this->matrix_.size() != this->height_ * this->width_) {
      throw std::invalid_argument("Matrix dimensions mismatch");
    }
  }

  Matrix(const std::vector<std::vector<ElemType>>& other)
      : height_(other.size()),
        width_(other.front().size()),
        matrix_(this->height_ * this->width_) {
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        this->At(i, j) = other[i][j];
      }
    }
  }

  Matrix(const Matrix& other)
      : height_(other.GetHeight()),
        width_(other.GetWidth()),
        matrix_(other.GetData()) {}
  Matrix(Matrix&& other) noexcept
      : height_(std::move(other.height_)),
        width_(std::move(other.width_)),
        matrix_(std::move(other.matrix_)) {}
  Matrix& operator=(const Matrix& other) {
    this->height_ = other.GetHeight();
    this->width_ = other.GetWidth();
    this->matrix_ = other.GetData();
    return *this;
  }
  Matrix& operator=(Matrix&& other) noexcept {
    this->height_ = std::move(other.height_);
    this->width_ = std::move(other.width_);
    this->matrix_ = std::move(other.matrix_);
    return *this;
  }

  ~Matrix() = default;

  Matrix<ElemType> Transpose() const {
    Matrix<ElemType> result(this->width_, this->height_);
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
  std::vector<ElemType>& GetData() { return this->matrix_; }
  const std::vector<ElemType>& GetData() const { return this->matrix_; }
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

  Matrix<ElemType> operator+(const Matrix<ElemType>& other) const {
    if (GetSize() != other.GetSize()) {
      throw std::invalid_argument("Matrix size mismatch in operator+");
    }
    Matrix<ElemType> result(*this);

    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        result.At(i, j) += other.At(i, j);
      }
    }

    return result;
  }
  Matrix<ElemType> operator-(const Matrix<ElemType>& other) const {
    if (GetSize() != other.GetSize()) {
      throw std::invalid_argument("Matrix size mismatch in operator-");
    }
    Matrix<ElemType> result(*this);

    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        result.At(i, j) -= other.At(i, j);
      }
    }
    return result;
  }
  Matrix<ElemType> operator*(const Matrix<ElemType>& other) const {
    if (this->GetWidth() != other.GetHeight()) {
      throw std::invalid_argument("Matrix size mismatch in operator*");
    }
    Matrix<ElemType> result(this->GetHeight(), other.GetWidth());

    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      for (LengthType j = 0; j < result.GetWidth(); ++j) {
        for (LengthType k = 0; k < this->width_; ++k) {
          result.At(i, j) += this->At(i, k) * other.At(k, j);
        }
      }
    }
    return result;
  }

  Matrix<ElemType> operator*(const ElemType num) const {
    Matrix<ElemType> result(*this);

    for (LengthType i = 0; i < result.GetHeight(); ++i) {
      for (LengthType j = 0; j < result.GetWidth(); ++j) {
        result.At(i, j) *= num;
      }
    }
    return result;
  }

  Matrix<ElemType>& operator+=(const Matrix<ElemType>& other) {
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        this->At(i, j) += other.At(i, j);
      }
    }
    return *this;
  }
  Matrix<ElemType>& operator-=(const Matrix<ElemType>& other) {
    for (LengthType i = 0; i < this->height_; ++i) {
      for (LengthType j = 0; j < this->width_; ++j) {
        this->At(i, j) -= other.At(i, j);
      }
    }
    return *this;
  }
  Matrix<ElemType>& operator*=(const Matrix<ElemType>& other) {
    *this = *this * other;
    return *this;
  }

  Matrix<ElemType> DotProd(const Matrix<ElemType>& other) const {
    return this->Transpose() * other;
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

  Matrix<ElemType> Inv() {
    // Gaussian algorithm impl
    if (this->GetHeight() != this->GetWidth() || this->GetHeight() == 0) {
      throw std::runtime_error("Matrix shape mismath for inv op");
    }

    Matrix<ElemType> curr(*this);
    Matrix<ElemType> result(this->GetHeight(), this->GetWidth());
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

    Matrix<ElemType> curr(*this);
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

  static Matrix<ElemType> Eye(LengthType rang) {
    Matrix<ElemType> result(rang, rang);
    for (LengthType i = 0; i < rang; ++i) {
      result.At(i, i) = 1.0;
    }
    return result;
  }

  static Matrix<ElemType> Diag(const std::vector<ElemType>& list) {
    Matrix<ElemType> result(list.size(), list.size());
    for (LengthType i = 0; i < list.size(); ++i) {
      result.At(i, i) = list[i];
    }
    return result;
  }

  static Matrix<ElemType> MakeVector(const std::vector<ElemType>& list,
                                     bool is_vertical = true) {
    Matrix<ElemType> result(list.size(), (list.size() == 0) ? 0 : 1);
    for (LengthType i = 0; i < list.size(); ++i) {
      result.At(i, 0) = list[i];
    }
    return result;
  }

  static Matrix<ElemType> MakeRotationMatrix(ElemType angle_x = 0,
                                             ElemType angle_y = 0,
                                             ElemType angle_z = 0) {
    return RotateX(angle_x) * RotateY(angle_y) * RotateZ(angle_z);
  }

protected:
  static Matrix<ElemType> RotateX(ElemType angle) {
    Matrix<ElemType> result(4, 4);

    result.At(result.GetHeight() - 1, result.GetWidth() - 1) = 1;
    result.At(0, 0) = 1;

    result.At(1, 1) = cos(angle);
    result.At(1, 2) = -sin(angle);
    result.At(2, 1) = -result.At(1, 2);
    result.At(2, 2) = result.At(1, 1);

    return result;
  }
  static Matrix<ElemType> RotateY(ElemType angle) {
    Matrix<ElemType> result(4, 4);

    result.At(result.GetHeight() - 1, result.GetWidth() - 1) = 1;
    result.At(1, 1) = 1;

    result.At(0, 0) = cos(angle);
    result.At(0, 2) = sin(angle);
    result.At(2, 0) = -result.At(0, 2);
    result.At(2, 2) = result.At(0, 0);

    return result;
  }
  static Matrix<ElemType> RotateZ(ElemType angle) {
    Matrix<ElemType> result(4, 4);

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
  std::vector<ElemType> matrix_;
};

}  // namespace Linear
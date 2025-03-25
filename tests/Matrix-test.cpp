#include "../MathUtils/Matrix.h"

#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace testing {

using Height = Linear::Detail::Height;
using Width = Linear::Detail::Width;

double delta = 1e-6;

TEST_CASE("Constructor dimensions", "[Matrix]") {
  Linear::Matrix<Height{4}, Width{5}> mat;
  REQUIRE(mat.GetHeight() == 4);
  REQUIRE(mat.GetWidth() == 5);
}

TEST_CASE("Constructor with template arguments", "[Matrix]") {
  Linear::Matrix<Height{3}, Width{4}> mat{1, 2, 3, 4,  5,  6,
                                          7, 8, 9, 10, 11, 12};
  {
    REQUIRE(mat(0, 2) == 3);
    REQUIRE(mat(2, 0) == 9);
  }
}

TEST_CASE("Transpose", "[Matrix]") {
  Linear::Matrix<Height{4}, Width{5}> mat;
  auto transposedMat = mat.Transpose();
  REQUIRE(transposedMat.GetHeight() == 5);
  REQUIRE(transposedMat.GetWidth() == 4);
}

TEST_CASE("Self-transpose", "[Matrix]") {
  Linear::Matrix<Height{4}, Width{4}> mat{1, 2,  3,  4,  5,  6,  7,  8,
                                          9, 10, 11, 12, 13, 14, 15, 16};

  auto transposedMat = mat.Transpose();
  mat.SelfTranspose();
  REQUIRE(mat(0, 1) == transposedMat(0, 1));
  REQUIRE(mat == transposedMat);
}

TEST_CASE("Multiplication", "[Matrix]") {
  Linear::Matrix<Height{3}, Width{4}> mat1{1, 2, 3, 4,  5,  6,
                                           7, 8, 9, 10, 11, 12};
  Linear::Matrix<Height{4}, Width{2}> mat2{4, 5, 6, 7, 8, 9, 0, 1};
  Linear::Matrix<Height{3}, Width{2}> expected_matrix{40,  50,  112,
                                                      138, 184, 226};

  Linear::Matrix<Height{3}, Width{2}> real_matrix = mat1 * mat2;
  REQUIRE(real_matrix == expected_matrix);
}

TEST_CASE("Multiplication by num", "[Matrix]") {
  Linear::Matrix<Height{3}, Width{4}> mat{1, 2, 3, 4,  5,  6,
                                          7, 8, 9, 10, 11, 12};
  double c = 10.1;
  REQUIRE((c * mat)(2, 1) == 101);
  REQUIRE(c * mat == mat * c);
}

TEST_CASE("Self-multiplication", "[Matrix]") {
  Linear::Matrix<Height{4}, Width{3}> real_matrix{1, 2, 3, 4,  5,  6,
                                                  7, 8, 9, 10, 11, 12};
  Linear::Matrix<Height{3}, Width{3}> mat2{4, 5, 6, 7, 8, 9, 0, 1, 2};
  Linear::Matrix<Height{4}, Width{3}> expected_matrix{
      18, 24, 30, 51, 66, 81, 84, 108, 132, 117, 150, 183};

  real_matrix *= mat2;
  REQUIRE(real_matrix == expected_matrix);
}

TEST_CASE("Diag function test", "[Matrix]") {
  auto diag_matrix =
      Linear::Matrix<Height{5}, Width{5}>::Diagonal(4, 5, 6, 7, 8);
  {
    REQUIRE(diag_matrix(3, 3) == 7);
    REQUIRE(diag_matrix(4, 4) == 8);
    REQUIRE(diag_matrix(4, 3) == 0);
  }
}

TEST_CASE("Rotation matrix", "[Matrix]") {
  auto rotation_matrix =
      Linear::Matrix<Height{4}, Width{4}>::MakeRotation({0, 0, 1}, M_PI / 2.0);

  Linear::Matrix<Height{4}, Width{1}> vector{1, 0, 0, 1};
  auto result = rotation_matrix * vector;
  CHECK(std::abs(result(0, 0) - 0) < delta);
  CHECK(std::abs(result(1, 0) - 1) < delta);
  CHECK(std::abs(result(2, 0) - 0) < delta);
  CHECK(std::abs(result(3, 0) - 1) < delta);
}

}  // namespace testing

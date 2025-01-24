#pragma once

#include <cmath>
#include "../MathUtils/Plane.h"

namespace Scene {

struct Camera {
  using ElemType = Linear::Plane::ElemType;

public:
  Camera();

  Camera(ElemType, ElemType, ElemType);

  // Rotate camera along 3 axis
  void Rotate(ElemType, ElemType, ElemType);

  const Linear::Matrix<4, 4>& GetTransformMatrix();

  const ElemType FOV = 90;
  ElemType aspectRatio = 1.7;
  ElemType nearDistance = 0.1;
  ElemType farDistance = 100.0;

private:
  void MakeTransform();

  Linear::Matrix<4, 4> transform_matrix_;

  Linear::Plane near_;
  Linear::Plane far_;
  Linear::Plane top_;
  Linear::Plane bottom_;
  Linear::Plane left_;
  Linear::Plane right_;
};

}  // namespace Scene
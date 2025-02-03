#pragma once

#include <cmath>
#include "../MathUtils/Plane.h"

namespace Scene {

struct Camera {
  using ElemType = Linear::Plane::ElemType;

public:
  class ParametersHandler {
  public:
    ParametersHandler();
    ParametersHandler(ElemType, ElemType, ElemType, ElemType, ElemType);

    void SetFov(ElemType);
    void SetAspect(ElemType);
    void SetNearDistance(ElemType);
    void SetFarDistance(ElemType);

    void BuildPlanes();

    void ConstructTransformMatrix(Linear::Matrix<4, 4>&);

  private:
    ElemType fov;
    ElemType aspect_ratio;  // TODO: Connect with QT
    ElemType near_distance;
    ElemType far_distance;
    ElemType hfov_sinus;
    ElemType hfov_cosinus;
    ElemType hfov_tangens;
    ElemType focal_length;

    Linear::Plane near;
    Linear::Plane far;
    Linear::Plane top;
    Linear::Plane bottom;
    Linear::Plane left;
    Linear::Plane right;
  };

  Camera();

  Camera(ElemType, ElemType, ElemType);

  // Rotate camera along 3 axis
  void Rotate(ElemType, ElemType, ElemType);

  void ResetRotate();

  const Linear::Matrix<4, 4>& GetTransformMatrix() const;

  const Linear::Matrix<4, 4>& GetRotateMatrix() const;

  Linear::Triangle Transform(Linear::Triangle&);

private:
  void ConstructTransformMatrix();

  Linear::Matrix<4, 4> transform_matrix_;
  Linear::Matrix<4, 4> rotation_matrix_;
  ParametersHandler handler_;
};

}  // namespace Scene

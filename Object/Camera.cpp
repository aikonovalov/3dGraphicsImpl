#include "Camera.h"

namespace Scene {

Camera::Camera()
    : near_({0, 0, 1}, -1),
      far_({0, 0, -1}, -1),
      top_({0, -1, 0}, -1),
      bottom_({0, 1, 0}, -1),
      left_({1, 0, 0}, -1),
      right_({-1, 0, 0}, -1) {}

Camera::Camera(const ElemType x_angle, const ElemType y_angle,
               const ElemType z_angle)
    : Camera() {
  Rotate(x_angle, y_angle, z_angle);
}

void Camera::Rotate(const ElemType x_angle, const ElemType y_angle,
                    const ElemType z_angle) {
  Linear::Matrix<4, 4> rotaion =
      Linear::Matrix<4, 4>::MakeRotationMatrix(x_angle, y_angle, z_angle);
  top_.Rotate(rotaion);
  bottom_.Rotate(rotaion);
  left_.Rotate(rotaion);
  right_.Rotate(rotaion);
  near_.Rotate(rotaion);
  far_.Rotate(rotaion);
}

const Linear::Matrix<4, 4>& Camera::GetTransformMatrix() {
  return transform_matrix_;
}

void Camera::MakeTransform() {
  ElemType tangens = std::tan(FOV / 2.0);
  ElemType dist = farDistance - nearDistance;
  transform_matrix_.At(0, 0) = 1.0 / (aspectRatio * tangens);
  transform_matrix_.At(1, 1) = 1.0 / tangens;
  transform_matrix_.At(2, 2) = -(farDistance + nearDistance) / dist;
  transform_matrix_.At(2, 3) = -2.0 * farDistance * nearDistance / dist;
  transform_matrix_.At(3, 2) = -1.0;
};

}  // namespace Scene
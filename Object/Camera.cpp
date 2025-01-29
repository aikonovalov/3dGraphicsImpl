#include "Camera.h"

namespace Scene {

Camera::ParametersHandler::ParametersHandler()
    : fov(90),
      aspect_ratio(1.7),
      near_distance(0.1),
      far_distance(100.0),
      hfov_sinus(std::sin(fov / 2.0)),
      hfov_cosinus(std::cos(fov / 2.0)),
      hfov_tangens(std::tan(fov / 2.0)),
      focal_length(1 / hfov_tangens) {
  BuildPlanes();
}

Camera::ParametersHandler::ParametersHandler(ElemType fov, ElemType asp_rat,
                                             ElemType ndist, ElemType fdist,
                                             ElemType flen)
    : fov(fov),
      aspect_ratio(asp_rat),
      near_distance(ndist),
      far_distance(fdist),
      focal_length(flen) {
  BuildPlanes();
}

void Camera::ParametersHandler::SetFov(ElemType new_fov) {
  fov = new_fov;
  hfov_sinus = std::sin(fov / 2.0);
  hfov_cosinus = std::cos(fov / 2.0);
  hfov_tangens = std::tan(fov / 2.0);

  BuildPlanes();
}

void Camera::ParametersHandler::SetAspect(ElemType new_aspect) {
  aspect_ratio = new_aspect;
  BuildPlanes();
}

void Camera::ParametersHandler::SetNearDistance(ElemType new_nd) {
  near_distance = new_nd;
  BuildPlanes();
}

void Camera::ParametersHandler::SetFarDistance(ElemType new_fd) {
  far_distance = new_fd;
  BuildPlanes();
}

void Camera::ParametersHandler::BuildPlanes() {
  near = Linear::Plane({0, 0, -1.0}, -near_distance);
  far = Linear::Plane({0, 0, 1.0}, far_distance);
  left = Linear::Plane({focal_length, 0, -1.0}, 0);
  right = Linear::Plane({-focal_length, 0, -1.0}, 0);
  top = Linear::Plane({0, focal_length, aspect_ratio}, 0);
  bottom = Linear::Plane({0, -focal_length, aspect_ratio}, 0);
}

void Camera::ParametersHandler::ConstructTransformMatrix(
    Linear::Matrix<4, 4>& matrix) {
  matrix.At(0, 0) = focal_length;
  matrix.At(1, 1) = near_distance / focal_length;
  matrix.At(2, 2) =
      -(far_distance + near_distance) / (far_distance - near_distance);
  matrix.At(3, 2) = -1.0;
  matrix.At(2, 3) =
      -2 * near_distance * far_distance / (far_distance - near_distance);
}

Camera::Camera() : handler_() {}

Camera::Camera(const ElemType xy_angle, const ElemType yz_angle,
               const ElemType xz_angle)
    : Camera() {
  Rotate(xy_angle, yz_angle, xz_angle);
}

void Camera::Rotate(const ElemType xy_angle, const ElemType yz_angle,
                    const ElemType xz_angle) {
  rotation_matrix_ *=
      Linear::Matrix<4, 4>::MakeRotationMatrix(xy_angle, yz_angle, xz_angle);
}

void Camera::ResetRotate() {
  rotation_matrix_ = Linear::Matrix<4, 4>::Eye();
}

const Linear::Matrix<4, 4>& Camera::GetTransformMatrix() const {
  return transform_matrix_;
}

const Linear::Matrix<4, 4>& Camera::GetRotateMatrix() const {
  return transform_matrix_;
}

Linear::Triangle Camera::Transform(Linear::Triangle& triangle) {
  triangle.Transform(transform_matrix_ * rotation_matrix_);
  return triangle;
}

void Camera::ConstructTransformMatrix() {
  handler_.ConstructTransformMatrix(transform_matrix_);
};

}  // namespace Scene
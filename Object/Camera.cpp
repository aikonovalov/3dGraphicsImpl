#include "Camera.h"
#include <cmath>

namespace Scene {

const Linear::Point4 Camera::kDEFAULT_POSITION = {0, 0, 0, 1};
const Linear::Vector4 Camera::kWORLD_UP_VEC = {0, 0, 1, 0};
const Linear::ElemType Camera::kDEFAULT_ASPECT_RATIO = 1;
const Linear::ElemType Camera::kDEFAULT_NEAR_DISTANCE = 0.1;
const Linear::ElemType Camera::kDEFAULT_FAR_DISTANCE = 1000;

const Linear::ElemType Camera::kDEFAULT_FOV = M_PI / 2.0;
const Linear::ElemType Camera::kDEFAULT_PITCH = 0;
const Linear::ElemType Camera::kDEFAULT_YAW = 0;

Camera::Camera(const Point4& position, const ElemType aspect_ratio,
               const ElemType near_distance, const ElemType far_distance,
               const ElemType fov, const ElemType pitch, const ElemType yaw)
    : position_(position),
      aspect_ratio_(aspect_ratio),
      near_distance_(near_distance),
      far_distance_(far_distance),
      fov_(fov),
      pitch_(pitch),
      yaw_(yaw) {
}

Linear::TransformMatrix4x4 Camera::GetAlignedFrustumMatrix() const {
  const ElemType half_fov_tangens = std::tan(GetFOV() / 2);

  Linear::TransformMatrix4x4 aligned_frustum_matrix =
      Linear::TransformMatrix4x4::Eye();
  aligned_frustum_matrix(0, 0) = 1.0 / (GetAspectRatio() * half_fov_tangens);
  aligned_frustum_matrix(1, 1) = 1.0 / half_fov_tangens;
  aligned_frustum_matrix(2, 2) = -(GetFarDistance() + GetNearDistance()) /
                                 (GetFarDistance() - GetNearDistance());
  aligned_frustum_matrix(2, 3) = -(2.0 * GetFarDistance() * GetNearDistance()) /
                                 (GetFarDistance() - GetNearDistance());
  aligned_frustum_matrix(3, 2) = -1.0;

  return aligned_frustum_matrix;
}

Linear::TransformMatrix4x4 Camera::GetViewDirectionMatrix() const {
  CameraAxes camera_axes = GetCameraAxes();

  TransformMatrix4x4 view_direction_matrix{
      camera_axes.right(0),
      camera_axes.right(1),
      camera_axes.right(2),
      -Linear::DotProduct(camera_axes.right, position_),
      camera_axes.up(0),
      camera_axes.up(1),
      camera_axes.up(2),
      -Linear::DotProduct(camera_axes.up, position_),
      -camera_axes.forward(0),
      -camera_axes.forward(1),
      -camera_axes.forward(2),
      Linear::DotProduct(camera_axes.forward, position_),
      0,
      0,
      0,
      1};

  return view_direction_matrix;
}

CameraAxes Camera::GetCameraAxes() const {
  const ElemType cos_pitch = std::cos(pitch_);
  const ElemType sin_pitch = std::sin(pitch_);
  const ElemType cos_yaw = std::cos(yaw_);
  const ElemType sin_yaw = std::sin(yaw_);

  CameraAxes result;

  result.forward = Linear::Normalize(
      {cos_pitch * cos_yaw, cos_pitch * sin_yaw, sin_pitch, 0});
  result.right =
      Linear::Normalize(Linear::CrossProduct(result.forward, kWORLD_UP_VEC));
  result.up =
      Linear::Normalize(Linear::CrossProduct(result.right, result.forward));

  return result;
}

Linear::TransformMatrix4x4 Camera::GetFullFrustumMatrix() const {
  return GetAlignedFrustumMatrix() * GetViewDirectionMatrix();
}

const FrustumPlanes Camera::GetFrustumPlanes() const {
  // In this method, we construct six clipping planes based on nine points of the camera's view frustum
  CameraAxes camera_axes = GetCameraAxes();
  ElemType half_fov_tangens = std::tan(fov_ / 2.0);

  ElemType near_height = 2 * half_fov_tangens * near_distance_;
  ElemType near_width = near_height * aspect_ratio_;

  ElemType far_height = 2 * half_fov_tangens * far_distance_;
  ElemType far_width = far_height * aspect_ratio_;

  Point4 center_near_plane = camera_axes.forward * near_distance_;
  Point4 center_far_plane = camera_axes.forward * far_distance_;

  Point4 near_up_left = center_near_plane -
                        camera_axes.right * (near_width / 2.0) +
                        camera_axes.up * (near_height / 2.0);
  Point4 near_up_right = center_near_plane +
                         camera_axes.right * (near_width / 2.0) +
                         camera_axes.up * (near_height / 2.0);
  Point4 near_down_left = center_near_plane -
                          camera_axes.right * (near_width / 2.0) -
                          camera_axes.up * (near_height / 2.0);
  Point4 near_down_right = center_near_plane +
                           camera_axes.right * (near_width / 2.0) -
                           camera_axes.up * (near_height / 2.0);

  Point4 far_up_left = center_far_plane -
                       camera_axes.right * (far_width / 2.0) +
                       camera_axes.up * (far_height / 2.0);
  Point4 far_up_right = center_far_plane +
                        camera_axes.right * (far_width / 2.0) +
                        camera_axes.up * (far_height / 2.0);
  Point4 far_down_left = center_far_plane -
                         camera_axes.right * (far_width / 2.0) -
                         camera_axes.up * (far_height / 2.0);
  Point4 far_down_right = center_far_plane +
                          camera_axes.right * (far_width / 2.0) -
                          camera_axes.up * (far_height / 2.0);

  return FrustumPlanes{
      .near = {near_down_left, near_down_right, near_up_right},
      .far = {far_up_right, far_down_right, far_down_left},
      .up = {far_up_right, near_up_right, near_up_left},
      .down = {far_down_right, near_down_left, near_down_right},
      .left = {far_down_left, near_up_left, near_down_left},
      .right = {near_up_right, far_down_right, near_down_right}};
}

Linear::Point4 Camera::GetPosition() const {
  return position_;
}

void Camera::SetPosition(const Linear::Point4& new_position) {
  position_ = new_position;
}

Linear::ElemType Camera::GetAspectRatio() const {
  return aspect_ratio_;
}
void Camera::SetAspectRatio(Linear::ElemType new_aspect_ratio) {
  aspect_ratio_ = new_aspect_ratio;
}

Linear::ElemType Camera::GetNearDistance() const {
  return near_distance_;
}
void Camera::SetNearDistance(ElemType new_near_distance) {
  near_distance_ = new_near_distance;
}

Linear::ElemType Camera::GetFarDistance() const {
  return far_distance_;
}
void Camera::SetFarDistance(Linear::ElemType new_far_distance) {
  far_distance_ = new_far_distance;
}

Linear::ElemType Camera::GetFOV() const {
  return fov_;
}
void Camera::SetFOV(Linear::ElemType new_fov) {
  fov_ = new_fov;
}

Linear::ElemType Camera::GetPitch() const {
  return pitch_;
}
void Camera::SetPitch(Linear::ElemType new_pitch) {
  pitch_ = new_pitch;
}

Linear::ElemType Camera::GetYAW() const {
  return yaw_;
}
void Camera::SetYAW(Linear::ElemType new_yaw) {
  yaw_ = new_yaw;
}

}  // namespace Scene

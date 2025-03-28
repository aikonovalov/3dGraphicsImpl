#pragma once

#include <array>
#include <cmath>
#include "../MathUtils/Plane.h"

namespace Scene {

struct FrustumPlanes {
  Linear::Plane near;
  Linear::Plane far;
  Linear::Plane up;
  Linear::Plane down;
  Linear::Plane left;
  Linear::Plane right;
};

struct CameraAxes {
  Linear::Vector4 forward;
  Linear::Vector4 right;
  Linear::Vector4 up;
};

class Camera {
  using ElemType = Linear::ElemType;
  using Point4 = Linear::Point4;
  using Vector4 = Linear::Vector4;
  using Plane = Linear::Plane;
  using TransformMatrix4x4 = Linear::TransformMatrix4x4;

public:
  explicit Camera(const Point4& position, const ElemType aspect_ratio,
                  const ElemType near_distance, const ElemType far_distance,
                  const ElemType fov, const ElemType pitch, const ElemType yaw);

  TransformMatrix4x4 GetAlignedFrustumMatrix() const;

  TransformMatrix4x4 GetViewDirectionMatrix() const;

  CameraAxes GetCameraAxes() const;

  TransformMatrix4x4 GetFullFrustumMatrix() const;

  const FrustumPlanes GetFrustumPlanes() const;

  Point4 GetPosition() const;
  void SetPosition(const Point4& new_position);

  ElemType GetAspectRatio() const;
  void SetAspectRatio(ElemType new_focal_length);

  ElemType GetNearDistance() const;
  void SetNearDistance(ElemType new_near_distance);

  ElemType GetFarDistance() const;
  void SetFarDistance(ElemType new_far_distance);

  ElemType GetFOV() const;
  void SetFOV(ElemType new_fov);

  ElemType GetPitch() const;
  void SetPitch(ElemType new_pitch);

  ElemType GetYAW() const;
  void SetYAW(ElemType new_yaw);

private:
  void UpdateDirection();

  static const Point4 kDEFAULT_POSITION;
  static const Vector4 kWORLD_UP_VEC;
  static const ElemType kDEFAULT_ASPECT_RATIO;
  static const ElemType kDEFAULT_NEAR_DISTANCE;
  static const ElemType kDEFAULT_FAR_DISTANCE;

  static const ElemType kDEFAULT_FOV;
  static const ElemType kDEFAULT_PITCH;
  static const ElemType kDEFAULT_YAW;

  Point4 position_ = kDEFAULT_POSITION;
  ElemType aspect_ratio_ = kDEFAULT_ASPECT_RATIO;
  ElemType near_distance_ = kDEFAULT_NEAR_DISTANCE;
  ElemType far_distance_ = kDEFAULT_FAR_DISTANCE;

  ElemType fov_ = kDEFAULT_FOV;
  ElemType pitch_ = kDEFAULT_PITCH;
  ElemType yaw_ = kDEFAULT_YAW;
};

}  // namespace Scene

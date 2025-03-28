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
  explicit Camera(const Point4& position = kDEFAULT_POSITION,
                  const ElemType aspect_ratio = kDEFAULT_ASPECT_RATIO,
                  const ElemType near_distance = kDEFAULT_NEAR_DISTANCE,
                  const ElemType far_distance = kDEFAULT_FAR_DISTANCE,
                  const ElemType fov = kDEFAULT_FOV,
                  const ElemType pitch = kDEFAULT_PITCH,
                  const ElemType yaw = kDEFAULT_YAW);

  void LookAtPoint(const Point4& view_point);

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

  static inline const Point4 kDEFAULT_POSITION = {0, 0, 0, 1};
  static inline const Vector4 kWORLD_UP_VEC = {0, 0, 1, 0};

  static inline const ElemType kDEFAULT_ASPECT_RATIO = 1;
  static inline const ElemType kDEFAULT_NEAR_DISTANCE = 0.1;
  static inline const ElemType kDEFAULT_FAR_DISTANCE = 10000.0;

  static inline const ElemType kDEFAULT_FOV = M_PI / 2.0;
  static inline const ElemType kDEFAULT_PITCH = 0;
  static inline const ElemType kDEFAULT_YAW = 0;

  Point4 position_;
  ElemType aspect_ratio_;
  ElemType near_distance_;
  ElemType far_distance_;

  ElemType fov_;
  ElemType pitch_;
  ElemType yaw_;
};

}  // namespace Scene

#pragma once

#include <array>
#include "../Object/Camera.h"
#include "../Object/Object.h"
#include "../Object/TriangleData.h"

namespace Rendering {

class LightCameras {
  using ElemType = Linear::ElemType;
  using Point4 = Linear::Point4;
  using Camera = Scene::Camera;

public:
  LightCameras(const Point4& location);

  void SetPosition(const Point4& new_position);

  const Camera& GetVisibleCamera(const Point4& direction);

private:
  static constexpr ElemType kDEFAULT_LIGHTCUBE_ASPECT_RATIO = 1;

  Camera x_plus_dir;
  Camera x_minus_dir;
  Camera y_plus_dir;
  Camera y_minus_dir;
  Camera z_plus_dir;
  Camera z_minus_dir;
};

class LightManager {
  using ElemType = Linear::ElemType;
  using Point4 = Linear::Point4;
  using Index = Linear::Index;
  using TransformMatrix4x4 = Linear::TransformMatrix4x4;

  using Camera = Scene::Camera;
  using Object = Scene::Object;

  using TriangleData = Scene::TriangleData;
  using Light = Detail::Light;
  using Lights = Detail::Lights;

public:
  LightManager();
  // Required for shadow mapping
  ElemType ComputeShadowMap(const std::vector<Object>& objects,
                            const Point4& point, const Light& light);

  // Required for shadow mapping
  ElemType ComputeShadowFactor(const std::vector<Object>& objects,
                               TriangleData& triangle,
                               const Point4& barycentric_point,
                               const Lights& light_container);

  // Simple Lambert model
  ElemType ComputeLightning(const std::vector<Object>& objects,
                            TriangleData& triangle,
                            const Point4& barycentric_point,
                            const Lights& light_container) const;

private:
  // Required for shadow mapping
  LightCameras cameras_;
};

}  // namespace Rendering

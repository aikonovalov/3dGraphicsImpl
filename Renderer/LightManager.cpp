#include "LightManager.h"
#include <algorithm>

namespace Rendering {

LightCameras::LightCameras(const Point4& position)
    : x_plus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO),
      x_minus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO),
      y_plus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO),
      y_minus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO),
      z_plus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO),
      z_minus_dir(position, kDEFAULT_LIGHTCUBE_ASPECT_RATIO) {

  x_plus_dir.LookAtPoint({1, 0, 0, 0});
  x_minus_dir.LookAtPoint({-1, 0, 0, 0});
  y_plus_dir.LookAtPoint({0, 1, 0, 0});
  y_minus_dir.LookAtPoint({0, -1, 0, 0});
  z_plus_dir.LookAtPoint({0, 0, 1, 0});
  z_minus_dir.LookAtPoint({0, 0, -1, 0});
}

void LightCameras::SetPosition(const Point4& new_position) {
  x_plus_dir.SetPosition(new_position);
  x_minus_dir.SetPosition(new_position);
  y_plus_dir.SetPosition(new_position);
  y_minus_dir.SetPosition(new_position);
  z_plus_dir.SetPosition(new_position);
  z_minus_dir.SetPosition(new_position);
}

const Scene::Camera& LightCameras::GetVisibleCamera(const Point4& direction) {
  ElemType abs_x = std::abs(direction(0));
  ElemType abs_y = std::abs(direction(1));
  ElemType abs_z = std::abs(direction(2));

  if (abs_x >= abs_y && abs_x >= abs_z) {
    return (direction(0) > 0) ? x_plus_dir : x_minus_dir;
  } else if (abs_y >= abs_x && abs_y >= abs_z) {
    return (direction(1) > 0) ? y_plus_dir : y_minus_dir;
  } else {
    return (direction(2) > 0) ? z_plus_dir : z_minus_dir;
  }
}

LightManager::LightManager() : cameras_({0, 0, 0, 0}) {
}

Linear::ElemType LightManager::ComputeShadowMap(
    const std::vector<Object>& objects, const Point4& point,
    const Light& light) {
  ElemType depth = 1;
  return depth;
}

Linear::ElemType LightManager::ComputeShadowFactor(
    const std::vector<Object>& objects, TriangleData& triangle,
    const Point4& barycentric_point, const Lights& light_container) {
  ElemType shadow_factor = 1;
  return shadow_factor;
}

Linear::ElemType LightManager::ComputeLightning(
    const std::vector<Object>& objects, TriangleData& triangle,
    const Point4& barycentric_point, const Lights& light_container) const {

  Point4 real_point =
      triangle.vertices.GetPointByBarycentric(barycentric_point);
  Point4 normal = Linear::Normalize(
      triangle.normals.GetPointByBarycentric(barycentric_point));

  ElemType lightning_unit = 0.0;

  for (const auto& light : light_container) {
    lightning_unit += light.ambient;

    ElemType diffuse_intensity = std::max(
        0.0, Linear::DotProduct(normal, Linear::Normalize(light.position)));
    lightning_unit += diffuse_intensity * light.diffuse;
  }

  return lightning_unit;
}

}  // namespace Rendering

#pragma once

#include <cstdint>
#include "Palette.h"
#include "Texture.h"

namespace Detail {

struct Material {
  Linear::Vector4 ambient = {0.1f, 0.1f, 0.1f, 0.0f};
  Linear::Vector4 diffuse = {0.8f, 0.8f, 0.8f, 0.0f};
  Linear::Vector4 specular = {1.0f, 1.0f, 1.0f, 0.0f};

  float shininess = 32.0f;
  Detail::Texture texture;

  static constexpr Color kDEFAULT_BASE_COLOR = 0xFFFFFFFF;
  Color base_color = kDEFAULT_BASE_COLOR;
};

}  // namespace Detail

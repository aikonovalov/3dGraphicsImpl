#pragma once

#include <cstdint>
#include "Palette.h"

namespace Detail {

struct Material {
  using ELemType = Linear::ElemType;

  static constexpr Color kDEFAULT_AMBIENT_COLOR = 0xFFFFFFFF;
  static constexpr Color kDEFAULT_DIFFUSE_COLOR = 0xFFFFFFFF;
  static constexpr Color kDEFAULT_SPECULAR_COLOR = 0xFFFFFFFF;
  static constexpr ELemType kDEFAULT_SHININESS = 32.0;

  Color ambient_color = kDEFAULT_AMBIENT_COLOR;
  Color diffuse_color = kDEFAULT_DIFFUSE_COLOR;
  Color specular_color = kDEFAULT_SPECULAR_COLOR;
  ELemType shininess = kDEFAULT_SHININESS;
};

}  // namespace Detail

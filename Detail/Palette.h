#pragma once

#include <cstdint>
#include <vector>
#include "../MathUtils/Point4.h"

namespace Detail {

using Color = uint32_t;
using ZDepth = Linear::ElemType;
using ScreenPicture = std::vector<Color>;
using ZBuffer = std::vector<ZDepth>;

struct WindowSize {
  using Height = Linear::Detail::Height;
  using Width = Linear::Detail::Width;

  Height height;
  Width width;
};

struct ScreenPoint {
  using Height = Linear::Detail::Height;
  using Width = Linear::Detail::Width;
  using ElemType = Linear::ElemType;

  Height y;
  Width x;
  ElemType depth;

  Linear::Point4 normal;
};

}  // namespace Detail

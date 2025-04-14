#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
#include "Material.h"
#include "Palette.h"

namespace Detail {

class Texture {
  using ElemType = Linear::ElemType;
  using Vector4 = Linear::Vector4;
  using Colors = std::vector<Detail::Color>;

  using Height = Linear::Detail::Height;
  using Width = Linear::Detail::Width;

public:
  Texture() = default;

  Texture(const Colors& pixels, const Height& height, const Width& width)
      : texture_data_(pixels), width_(width), height_(height) {
  }

  Detail::Color Sample(const Vector4& texture_coords) const {
    ElemType u = texture_coords(0) - std::floor(texture_coords(0));
    ElemType v = texture_coords(1) - std::floor(texture_coords(1));

    Width x = Width{std::min(static_cast<int>(u * width_), width_ - 1)};
    Height y = Height{std::min(static_cast<int>(v * height_), height_ - 1)};

    return texture_data_[y * width_ + x];
  }

  Width GetWidth() const {
    return width_;
  }
  Height GetHeight() const {
    return height_;
  }

private:
  Colors texture_data_;
  Height height_;
  Width width_;
};

}  // namespace Detail

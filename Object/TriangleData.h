#pragma once

#include <array>
#include "../Detail/Material.h"
#include "../Detail/Texture.h"
#include "../MathUtils/Triangle.h"

namespace Scene {

struct TriangleData {
  Linear::Triangle vertices;
  Linear::Triangle normals;
  Linear::Triangle texture_coords;

  Detail::Material material;
  Detail::Texture texture;

  TriangleData(const Linear::Triangle& vertices,
               const Linear::Triangle& normals,
               const Linear::Triangle& texture_coords,
               const Detail::Material& material = {},
               const Detail::Texture& texture = {})
      : vertices(vertices),
        normals(normals),
        texture_coords(texture_coords),
        material(material),
        texture(texture) {
  }
};

}  // namespace Scene

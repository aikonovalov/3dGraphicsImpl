#pragma once

#include <array>
#include "../Detail/Material.h"
#include "../Detail/Texture.h"
#include "../MathUtils/Triangle.h"

namespace Scene {

struct TriangleData {
  using Triangle = Linear::Triangle;
  using Index = Linear::Index;

  Triangle vertices;
  Triangle normals;
  Triangle texture_coords;

  Index material_index = -1;

  TriangleData(const Triangle& vertices, const Triangle& normals,
               const Triangle& texture_coords, const Index material_index = -1)
      : vertices(vertices),
        normals(normals),
        texture_coords(texture_coords),
        material_index(material_index) {
  }
};

}  // namespace Scene

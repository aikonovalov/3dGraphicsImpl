#pragma once

#include <vector>
#include "../MathUtils/Triangle.h"

namespace Scene {

class Object {
  using Triangle = Linear::Triangle;
  using Point4 = Linear::Point4;
  using Index = Linear::Index;

public:
  Object() = default;
  Object(std::vector<Triangle>&& triangles);

  Index GetTrianglesCount() const;

  Triangle GetTriangle(Index index) const;
  void SetTriangle(Index index, const Triangle& new_triangle);

  Point4 GetPosition() const;
  void SetPosition(const Point4& new_position);

private:
  static inline const Point4 kDEFAULT_POSITION = {0, 0, 0, 1};

  Point4 position_ = kDEFAULT_POSITION;
  std::vector<Triangle> triangles_;
};

}  // namespace Scene

#pragma once

#include <vector>
#include "TriangleData.h"

namespace Scene {

class Object {
  using TriangleDatas = std::vector<TriangleData>;
  using Material = Detail::Material;
  using Materials = std::vector<Detail::Material>;
  using Point4 = Linear::Point4;
  using Index = Linear::Index;

public:
  Object() = default;
  Object(TriangleDatas&& triangles, Materials&& materials);

  Index GetTrianglesCount() const;

  TriangleData& operator()(Index index);

  const Materials& GetMaterials() const;
  const Material* GetMaterial(Index index) const;

  Point4 GetPosition() const;
  void SetPosition(const Point4& new_position);

private:
  static inline const Point4 kDEFAULT_POSITION = {0, 0, 0, 1};

  Point4 position_ = kDEFAULT_POSITION;
  TriangleDatas triangles_;
  Materials materials_;
};

}  // namespace Scene

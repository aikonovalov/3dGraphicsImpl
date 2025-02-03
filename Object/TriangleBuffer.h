#pragma once

#include "../MathUtils/Triangle.h"

namespace Scene {
class TriangleBuffer {
public:
  TriangleBuffer();

  TriangleBuffer(const std::vector<Linear::Triangle>&);

  TriangleBuffer(const TriangleBuffer&);
  TriangleBuffer(TriangleBuffer&&) noexcept;
  TriangleBuffer& operator=(const TriangleBuffer&);
  TriangleBuffer& operator=(TriangleBuffer&&) noexcept;

  ~TriangleBuffer();

  size_t GetTrianglesCount() const;

  Linear::Triangle& GetTriangle(size_t);

  void Swap(TriangleBuffer&) noexcept;

private:
  size_t triangles_cnt_;

  std::unique_ptr<char[]> triangle_holder_;
};

}  // namespace Scene

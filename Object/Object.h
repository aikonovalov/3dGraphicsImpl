#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "TriangleBuffer.h"

namespace Scene {

class Object {
  using ElemType = double;

public:
  Object();

  Object(const Object&);
  Object(Object&&) noexcept;
  Object& operator=(const Object&);
  Object& operator=(Object&&) noexcept;

  Object(const std::vector<Linear::Triangle>&);

  ~Object();

  size_t GetTrianglesCount() const;

  Linear::Triangle& GetTriangle(size_t);

private:
  TriangleBuffer buffer;
};

// class ObjectAlt {
//   using ElemType = double;

// public:
//   ObjectAlt();
//   ObjectAlt(const std::vector<Linear::Triangle>&);

//   ~ObjectAlt();

//   Linear::Triangle& GetTriangle(size_t);

// private:
//   size_t triangles_cnt_;
//   std::vector<Linear::Triangle> triangle_holder_;
// };

}  // namespace Scene
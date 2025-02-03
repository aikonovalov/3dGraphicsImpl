#include "Object.h"
#include <cstddef>
#include <cstring>
#include <memory>
#include "TriangleBuffer.h"

namespace Scene {

Object::Object() : buffer(){};

Object::Object(const Object& other) : buffer(other.buffer) {}

Object::Object(Object&& other) noexcept : buffer(other.buffer) {}

Object::Object(const std::vector<Linear::Triangle>& triangles)
    : buffer(triangles) {}

Object::~Object() = default;

size_t Object::GetTrianglesCount() const {
  return buffer.GetTrianglesCount();
}

Linear::Triangle& Object::GetTriangle(size_t index) {
  return buffer.GetTriangle(index);
}

// ObjectAlt::ObjectAlt() : triangles_cnt_(0) {}

// ObjectAlt::ObjectAlt(const std::vector<Linear::Triangle>& triangles)
//     : triangles_cnt_(triangles.size()), triangle_holder_(triangles) {}

// ObjectAlt::~ObjectAlt() = default;

// Linear::Triangle& ObjectAlt::GetTriangle(size_t index) {
//   if (index >= triangles_cnt_) {
//     throw std::invalid_argument("Triangle index is invalid");
//   }
//   return triangle_holder_[index];
// }

}  // namespace Scene

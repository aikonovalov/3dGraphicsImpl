#include "Object.h"

namespace Scene {

Object::Object(std::vector<Linear::Triangle>&& triangles)
    : triangles_(std::move(triangles)) {
}

Linear::Index Object::GetTrianglesCount() const {
  return triangles_.size();
}

Linear::Triangle Object::GetTriangle(Index index) const {
  assert(index >= 0 && index < triangles_.size() && "Invalid index");
  return triangles_[index];
}
void Object::SetTriangle(Linear::Index index,
                         const Linear::Triangle& new_triangle) {
  assert(index >= 0 && index < triangles_.size() && "Invalid index");
  triangles_[index] = new_triangle;
}

Linear::Point4 Object::GetPosition() const {
  return position_;
}

void Object::SetPosition(const Linear::Point4& new_position) {
  position_ = new_position;
}

}  // namespace Scene

#include "Object.h"
#include "TriangleData.h"

namespace Scene {

Object::Object(TriangleDatas&& triangles) : triangles_(std::move(triangles)) {
}

Linear::Index Object::GetTrianglesCount() const {
  return triangles_.size();
}

TriangleData& Object::operator()(Linear::Index index) {
  assert(index >= 0 && index < triangles_.size() && "Invalid index");
  return triangles_[index];
}

Linear::Point4 Object::GetPosition() const {
  return position_;
}

void Object::SetPosition(const Linear::Point4& new_position) {
  position_ = new_position;
}

}  // namespace Scene

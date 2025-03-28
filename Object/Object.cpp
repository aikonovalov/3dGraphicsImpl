#include "Object.h"
#include <cassert>
#include <vector>
#include "TriangleData.h"

namespace Scene {

Object::Object(TriangleDatas&& triangles, Materials&& materials)
    : triangles_(std::move(triangles)), materials_(std::move(materials)) {
}

Linear::Index Object::GetTrianglesCount() const {
  return triangles_.size();
}

TriangleData& Object::operator()(Linear::Index index) {
  assert(index >= 0 && index < triangles_.size() && "Invalid index");
  return triangles_[index];
}

const std::vector<Detail::Material>& Object::GetMaterials() const {
  return materials_;
}

const Detail::Material* Object::GetMaterial(Index index) const {
  if (index < 0 || index >= materials_.size()) {
    return nullptr;
  }
  return &materials_[index];
}

Linear::Point4 Object::GetPosition() const {
  return position_;
}

void Object::SetPosition(const Linear::Point4& new_position) {
  position_ = new_position;
}

}  // namespace Scene

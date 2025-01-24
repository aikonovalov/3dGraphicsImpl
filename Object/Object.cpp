#include "Object.h"
#include <cstring>
#include <memory>

namespace Scene {

Object::Object() : triangles_cnt_(0), triangle_holder_(nullptr) {}

Object::Object(const Object& other) : triangles_cnt_(other.triangles_cnt_) {
  triangle_holder_ =
      std::make_unique<char[]>(triangles_cnt_ * sizeof(Linear::Triangle));
  std::memcpy(triangle_holder_.get(), other.triangle_holder_.get(),
              triangles_cnt_ * sizeof(Linear::Triangle));
};

Object::Object(Object&& other) noexcept
    : triangles_cnt_(other.triangles_cnt_),
      triangle_holder_(std::move(other.triangle_holder_)) {
  other.triangles_cnt_ = 0;
}

Object::Object(const std::vector<Linear::Triangle>& triangles)
    : triangles_cnt_(triangles.size()),
      triangle_holder_(
          std::make_unique<char[]>(triangles_cnt_ * sizeof(Linear::Triangle))) {
  for (size_t i = 0; i < triangles_cnt_; ++i) {
    new (triangle_holder_.get() + i * sizeof(Linear::Triangle))
        Linear::Triangle(triangles[i]);
  }
}

Object::~Object() {
  for (size_t i = 0; i < triangles_cnt_; ++i) {
    reinterpret_cast<Linear::Triangle*>(triangle_holder_.get() +
                                        i * sizeof(Linear::Triangle))
        ->~Triangle();
  }
}

Linear::Triangle& Object::GetTriangle(size_t index) {
  if (index >= triangles_cnt_) {
    throw std::invalid_argument("Triangle index is invalid");
  }
  Linear::Triangle* triangle =
      reinterpret_cast<Linear::Triangle*>(triangle_holder_.get()) + index;
  return *triangle;
}

ObjectAlt::ObjectAlt() : triangles_cnt_(0) {}

ObjectAlt::ObjectAlt(const std::vector<Linear::Triangle>& triangles)
    : triangles_cnt_(triangles.size()), triangle_holder_(triangles) {}

ObjectAlt::~ObjectAlt() = default;

Linear::Triangle& ObjectAlt::GetTriangle(size_t index) {
  if (index >= triangles_cnt_) {
    throw std::invalid_argument("Triangle index is invalid");
  }
  return triangle_holder_[index];
}

}  // namespace Scene
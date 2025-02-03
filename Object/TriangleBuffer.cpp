#include "TriangleBuffer.h"
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace Scene {

TriangleBuffer::TriangleBuffer()
    : triangles_cnt_(0), triangle_holder_(nullptr) {}

TriangleBuffer::TriangleBuffer(const std::vector<Linear::Triangle>& triangles)
    : triangles_cnt_(triangles.size()),
      triangle_holder_(
          std::make_unique<char[]>(triangles_cnt_ * sizeof(Linear::Triangle))) {
  for (size_t i = 0; i < triangles_cnt_; ++i) {
    new (triangle_holder_.get() + i * sizeof(Linear::Triangle))
        Linear::Triangle(triangles[i]);
  }
}

TriangleBuffer::TriangleBuffer(const TriangleBuffer& other)
    : triangles_cnt_(other.triangles_cnt_) {
  triangle_holder_ =
      std::make_unique<char[]>(triangles_cnt_ * sizeof(Linear::Triangle));
  std::memcpy(triangle_holder_.get(), other.triangle_holder_.get(),
              triangles_cnt_ * sizeof(Linear::Triangle));
}

TriangleBuffer::TriangleBuffer(TriangleBuffer&& other) noexcept
    : triangles_cnt_(other.triangles_cnt_),
      triangle_holder_(std::move(other.triangle_holder_)) {
  other.triangles_cnt_ = 0;
}

TriangleBuffer& TriangleBuffer::operator=(const TriangleBuffer& other) {
  TriangleBuffer tmp;
  Swap(tmp);
  triangle_holder_ =
      std::make_unique<char[]>(triangles_cnt_ * sizeof(Linear::Triangle));
  std::memcpy(triangle_holder_.get(), other.triangle_holder_.get(),
              triangles_cnt_ * sizeof(Linear::Triangle));

  return *this;
}

TriangleBuffer& TriangleBuffer::operator=(TriangleBuffer&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  Swap(other);
  return *this;
}

TriangleBuffer::~TriangleBuffer() {
  for (size_t i = 0; i < triangles_cnt_; ++i) {
    reinterpret_cast<Linear::Triangle*>(triangle_holder_.get() +
                                        i * sizeof(Linear::Triangle))
        ->~Triangle();
  }
}

size_t TriangleBuffer::GetTrianglesCount() const {
  return this->triangles_cnt_;
}

Linear::Triangle& TriangleBuffer::GetTriangle(size_t index) {
  assert(index < triangles_cnt_);
  Linear::Triangle* triangle =
      reinterpret_cast<Linear::Triangle*>(triangle_holder_.get()) + index;
  return *triangle;
}

void TriangleBuffer::Swap(TriangleBuffer& other) noexcept {
  this->triangle_holder_.swap(other.triangle_holder_);
  std::swap(this->triangles_cnt_, other.triangles_cnt_);
}

}  // namespace Scene

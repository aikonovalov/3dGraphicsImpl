#include "Plane.h"
#include <cstddef>
#include <optional>
#include "Point4.h"

namespace Linear {

Plane::Plane() : normal_(0, 0, 0, 1), shift_(ElemType()) {
}

Plane::Plane(const Point4& normal, ElemType shift = ElemType())
    : normal_(normal), shift_(shift) {
}

Plane::Plane(const Point4& point1, const Point4& point2, const Point4& point3) {
  Point4 v1 = point2 - point1;
  Point4 v2 = point3 - point1;
  normal_ = Normalize(CrossProduct(v1, v2));
  shift_ = -DotProduct(point1, normal_);
}

Plane::ElemType Plane::GetDistance(const Point4& point) {
  return DotProduct(normal_, point) + shift_;
}

std::optional<Point4> Plane::GetIntersectWithVector(
    const OffsetedVector& vector) {
  ElemType dist1 = this->GetDistance(vector.begin);
  ElemType dist2 = this->GetDistance(vector.end);

  if (dist1 * dist2 > 0) {
    return std::nullopt;
  }
  ElemType lambda = dist1 / (dist1 - dist2);
  Vector4 result = vector.begin + (vector.end - vector.begin) * lambda;
  return result;
}

void Plane::ClipThrough(std::queue<Linear::Triangle>& clip_pool) {
  //  The method accepts a std::queue of triangles and performs clipping through the plane.
  //  The plane divides space into two half-spaces: S+ (where the dot product of any vector with the plane normal
  //  is non-negative) and S– (where it is negative). The method processes all triangles
  //  that were in the queue before this method was launched. For each triangle:
  //
  //  - If the triangle lies completely in S–, it is skipped.
  //  - If the triangle lies completely in S+, it is appended to the end of the queue.
  //  - If the triangle intersects the plane, it is clipped into one or more sub-triangles,
  //    and those sub-triangles that lie in S+ are appended to the end of the queue.
  size_t pool_size = clip_pool.size();
  for (size_t i = 0; i < pool_size; ++i) {
    Linear::Triangle curr = clip_pool.front();
    clip_pool.pop();
    ElemType dist0 = GetDistance(curr.GetPoint(0));
    ElemType dist1 = GetDistance(curr.GetPoint(1));
    ElemType dist2 = GetDistance(curr.GetPoint(2));

    if (dist0 <= 0 && dist1 <= 0 && dist2 <= 0) {
      continue;
    } else if (dist0 >= 0 && dist1 >= 0 && dist2 >= 0) {
      clip_pool.push(curr);
    }

    Point4 intersect_01, intersect_12, intersect_20;
    intersect_01 = GetIntersectWithVector({curr.GetPoint(0), curr.GetPoint(1)});
    intersect_12 = GetIntersectWithVector({curr.GetPoint(1), curr.GetPoint(2)});
    intersect_20 = GetIntersectWithVector({curr.GetPoint(2), curr.GetPoint(0)});

    if (dist0 <= 0) {
      if (dist1 <= 0) {
        clip_pool.emplace(intersect_20, intersect_12, curr.GetPoint(2));
      } else if (dist2 <= 0) {
        // dist1 > 0
        clip_pool.emplace(intersect_01, intersect_12, curr.GetPoint(2));
      } else {
        // dist1 > 0 && dist2 > 0
        clip_pool.emplace(intersect_01, curr.GetPoint(1), curr.GetPoint(2));
        clip_pool.emplace(intersect_01, curr.GetPoint(2), intersect_20);
      }
    } else if (dist1 <= 0) {
      // dist0 > 0
      if (dist2 <= 0) {
        clip_pool.emplace(curr.GetPoint(0), intersect_01, intersect_20);
      } else {
        clip_pool.emplace(curr.GetPoint(0), intersect_01, intersect_12);
        clip_pool.emplace(curr.GetPoint(0), intersect_12, curr.GetPoint(2));
      }
    } else {
      // dist0 > 0 && dist1 > 0
      clip_pool.emplace(curr.GetPoint(0), curr.GetPoint(1), intersect_12);
      clip_pool.emplace(curr.GetPoint(0), intersect_12, intersect_20);
    }
  }
}

void Plane::Transform(const TransformMatrix4x4& transform_matrix,
                      ElemType new_shift) {
  normal_ = transform_matrix * normal_;
  shift_ = new_shift;
}

}  // namespace Linear

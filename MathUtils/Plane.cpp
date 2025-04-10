#include "Plane.h"
#include <cstddef>
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

IntersectionResult Plane::GetIntersectWithVector(const OffsetedVector& vector) {
  ElemType dist1 = this->GetDistance(vector.begin);
  ElemType dist2 = this->GetDistance(vector.end);

  if (dist1 * dist2 > 0) {
    return IntersectionResult{.has_intersection = false};
  }
  ElemType lambda = dist1 / (dist1 - dist2);
  Vector4 result = vector.begin + (vector.end - vector.begin) * lambda;
  return IntersectionResult{.has_intersection = true, .intersection = result};
}

bool Plane::IsTriangleFaceTo(const Triangle& triangle) {
  return DotProduct(triangle.GetNormal(), normal_) <= -kEPS;
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

    ElemType dist0 = GetDistance(curr(0));
    ElemType dist1 = GetDistance(curr(1));
    ElemType dist2 = GetDistance(curr(2));

    if (dist0 <= 0 && dist1 <= 0 && dist2 <= 0) {
      continue;
    } else if (dist0 >= 0 && dist1 >= 0 && dist2 >= 0) {
      clip_pool.push(curr);
      continue;
    }

    IntersectionResult intersect_01 =
        GetIntersectWithVector({curr(0), curr(1)});
    IntersectionResult intersect_12 =
        GetIntersectWithVector({curr(1), curr(2)});
    IntersectionResult intersect_20 =
        GetIntersectWithVector({curr(2), curr(0)});

    if (dist0 <= 0) {
      if (dist1 <= 0) {
        // Vertices 0 and 1 outside of S+, only 2 in S+
        clip_pool.emplace(intersect_20.intersection, intersect_12.intersection,
                          curr(2));
      } else if (dist2 <= 0) {
        // Vertices 0 and 2 outside of S+, only 1 in S+
        clip_pool.emplace(intersect_01.intersection, intersect_12.intersection,
                          curr(2));
      } else {
        // Vertex 0 is outside of S+, and 1 and 2 are in S+
        clip_pool.emplace(intersect_01.intersection, curr(1), curr(2));
        clip_pool.emplace(intersect_01.intersection, curr(2),
                          intersect_20.intersection);
      }
    } else if (dist1 <= 0) {
      // Vertex 1 is outside of S+, and 0 is in S+
      if (dist2 <= 0) {
        // Only 0 in S+
        clip_pool.emplace(curr(0), intersect_01.intersection,
                          intersect_20.intersection);
      } else {
        // Vertices 0 and 2 in S+, vertex 1 outside S+
        clip_pool.emplace(curr(0), intersect_01.intersection,
                          intersect_12.intersection);
        clip_pool.emplace(curr(0), intersect_12.intersection, curr(2));
      }
    } else {
      // Vertices 0 and 1 are in S+, and 2 are outside S+
      clip_pool.emplace(curr(0), curr(1), intersect_12.intersection);
      clip_pool.emplace(curr(0), intersect_12.intersection,
                        intersect_20.intersection);
    }
  }
}

void Plane::Transform(const TransformMatrix4x4& transform_matrix,
                      ElemType new_shift) {
  normal_ = transform_matrix * normal_;
  shift_ = new_shift;
}

}  // namespace Linear

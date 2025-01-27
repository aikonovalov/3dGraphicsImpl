#include "Plane.h"
#include <cstddef>
#include "Point3d.h"
#include "Triangle.h"

namespace Linear {

Plane::Plane() : normal_(0, 0, 1), shift_(ElemType()) {}

Plane::Plane(const Point3d& normal, ElemType shift = ElemType())
    : normal_(normal), shift_(shift) {}

Plane::Plane(const Point3d& p1, const Point3d& p2, const Point3d& p3) {
  auto v1 = p2 - p1;
  auto v2 = p3 - p1;
  normal_ = v1.Cross(v2).Normalize();
  shift_ = -p1.DotProd(normal_);
}

Plane::ElemType Plane::GetDistance(const Point3d& point) {
  return normal_.DotProd(point) + shift_;
}

bool Plane::GetIntersect(const std::pair<Point3d, Point3d>& vector,
                         Point3d& result) {
  ElemType dist1 = this->GetDistance(vector.first);
  ElemType dist2 = this->GetDistance(vector.second);

  if (dist1 * dist2 > 0) {
    return false;
  }
  ElemType lambda = dist1 / (dist1 - dist2);
  result = vector.first + (vector.second - vector.first) * lambda;

  return true;
}

void Plane::ClipThrough(std::queue<Linear::Triangle>& clip_pool) {
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

    std::pair<bool, Point3d> intersect_01;
    std::pair<bool, Point3d> intersect_12;
    std::pair<bool, Point3d> intersect_20;
    intersect_01.first =
        GetIntersect({curr.GetPoint(0), curr.GetPoint(1)}, intersect_01.second);
    intersect_12.first =
        GetIntersect({curr.GetPoint(1), curr.GetPoint(2)}, intersect_12.second);
    intersect_20.first =
        GetIntersect({curr.GetPoint(2), curr.GetPoint(0)}, intersect_20.second);

    // Important: keep the traversal order (clockwise or counterclockwise)
    if (dist0 <= 0) {
      if (dist1 <= 0) {
        clip_pool.push(Triangle(intersect_20.second, intersect_12.second,
                                curr.GetPoint(2)));
      } else if (dist2 <= 0) {
        // dist1 > 0
        clip_pool.push(Triangle(intersect_01.second, intersect_12.second,
                                curr.GetPoint(2)));
      } else {
        // dist1 > 0 && dist2 > 0
        clip_pool.push(
            Triangle(intersect_01.second, curr.GetPoint(1), curr.GetPoint(2)));
        clip_pool.push(Triangle(intersect_01.second, curr.GetPoint(2),
                                intersect_20.second));
      }
    } else if (dist1 <= 0) {
      // dist0 > 0
      if (dist2 <= 0) {
        clip_pool.push(Triangle(curr.GetPoint(0), intersect_01.second,
                                intersect_20.second));
      } else {
        clip_pool.push(Triangle(curr.GetPoint(0), intersect_01.second,
                                intersect_12.second));
        clip_pool.push(
            Triangle(curr.GetPoint(0), intersect_12.second, curr.GetPoint(2)));
      }
    } else {
      // dist0 > 0 && dist1 > 0
      clip_pool.push(
          Triangle(curr.GetPoint(0), curr.GetPoint(1), intersect_12.second));
      clip_pool.push(
          Triangle(curr.GetPoint(0), intersect_12.second, intersect_20.second));
    }
  }
}

void Plane::Rotate(const Matrix<4, 4>& rotation) {
  normal_.Transform(rotation);
}

}  // namespace Linear
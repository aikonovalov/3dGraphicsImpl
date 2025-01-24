#include "Plane.h"

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

void Plane::Rotate(const Matrix<4, 4>& rotation) {
  normal_.Transform(rotation);
}

}  // namespace Linear
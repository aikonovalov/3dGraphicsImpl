#pragma once

#include "Point3d.h"

namespace Linear {

template <typename ElemType,
          typename = typename std::enable_if<
              std::is_arithmetic<ElemType>::value, ElemType>::type>
class Plane {

public:
  Plane() : normal_(0, 0, 1), shift_(ElemType()) {}
  Plane(const Point3d<ElemType>& normal, ElemType shift = ElemType())
      : normal_(normal), shift_(shift) {}
  Plane(const Point3d<ElemType>& p1, const Point3d<ElemType>& p2,
        const Point3d<ElemType>& p3) {
    auto v1 = p2 - p1;
    auto v2 = p3 - p1;
    normal_ = v1.Cross(v2).Normalize();
    shift_ = -p1.DotProd(normal_);
  }

  ElemType GetDistance(const Point3d<ElemType>& point) {
    return normal_.DotProd(point) + shift_;
  }

  bool GetIntersect(
      const std::pair<Point3d<ElemType>, Point3d<ElemType>>& vector,
      Point3d<ElemType>& result) {
    ElemType dist1 = this->GetDistance(vector.first);
    ElemType dist2 = this->GetDistance(vector.second);

    if (dist1 * dist2 > 0) {
      return false;
    }
    ElemType lambda = dist1 / (dist1 - dist2);
    result = vector.first + (vector.second - vector.first) * lambda;

    return true;
  }

private:
  Point3d<ElemType> normal_;
  ElemType shift_;
};

}  // namespace Linear
#pragma once

#include <queue>
#include "Triangle.h"

namespace Linear {

class Plane {

public:
  using ElemType = Point3d::ElemType;

  Plane();
  Plane(const Point3d&, ElemType);
  Plane(const Point3d&, const Point3d&, const Point3d&);

  ElemType GetDistance(const Point3d&);
  bool GetIntersect(const std::pair<Point3d, Point3d>&, Point3d&);

  void ClipThrough(std::queue<Linear::Triangle>&);

  void Rotate(const Matrix<4, 4>&);

private:
  Point3d normal_;
  ElemType shift_;
};

}  // namespace Linear

#pragma once

#include <array>
#include <stdexcept>
#include "Matrix.h"
#include "Point4.h"

namespace Linear {

class Triangle {
public:
  Triangle(const Point4&, const Point4&, const Point4&);
  Triangle(const Matrix<Detail::Height{4}, Detail::Width{3}>&);

  Point4& operator()(Index index);

  Point4 operator()(Index index) const;

  Point4 GetNormal() const;

  Point4 GetPointByBarycentric(const Point4& barycentric_point);

  ElemType GetAreaXYProjection() const;

  void OffsetCoords(const Point4& offset);

  void Transform(const TransformMatrix4x4&);

private:
  bool CollinearCheck(const Point4& point1, const Point4& point2,
                      const Point4& point3) const {
    Vector4 edge1 = point2 - point1;
    Vector4 edge2 = point3 - point1;
    return !IsEmpty(CrossProduct(edge1, edge2));
  }

  std::array<Point4, 3> points_;
};

}  // namespace Linear

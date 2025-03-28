#pragma once

#include "Matrix.h"

namespace Linear {

using Vector4 = Matrix<Detail::Height{4}, Detail::Width{1}>;
using Point4 = Vector4;
using TransformMatrix4x4 = Matrix<Detail::Height{4}, Detail::Width{4}>;

bool IsEmpty(const Point4&);

Point4 CrossProduct(const Point4&, const Point4&);

Point4 Normalize(const Point4&);

ElemType DotProduct(const Point4&, const Point4&);

Point4 Transform(const TransformMatrix4x4&, const Point4&);

}  // namespace Linear

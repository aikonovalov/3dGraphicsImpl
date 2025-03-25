#pragma once

#include "Matrix.h"

namespace Linear {

using Height = Detail::Height;
using Width = Detail::Width;
using ElemType = Matrix<Height{4}, Width{1}>::ElemType;

using Vector4 = Matrix<Height{4}, Width{1}>;
using Point4 = Vector4;

Point4 CrossProduct(const Point4&);

Point4 Normalize(const Point4&);

ElemType DotProduct(const Point4&);

Point4 Transform(const Matrix<Height{4}, Width{4}>&, const Point4&);

}  // namespace Linear

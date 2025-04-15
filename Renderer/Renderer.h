#pragma once

#include <queue>
#include <vector>
#include "../Detail/Palette.h"
#include "../MathUtils/Plane.h"
#include "../Object/Camera.h"
#include "../Object/Object.h"

namespace Core {

using windowHeight = Linear::Detail::Height;
using windowWidth = Linear::Detail::Width;

class Model;

}  // namespace Core

namespace Rendering {

static constexpr double kDEFAULT_AMBIENT = 0.2;
static constexpr double kDEFAULT_DIFFUSE = 0.8;

class Renderer {
  using ElemType = Linear::ElemType;
  using Point4 = Linear::Point4;
  using Triangle = Linear::Triangle;

  using Plane = Linear::Plane;
  using IntersectionResult = Linear::IntersectionResult;

  using Camera = Scene::Camera;
  using Object = Scene::Object;

  using TriangleData = Scene::TriangleData;
  using OffsetedVector = Linear::OffsetedVector;
  using Color = Detail::Color;
  using ZDepth = Detail::ZDepth;
  using ScreenPicture = Detail::ScreenPicture;
  using ZBuffer = Detail::ZBuffer;
  using ScreenPoint = Detail::ScreenPoint;

  using Height = Linear::Detail::Height;
  using Width = Linear::Detail::Width;
  using Index = Linear::Index;

  using WindowSize = Detail::WindowSize;
  using Lights = Detail::Lights;

public:
  void CameraRatioCheck(Camera& camera, WindowSize window_size);

  Point4 ComputeBarycentric(const Point4& point, const TriangleData& triangle,
                            const ElemType& triangle_area);

  ElemType ComputeLighting(const Point4& normal, const Point4& light_dir,
                           ElemType ambient = kDEFAULT_AMBIENT,
                           ElemType diffuse = kDEFAULT_DIFFUSE);

  void ClipTrianglesThroughPlane(const Plane& plane,
                                 std::queue<TriangleData>& triangle_pool);

  void DrawPixel(const WindowSize& window_size, ScreenPicture& pixels,
                 ZBuffer& z_buffer, const ScreenPoint& location, Color color);

  void DrawLine(const ScreenPoint& begin, const ScreenPoint& end,
                const WindowSize& window_size, ScreenPicture& pixels,
                ZBuffer& z_buffer, Color color);

  void DrawBorder(const TriangleData& triangle, const WindowSize& window_size,
                  ScreenPicture& pixels, ZBuffer& z_buffer, Color color);

  void RasterizeTriangle(TriangleData& triangle, WindowSize window_size,
                         ScreenPicture& pixels, ZBuffer& z_buffer);

  ScreenPicture RenderScene(const std::vector<Object>& objects, Camera& camera,
                            const Lights& lights, WindowSize window_size);

private:
  static constexpr ElemType kEPS = 1e-6;
  static constexpr Color kBORDER_COLOR = 0x008000;
  static constexpr ZDepth kMAX_Z_DEPTH = 0xFFFFFF;

  OffsetedVector GetBoundingBoxBorders(const TriangleData& triangle,
                                       WindowSize window_size);

  Width ConvertToScreenX(WindowSize window_size, const Point4& point);
  Height ConvertToScreenY(WindowSize window_size, const Point4& point);
};

}  // namespace Rendering

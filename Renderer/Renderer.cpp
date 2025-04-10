#include "Renderer.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace Rendering {

void Renderer::CameraRatioCheck(Camera& camera, WindowSize window_size) {
  double aspect_ratio = static_cast<double>(window_size.width) /
                        static_cast<double>(window_size.height);
  if (camera.GetAspectRatio() != aspect_ratio) {
    camera.SetAspectRatio(aspect_ratio);
  }
}

Linear::Point4 Renderer::ComputeBarycentric(const Point4& point,
                                            const Triangle& triangle,
                                            const ElemType& triangle_area) {
  ElemType area01 =
      Triangle{triangle(0), triangle(1), point}.GetAreaXYProjection();
  ElemType area12 =
      Triangle{triangle(1), triangle(2), point}.GetAreaXYProjection();
  ElemType area20 =
      Triangle{triangle(2), triangle(0), point}.GetAreaXYProjection();

  return {area01 / triangle_area, area12 / triangle_area,
          area20 / triangle_area, 0};
}

Linear::ElemType Renderer::ComputeDepth(const Triangle& triangle,
                                        const Point4& barycentric_point) {
  return triangle(0)(2) * barycentric_point(0) +
         triangle(1)(2) * barycentric_point(1) +
         triangle(2)(2) * barycentric_point(2);
}

double Renderer::ComputeLighting(const Point4& normal, const Point4& light_dir,
                                 double ambient, double diffuse) {
  double lambert = std::max(0.0, DotProduct(normal, light_dir));
  return ambient + diffuse * lambert;
}

void Renderer::DrawPixel(const WindowSize& window_size, ScreenPicture& pixels,
                         ZBuffer& z_buffer, const ScreenPoint& location,
                         Color color) {
  if (location.x >= 0 && location.x < window_size.width && location.y >= 0 &&
      location.y < window_size.height) {
    if (location.depth <=
        z_buffer[location.y * window_size.width + location.x]) {
      pixels[location.y * window_size.width + location.x] = color;
      z_buffer[location.y * window_size.width + location.x] = location.depth;
    }
  }
}

void Renderer::DrawLine(const ScreenPoint& begin, const ScreenPoint& end,
                        const WindowSize& window_size, ScreenPicture& pixels,
                        ZBuffer& z_buffer, Color color) {
  int curr_x = begin.x;
  int curr_y = begin.y;

  int delta_x = std::abs(end.x - begin.x);
  int delta_y = std::abs(end.y - begin.y);

  int steps = std::max(delta_x, delta_y);

  ElemType depth_begin = begin.depth;
  ElemType depth_end = end.depth;
  ElemType depth_delta = (steps > 0) ? (depth_end - depth_begin) / steps : 0.0;

  int direction_x = (begin.x < end.x) ? 1 : -1;
  int direction_y = (begin.y < end.y) ? 1 : -1;

  int error = delta_x - delta_y;
  int step_count = 0;

  DrawPixel(window_size, pixels, z_buffer,
            {Height{curr_y}, Width{curr_x}, depth_begin}, color);

  while (curr_x != end.x || curr_y != end.y) {
    int doubled_error = 2 * error;

    if (doubled_error + delta_y > 0) {
      error -= delta_y;
      curr_x += direction_x;
    }
    if (doubled_error - delta_x < 0) {
      error += delta_x;
      curr_y += direction_y;
    }

    step_count++;
    ElemType current_depth = depth_begin + depth_delta * step_count;
    DrawPixel(window_size, pixels, z_buffer,
              {Height{curr_y}, Width{curr_x}, current_depth}, color);
  }
}

void Renderer::DrawBorder(const Triangle& triangle,
                          const WindowSize& window_size, ScreenPicture& pixels,
                          ZBuffer& z_buffer, Color color) {
  DrawLine({.x = Width{int(triangle(0)(0))},
            .y = Height{int(triangle(0)(1))},
            .depth = triangle(0)(2)},
           {.x = Width{int(triangle(1)(0))},
            .y = Height{int(triangle(1)(1))},
            .depth = triangle(1)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);

  DrawLine({.x = Width{int(triangle(1)(0))},
            .y = Height{int(triangle(1)(1))},
            .depth = triangle(1)(2)},
           {.x = Width{int(triangle(2)(0))},
            .y = Height{int(triangle(2)(1))},
            .depth = triangle(2)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);

  DrawLine({.x = Width{int(triangle(2)(0))},
            .y = Height{int(triangle(2)(1))},
            .depth = triangle(2)(2)},
           {.x = Width{int(triangle(0)(0))},
            .y = Height{int(triangle(0)(1))},
            .depth = triangle(0)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);
}

void Renderer::RasterizeTriangle(Triangle& triangle, WindowSize window_size,
                                 ScreenPicture& pixels, ZBuffer& z_buffer) {
  for (Index i = 0; i < 3; ++i) {
    triangle(i) = triangle(i) * (1.0 / triangle(i)(3));
    triangle(i)(0) = ConvertToScreenX(window_size, triangle(i)(0));
    triangle(i)(1) = ConvertToScreenY(window_size, triangle(i)(1));
  }

  ElemType triangle_area = triangle.GetAreaXYProjection();
  if (std::abs(triangle_area) < kEPS) {
    // Triangle is degenerate
    return;
  }

  OffsetedVector bound_box_borders =
      GetBoundingBoxBorders(triangle, window_size);

  for (Index i = bound_box_borders.begin(1); i < bound_box_borders.end(1);
       ++i) {
    for (Index j = bound_box_borders.begin(0); j < bound_box_borders.end(0);
         ++j) {
      Point4 barycentric_point = ComputeBarycentric(
          {ElemType(j), ElemType(i), 0, 0}, triangle, triangle_area);
      if (barycentric_point(0) >= -kEPS && barycentric_point(1) >= -kEPS &&
          barycentric_point(2) >= -kEPS) {
        DrawPixel(
            window_size, pixels, z_buffer,
            {Height(i), Width(j), ComputeDepth(triangle, barycentric_point)},
            0xFFFFFF);
      }
    }
  }
}

Detail::ScreenPicture Renderer::RenderScene(const std::vector<Object>& objects,
                                            Camera& camera,
                                            WindowSize window_size) {
  CameraRatioCheck(camera, window_size);

  ScreenPicture pixels(window_size.width * window_size.height, 0x000000);
  ZBuffer z_buf(window_size.width * window_size.height, kMAX_Z_DEPTH);

  Scene::FrustumPlanes frustum_planes = camera.GetFrustumPlanes();

  for (auto object : objects) {
    // Clipping
    std::queue<Linear::Triangle> clipping_pool;
    for (auto index = 0; index < object.GetTrianglesCount(); ++index) {
      Triangle triangle = object.GetTriangle(index);
      triangle.OffsetCoords(camera.GetPosition() - object.GetPosition());
      // if (frustum_planes.near.IsTriangleFaceTo(triangle)) {
      clipping_pool.push(triangle);
      // }
    }

    frustum_planes.near.ClipThrough(clipping_pool);
    frustum_planes.far.ClipThrough(clipping_pool);

    frustum_planes.up.ClipThrough(clipping_pool);
    frustum_planes.down.ClipThrough(clipping_pool);

    frustum_planes.left.ClipThrough(clipping_pool);
    frustum_planes.right.ClipThrough(clipping_pool);

    // Frustum apply
    std::vector<Linear::Triangle> frustumed_triangles;
    while (!clipping_pool.empty()) {
      clipping_pool.front().Transform(camera.GetFullFrustumMatrix());
      frustumed_triangles.push_back(std::move(clipping_pool.front()));

      clipping_pool.pop();
    }

    for (auto& triangle : frustumed_triangles) {
      RasterizeTriangle(triangle, window_size, pixels, z_buf);
    }
    for (auto& triangle : frustumed_triangles) {
      DrawBorder(triangle, window_size, pixels, z_buf, kBORDER_COLOR);
    }
  }

  return pixels;
}

Linear::Detail::Width Renderer::ConvertToScreenX(WindowSize window_size,
                                                 const Point4& point) {
  return Width{
      static_cast<int>((point(0) + 1.0) * 0.5 * (window_size.width - 1))};
};

Linear::Detail::Height Renderer::ConvertToScreenY(WindowSize window_size,
                                                  const Point4& point) {
  return Height{static_cast<int>((1.0 - (point(1) + 1.0) * 0.5) *
                                 (window_size.height - 1))};
};

Linear::OffsetedVector Renderer::GetBoundingBoxBorders(const Triangle& triangle,
                                                       WindowSize window_size) {
  Point4 begin{
      std::max(0.0, std::min({triangle(0)(0), triangle(1)(0), triangle(2)(0)})),
      std::max(0.0, std::min({triangle(0)(1), triangle(1)(1), triangle(2)(1)})),
      0, 0};
  Point4 end{
      std::min(window_size.width - 1.0,
               std::max({triangle(0)(0), triangle(1)(0), triangle(2)(0)})),
      std::min(window_size.height - 1.0,
               std::max({triangle(0)(1), triangle(1)(1), triangle(2)(1)})),
      0, 0};
  return {begin, end};
}

}  // namespace Rendering

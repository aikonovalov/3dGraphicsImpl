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
                                            const TriangleData& triangle_data,
                                            const ElemType& triangle_area) {
  ElemType area01 =
      Triangle{triangle_data.vertices(0), triangle_data.vertices(1), point}
          .GetAreaXYProjection();
  ElemType area12 =
      Triangle{triangle_data.vertices(1), triangle_data.vertices(2), point}
          .GetAreaXYProjection();
  ElemType area20 =
      Triangle{triangle_data.vertices(2), triangle_data.vertices(0), point}
          .GetAreaXYProjection();

  return {area01 / triangle_area, area12 / triangle_area,
          area20 / triangle_area, 0};
}

Linear::ElemType Renderer::ComputeDepth(const TriangleData& triangle_data,
                                        const Point4& barycentric_point) {
  return triangle_data.vertices(0)(2) * barycentric_point(0) +
         triangle_data.vertices(1)(2) * barycentric_point(1) +
         triangle_data.vertices(2)(2) * barycentric_point(2);
}

double Renderer::ComputeLighting(const Point4& normal, const Point4& light_dir,
                                 double ambient, double diffuse) {
  double lambert = std::max(0.0, DotProduct(normal, light_dir));
  return ambient + diffuse * lambert;
}

void Renderer::ClipThrough(const Plane& plane,
                           std::queue<TriangleData>& clip_pool) {
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
    TriangleData curr = clip_pool.front();
    clip_pool.pop();

    ElemType dist0 = plane.GetDistance(curr.vertices(0));
    ElemType dist1 = plane.GetDistance(curr.vertices(1));
    ElemType dist2 = plane.GetDistance(curr.vertices(2));

    if (dist0 <= 0 && dist1 <= 0 && dist2 <= 0) {
      continue;
    } else if (dist0 >= 0 && dist1 >= 0 && dist2 >= 0) {
      clip_pool.push(curr);
      continue;
    }

    auto interpolate = [](const Point4& a, const Point4& b, ElemType t) {
      return a + (b - a) * t;
    };

    auto interpolate_attr = [&](const Linear::Triangle& attr, int i0, int i1,
                                ElemType t) {
      return interpolate(attr(i0), attr(i1), t);
    };

    auto intersect = [&](int begin_vertex_index, int end_vertex_index) {
      OffsetedVector vec{curr.vertices(begin_vertex_index),
                         curr.vertices(end_vertex_index)};
      IntersectionResult intersection_result =
          plane.GetIntersectWithVector(vec);
      ElemType dist_begin = plane.GetDistance(vec.begin);
      ElemType dist_end = plane.GetDistance(vec.end);
      ElemType t = dist_begin / (dist_begin - dist_end);
      return std::make_tuple(
          intersection_result.intersection,
          interpolate_attr(curr.normals, begin_vertex_index, end_vertex_index,
                           t),
          interpolate_attr(curr.texture_coords, begin_vertex_index,
                           end_vertex_index, t));
    };

    auto [intersect_01, normal_01, texture_coords_01] = intersect(0, 1);
    auto [intersect_12, normal_12, texture_coords_12] = intersect(1, 2);
    auto [intersect_20, normal_20, texture_coords_20] = intersect(2, 0);

    if (dist0 <= 0) {
      if (dist1 <= 0) {
        // Vertices 0 and 1 outside of S+, only 2 in S+
        clip_pool.push(
            {{intersect_20, intersect_12, curr.vertices(2)},
             {normal_20, normal_12, curr.normals(2)},
             {texture_coords_20, texture_coords_12, curr.texture_coords(2)},
             curr.material,
             curr.texture});
      } else if (dist2 <= 0) {
        // Vertices 0 and 2 outside of S+, only 1 in S+
        clip_pool.push(
            {{intersect_01, intersect_12, curr.vertices(2)},
             {normal_01, normal_12, curr.normals(2)},
             {texture_coords_01, texture_coords_12, curr.texture_coords(2)},
             curr.material,
             curr.texture});
      } else {
        // Vertex 0 is outside of S+, and 1 and 2 are in S+
        clip_pool.push({{intersect_01, curr.vertices(1), curr.vertices(2)},
                        {normal_01, curr.normals(1), curr.normals(2)},
                        {texture_coords_01, curr.texture_coords(1),
                         curr.texture_coords(2)},
                        curr.material,
                        curr.texture});
        clip_pool.push(
            {{intersect_01, curr.vertices(2), intersect_20},
             {normal_01, curr.normals(2), normal_20},
             {texture_coords_01, curr.texture_coords(2), texture_coords_20},
             curr.material,
             curr.texture});
      }
    } else if (dist1 <= 0) {
      // Vertex 1 is outside of S+, and 0 is in S+
      if (dist2 <= 0) {
        // Only 0 in S+
        clip_pool.push(
            {{curr.vertices(0), intersect_01, intersect_20},
             {curr.normals(0), normal_01, normal_20},
             {curr.texture_coords(0), texture_coords_01, texture_coords_20},
             curr.material,
             curr.texture});
      } else {
        // Vertices 0 and 2 in S+, vertex 1 outside S+
        clip_pool.push(
            {{curr.vertices(0), intersect_01, intersect_12},
             {curr.normals(0), normal_01, normal_12},
             {curr.texture_coords(0), texture_coords_01, texture_coords_12},
             curr.material,
             curr.texture});
        clip_pool.push({{curr.vertices(0), intersect_12, curr.vertices(2)},
                        {curr.normals(0), normal_12, curr.normals(2)},
                        {curr.texture_coords(0), texture_coords_12,
                         curr.texture_coords(2)},
                        curr.material,
                        curr.texture});
      }
    } else {
      // Vertices 0 and 1 are in S+, and 2 are outside S+
      clip_pool.push(
          {{curr.vertices(0), curr.vertices(1), intersect_12},
           {curr.normals(0), curr.normals(1), normal_12},
           {curr.texture_coords(0), curr.texture_coords(1), texture_coords_12},
           curr.material,
           curr.texture});
      clip_pool.push(
          {{curr.vertices(0), intersect_12, intersect_20},
           {curr.normals(0), normal_12, normal_20},
           {curr.texture_coords(0), texture_coords_12, texture_coords_20},
           curr.material,
           curr.texture});
    }
  }
}

void Renderer::DrawPixel(const WindowSize& window_size, ScreenPicture& pixels,
                         ZBuffer& z_buffer, const ScreenPoint& location,
                         Color color) {
  if (location.x >= 0 && location.x < window_size.width && location.y >= 0 &&
      location.y < window_size.height) {
    int index = location.y * window_size.width + location.x;
    if (location.depth <= z_buffer[index]) {
      pixels[index] = color;
      z_buffer[index] = location.depth;
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

void Renderer::DrawBorder(const TriangleData& triangle_data,
                          const WindowSize& window_size, ScreenPicture& pixels,
                          ZBuffer& z_buffer, Color color) {
  DrawLine({.x = Width{int(triangle_data.vertices(0)(0))},
            .y = Height{int(triangle_data.vertices(0)(1))},
            .depth = triangle_data.vertices(0)(2)},
           {.x = Width{int(triangle_data.vertices(1)(0))},
            .y = Height{int(triangle_data.vertices(1)(1))},
            .depth = triangle_data.vertices(1)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);

  DrawLine({.x = Width{int(triangle_data.vertices(1)(0))},
            .y = Height{int(triangle_data.vertices(1)(1))},
            .depth = triangle_data.vertices(1)(2)},
           {.x = Width{int(triangle_data.vertices(2)(0))},
            .y = Height{int(triangle_data.vertices(2)(1))},
            .depth = triangle_data.vertices(2)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);

  DrawLine({.x = Width{int(triangle_data.vertices(2)(0))},
            .y = Height{int(triangle_data.vertices(2)(1))},
            .depth = triangle_data.vertices(2)(2)},
           {.x = Width{int(triangle_data.vertices(0)(0))},
            .y = Height{int(triangle_data.vertices(0)(1))},
            .depth = triangle_data.vertices(0)(2)},
           window_size, pixels, z_buffer, kBORDER_COLOR);
}

void Renderer::RasterizeTriangle(TriangleData& triangle_data,
                                 WindowSize window_size, ScreenPicture& pixels,
                                 ZBuffer& z_buffer) {
  for (Index i = 0; i < 3; ++i) {
    triangle_data.vertices(i) =
        triangle_data.vertices(i) * (1.0 / triangle_data.vertices(i)(3));
    triangle_data.vertices(i)(0) =
        ConvertToScreenX(window_size, triangle_data.vertices(i)(0));
    triangle_data.vertices(i)(1) =
        ConvertToScreenY(window_size, triangle_data.vertices(i)(1));
  }

  ElemType triangle_area = triangle_data.vertices.GetAreaXYProjection();
  if (std::abs(triangle_area) < kEPS) {
    // TriangleData is degenerate
    return;
  }

  OffsetedVector bound_box_borders =
      GetBoundingBoxBorders(triangle_data, window_size);

  for (Index i = bound_box_borders.begin(1); i <= bound_box_borders.end(1);
       ++i) {
    for (Index j = bound_box_borders.begin(0); j <= bound_box_borders.end(0);
         ++j) {
      Point4 barycentric_point = ComputeBarycentric(
          {ElemType(j), ElemType(i), 0, 0}, triangle_data, triangle_area);
      if (barycentric_point(0) >= -kEPS && barycentric_point(1) >= -kEPS &&
          barycentric_point(2) >= -kEPS) {
        ElemType depth = ComputeDepth(triangle_data, barycentric_point);
        DrawPixel(window_size, pixels, z_buffer, {Height(i), Width(j), depth},
                  0xFFFFFF);
      }
    }
  }
}

Detail::ScreenPicture Renderer::RenderScene(const std::vector<Object>& objects,
                                            Camera& camera,
                                            const Lights& lights,
                                            WindowSize window_size) {
  CameraRatioCheck(camera, window_size);

  ScreenPicture pixels(window_size.width * window_size.height, 0x000000);
  ZBuffer z_buf(window_size.width * window_size.height, kMAX_Z_DEPTH);

  Scene::FrustumPlanes frustum_planes = camera.GetFrustumPlanes();

  for (auto object : objects) {
    // Clipping
    std::queue<Scene::TriangleData> clipping_pool;
    for (auto index = 0; index < object.GetTrianglesCount(); ++index) {
      TriangleData triangle_data = object(index);
      triangle_data.vertices.OffsetCoords(object.GetPosition() -
                                          camera.GetPosition());
      clipping_pool.push(triangle_data);
    }

    ClipThrough(frustum_planes.near, clipping_pool);
    ClipThrough(frustum_planes.far, clipping_pool);

    ClipThrough(frustum_planes.up, clipping_pool);
    ClipThrough(frustum_planes.down, clipping_pool);

    ClipThrough(frustum_planes.left, clipping_pool);
    ClipThrough(frustum_planes.right, clipping_pool);

    // Frustum apply
    std::vector<TriangleData> frustumed_triangles;
    while (!clipping_pool.empty()) {
      clipping_pool.front().vertices.Transform(camera.GetFullFrustumMatrix());
      frustumed_triangles.push_back(std::move(clipping_pool.front()));

      clipping_pool.pop();
    }

    // Draw triangles
    for (auto& triangle_data : frustumed_triangles) {
      RasterizeTriangle(triangle_data, window_size, pixels, z_buf);
    }
    // Draw triangle borders
    for (auto& triangle_data : frustumed_triangles) {
      DrawBorder(triangle_data, window_size, pixels, z_buf, kBORDER_COLOR);
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

Linear::OffsetedVector Renderer::GetBoundingBoxBorders(
    const TriangleData& triangle_data, WindowSize window_size) {
  Point4 begin{std::max(0.0, std::min({triangle_data.vertices(0)(0),
                                       triangle_data.vertices(1)(0),
                                       triangle_data.vertices(2)(0)})),
               std::max(0.0, std::min({triangle_data.vertices(0)(1),
                                       triangle_data.vertices(1)(1),
                                       triangle_data.vertices(2)(1)})),
               0, 0};
  Point4 end{std::min(window_size.width - 1.0,
                      std::max({triangle_data.vertices(0)(0),
                                triangle_data.vertices(1)(0),
                                triangle_data.vertices(2)(0)})),
             std::min(window_size.height - 1.0,
                      std::max({triangle_data.vertices(0)(1),
                                triangle_data.vertices(1)(1),
                                triangle_data.vertices(2)(1)})),
             0, 0};
  return {begin, end};
}

}  // namespace Rendering

#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>

namespace {

using Linear::ElemType;

ElemType EvalAffinePlane2D(ElemType x0, ElemType y0, ElemType q0, ElemType x1,
                           ElemType y1, ElemType q1, ElemType x2, ElemType y2,
                           ElemType q2, ElemType px, ElemType py) {
  const ElemType dx1 = x1 - x0;
  const ElemType dy1 = y1 - y0;
  const ElemType dx2 = x2 - x0;
  const ElemType dy2 = y2 - y0;
  const ElemType dq1 = q1 - q0;
  const ElemType dq2 = q2 - q0;
  const ElemType det = dx1 * dy2 - dx2 * dy1;
  if (std::abs(det) < 1e-40) {
    return q0;
  }
  const ElemType a = (dq1 * dy2 - dq2 * dy1) / det;
  const ElemType b = (dx1 * dq2 - dx2 * dq1) / det;
  const ElemType c = q0 - a * x0 - b * y0;
  return a * px + b * py + c;
}

}  // namespace

namespace Rendering {

void Renderer::CameraRatioCheck(Camera& camera, WindowSize window_size) {
  double aspect_ratio = static_cast<double>(window_size.width) /
                        static_cast<double>(window_size.height);
  if (camera.GetAspectRatio() != aspect_ratio) {
    camera.SetAspectRatio(aspect_ratio);
  }
}

bool Renderer::IsBackfaceCulled(const TriangleData& triangle,
                                const Camera& camera) {
  Point4 e1 = triangle.vertices(1) - triangle.vertices(0);
  Point4 e2 = triangle.vertices(2) - triangle.vertices(0);

  Point4 face_normal = Linear::CrossProduct(e1, e2);
  ElemType len_sq = Linear::DotProduct(face_normal, face_normal);
  if (len_sq < kEPS * kEPS) {
    return true;
  }

  Point4 vertex = triangle.vertices(0);
  Point4 view_vector = Linear::Normalize(camera.GetPosition() - vertex);
  return Linear::DotProduct(face_normal, view_vector) < -kEPS * std::sqrt(len_sq);
}

Linear::Point4 Renderer::ComputeBarycentric(const Point4& point,
                                            const Triangle& triangle,
                                            const ElemType& triangle_area) {
  const ElemType opp2 =
      Triangle{triangle(0), triangle(1), point}.GetAreaXYProjection();
  const ElemType opp0 =
      Triangle{triangle(1), triangle(2), point}.GetAreaXYProjection();
  const ElemType opp1 =
      Triangle{triangle(2), triangle(0), point}.GetAreaXYProjection();

  return {opp0 / triangle_area, opp1 / triangle_area, opp2 / triangle_area,
          0};
}

Linear::ElemType Renderer::MultiplyColor(const Color& color,
                                         const ElemType& scalar) {
  uint8_t a = (color >> 24) & 0xFF;
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  auto clampChannel = [](int channel) -> uint8_t {
    return static_cast<uint8_t>(std::min(255, std::max(0, channel)));
  };

  int new_r = static_cast<int>(r * scalar);
  int new_g = static_cast<int>(g * scalar);
  int new_b = static_cast<int>(b * scalar);

  Color result = (static_cast<Color>(a) << 24) |
                 (static_cast<Color>(clampChannel(new_r)) << 16) |
                 (static_cast<Color>(clampChannel(new_g)) << 8) |
                 static_cast<Color>(clampChannel(new_b));
  return result;
}

Linear::Point4 Renderer::ConstructTextureCoord(
    const TriangleData& triangle_data, const Point4& barycentric_point,
    const Point4& normalize_point) const {
  ElemType normalize_coeff = 0;
  for (Index i = 0; i < 3; ++i) {
    normalize_coeff += barycentric_point(i) * normalize_point(i);
  }
  Point4 result;
  for (Index i = 0; i < 3; ++i) {
    result += triangle_data.texture_coords(i) * normalize_point(i) *
              barycentric_point(i);
  }
  result *= (1.0 / normalize_coeff);
  return result;
}

Detail::Color Renderer::GetTextureColor(const Material* const material,
                                        const Point4& texture_coord) const {
  if (material) {
    return material->texture.Sample(texture_coord);
  }
  return kDEFAULT_COLOR;
}

void Renderer::ClipTrianglesThroughPlane(const Plane& plane,
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

    if (dist0 < -kEPS && dist1 < -kEPS && dist2 < -kEPS) {
      continue;
    } else if (dist0 > kEPS && dist1 > kEPS && dist2 > kEPS) {
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
             curr.material_index});
      } else if (dist2 <= 0) {
        // Vertices 0 and 2 outside of S+, only 1 in S+
        clip_pool.push(
            {{intersect_01, curr.vertices(1), intersect_12},
             {normal_01, curr.normals(1), normal_12},
             {texture_coords_01, curr.texture_coords(1), texture_coords_12},
             curr.material_index});
      } else {
        // Vertex 0 is outside of S+, and 1 and 2 are in S+
        clip_pool.push({{intersect_01, curr.vertices(1), curr.vertices(2)},
                        {normal_01, curr.normals(1), curr.normals(2)},
                        {texture_coords_01, curr.texture_coords(1),
                         curr.texture_coords(2)},
                        curr.material_index});
        clip_pool.push(
            {{intersect_01, curr.vertices(2), intersect_20},
             {normal_01, curr.normals(2), normal_20},
             {texture_coords_01, curr.texture_coords(2), texture_coords_20},
             curr.material_index});
      }
    } else if (dist1 <= 0) {
      // Vertex 1 is outside of S+, and 0 is in S+
      if (dist2 <= 0) {
        // Only 0 in S+
        clip_pool.push(
            {{curr.vertices(0), intersect_01, intersect_20},
             {curr.normals(0), normal_01, normal_20},
             {curr.texture_coords(0), texture_coords_01, texture_coords_20},
             curr.material_index});
      } else {
        // Vertices 0 and 2 in S+, vertex 1 outside S+
        clip_pool.push(
            {{curr.vertices(0), intersect_01, intersect_12},
             {curr.normals(0), normal_01, normal_12},
             {curr.texture_coords(0), texture_coords_01, texture_coords_12},
             curr.material_index});
        clip_pool.push({{curr.vertices(0), intersect_12, curr.vertices(2)},
                        {curr.normals(0), normal_12, curr.normals(2)},
                        {curr.texture_coords(0), texture_coords_12,
                         curr.texture_coords(2)},
                        curr.material_index});
      }
    } else {
      // Vertices 0 and 1 are in S+, and 2 are outside S+
      clip_pool.push(
          {{curr.vertices(0), curr.vertices(1), intersect_12},
           {curr.normals(0), curr.normals(1), normal_12},
           {curr.texture_coords(0), curr.texture_coords(1), texture_coords_12},
           curr.material_index});
      clip_pool.push(
          {{curr.vertices(0), intersect_12, intersect_20},
           {curr.normals(0), normal_12, normal_20},
           {curr.texture_coords(0), texture_coords_12, texture_coords_20},
           curr.material_index});
    }
  }
}

void Renderer::DrawPixel(const WindowSize& window_size, ScreenPicture& pixels,
                         ZBuffer& z_buffer, const ScreenPoint& location,
                         Color color) {
  if (location.x >= 0 && location.x < window_size.width && location.y >= 0 &&
      location.y < window_size.height) {
    int index = location.y * window_size.width + location.x;
    if (location.depth < z_buffer[index]) {
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
                                 const Material* const material,
                                 const Camera& camera, WindowSize window_size,
                                 ScreenPicture& pixels, ZBuffer& z_buffer,
                                 const Lights& lights) {

  // For correct light computaion
  TriangleData triangle_cpy = triangle_data;

  // Frustum transform
  triangle_data.vertices.Transform(camera.GetFullFrustumMatrix());

  Point4 normalize_point;
  ElemType sx[3]{};
  ElemType sy[3]{};

  for (Index i = 0; i < 3; ++i) {
    const ElemType inv_w = 1.0 / triangle_data.vertices(i)(3);
    normalize_point(i) = inv_w;

    Point4 ndc = triangle_data.vertices(i) * inv_w;
    const ElemType ndc_x = ndc(0);
    const ElemType ndc_y = ndc(1);
    const ElemType ndc_z = ndc(2);

    sx[i] = (ndc_x + 1.0) * 0.5 * static_cast<ElemType>(window_size.width - 1);
    sy[i] =
        (1.0 - (ndc_y + 1.0) * 0.5) * static_cast<ElemType>(window_size.height - 1);
    triangle_data.vertices(i)(0) = sx[i];
    triangle_data.vertices(i)(1) = sy[i];
    triangle_data.vertices(i)(2) = ndc_z;
    triangle_data.vertices(i)(3) = 0;
  }

  const ElemType invw0 = normalize_point(0);
  const ElemType invw1 = normalize_point(1);
  const ElemType invw2 = normalize_point(2);
  const ElemType uow0 = triangle_cpy.texture_coords(0)(0) * invw0;
  const ElemType uow1 = triangle_cpy.texture_coords(1)(0) * invw1;
  const ElemType uow2 = triangle_cpy.texture_coords(2)(0) * invw2;
  const ElemType vow0 = triangle_cpy.texture_coords(0)(1) * invw0;
  const ElemType vow1 = triangle_cpy.texture_coords(1)(1) * invw1;
  const ElemType vow2 = triangle_cpy.texture_coords(2)(1) * invw2;

  ElemType triangle_area = triangle_data.vertices.GetAreaXYProjection();
  if (std::abs(triangle_area) < kEPS) {
    return;
  }

  OffsetedVector bound_box_borders =
      GetBoundingBoxBorders(triangle_data, window_size);

  for (Index j = bound_box_borders.begin(0); j <= bound_box_borders.end(0);
      ++j) {
    for (Index i = bound_box_borders.begin(1); i <= bound_box_borders.end(1);
        ++i) {
      const ElemType px = static_cast<ElemType>(j) + 0.5;
      const ElemType py = static_cast<ElemType>(i) + 0.5;

      Point4 barycentric_point =
          ComputeBarycentric({px, py, 0, 0}, triangle_data.vertices,
                             triangle_area);
      if (barycentric_point(0) >= -kEPS && barycentric_point(1) >= -kEPS &&
          barycentric_point(2) >= -kEPS) {
        ElemType norm_coeff = 0;
        for (Index n = 0; n < 3; ++n) {
          norm_coeff += barycentric_point(n) * normalize_point(n);
        }

        if (std::abs(norm_coeff) < kEPS) {
          continue;
        }

        Point4 perspective_barycentric;
        for (Index n = 0; n < 3; ++n) {
          perspective_barycentric(n) =
              (barycentric_point(n) * normalize_point(n)) / norm_coeff;
        }

        perspective_barycentric(3) = 0;

        ElemType depth = triangle_data.vertices.GetPointByBarycentric(
            perspective_barycentric)(2);

        ElemType intensity = light_manager_.ComputeLightning(
            {}, triangle_cpy, perspective_barycentric, lights);

        const ElemType inv_w_at_p =
            EvalAffinePlane2D(sx[0], sy[0], invw0, sx[1], sy[1], invw1, sx[2],
                              sy[2], invw2, px, py);
        if (std::abs(inv_w_at_p) < kEPS) {
          continue;
        }
        const ElemType u_at_p =
            EvalAffinePlane2D(sx[0], sy[0], uow0, sx[1], sy[1], uow1, sx[2],
                              sy[2], uow2, px, py) /
            inv_w_at_p;
        const ElemType v_at_p =
            EvalAffinePlane2D(sx[0], sy[0], vow0, sx[1], sy[1], vow1, sx[2],
                              sy[2], vow2, px, py) /
            inv_w_at_p;
        Point4 texture_coord{u_at_p, v_at_p, 0, 0};

        Color texture_color = GetTextureColor(material, texture_coord);

        Color final_color = MultiplyColor(texture_color, intensity);

        DrawPixel(window_size, pixels, z_buffer, {Height(i), Width(j), depth},
                  final_color);
      }
    }
  }
}

Detail::ScreenPicture Renderer::RenderScene(const std::vector<Object>& objects,
                                            Camera& camera,
                                            const Lights& lights,
                                            WindowSize window_size) {
  CameraRatioCheck(camera, window_size);

  Lights view_lights = lights;
  for (auto& light : view_lights) {
    light.position -= camera.GetPosition();
  }

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

      if (!IsBackfaceCulled(triangle_data, camera)) {
        clipping_pool.push(triangle_data);
      }
    }

    ClipTrianglesThroughPlane(frustum_planes.near, clipping_pool);
    ClipTrianglesThroughPlane(frustum_planes.far, clipping_pool);

    ClipTrianglesThroughPlane(frustum_planes.up, clipping_pool);
    ClipTrianglesThroughPlane(frustum_planes.down, clipping_pool);

    ClipTrianglesThroughPlane(frustum_planes.left, clipping_pool);
    ClipTrianglesThroughPlane(frustum_planes.right, clipping_pool);

    std::vector<TriangleData> clipped_triangles;
    while (!clipping_pool.empty()) {
      clipped_triangles.push_back(std::move(clipping_pool.front()));
      clipping_pool.pop();
    }

    // Draw triangles
    for (auto& triangle_data : clipped_triangles) {
      RasterizeTriangle(triangle_data,
                        object.GetMaterial(triangle_data.material_index),
                        camera, window_size, pixels, z_buf, view_lights);
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
  ElemType min_x = std::min({triangle_data.vertices(0)(0),
                             triangle_data.vertices(1)(0),
                             triangle_data.vertices(2)(0)});
  ElemType max_x = std::max({triangle_data.vertices(0)(0),
                             triangle_data.vertices(1)(0),
                             triangle_data.vertices(2)(0)});
  ElemType min_y = std::min({triangle_data.vertices(0)(1),
                             triangle_data.vertices(1)(1),
                             triangle_data.vertices(2)(1)});
  ElemType max_y = std::max({triangle_data.vertices(0)(1),
                             triangle_data.vertices(1)(1),
                             triangle_data.vertices(2)(1)});

  Point4 begin{std::max(0.0, std::floor(min_x)), std::max(0.0, std::floor(min_y)),
               0, 0};
  Point4 end{std::min(static_cast<ElemType>(window_size.width - 1),
                      std::ceil(max_x)),
             std::min(static_cast<ElemType>(window_size.height - 1),
                      std::ceil(max_y)),
             0, 0};
  return {begin, end};
}

}  // namespace Rendering

#include "Parser.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Object.h"
#include "stb_image.h"

namespace Scene {

static Linear::Point4 parse_vertex_line(const std::string& line) {
  std::istringstream stream(line);
  std::string prefix;
  Linear::ElemType x, y, z;
  stream >> prefix >> x >> y >> z;
  return Linear::Point4(x, y, z, 1.0f);
}

static Linear::Point4 parse_normal_line(const std::string& line) {
  std::istringstream stream(line);
  std::string prefix;
  Linear::ElemType x, y, z;
  stream >> prefix >> x >> y >> z;
  return Linear::Point4(x, y, z, 0.0f);
}

static Linear::Vector4 parse_tex_coord_line(const std::string& line) {
  std::istringstream stream(line);
  std::string prefix;
  Linear::ElemType u, v;
  stream >> prefix >> u >> v;
  return Linear::Vector4(u, 1.0 - v, 0.0f, 0.0f);
}

static void parse_face_token(const std::string& token, int& vertex_idx,
                             int& tex_coord_idx, int& normal_idx) {
  vertex_idx = tex_coord_idx = normal_idx = -1;

  size_t first_slash = token.find('/');
  size_t second_slash = token.find('/', first_slash + 1);

  if (first_slash == std::string::npos) {
    vertex_idx = std::stoi(token) - 1;
  } else {
    vertex_idx = std::stoi(token.substr(0, first_slash)) - 1;

    if (second_slash == std::string::npos) {
      std::string tex = token.substr(first_slash + 1);
      if (!tex.empty())
        tex_coord_idx = std::stoi(tex) - 1;
    } else {
      std::string tex =
          token.substr(first_slash + 1, second_slash - first_slash - 1);
      if (!tex.empty())
        tex_coord_idx = std::stoi(tex) - 1;

      std::string norm = token.substr(second_slash + 1);
      if (!norm.empty())
        normal_idx = std::stoi(norm) - 1;
    }
  }
}

static Detail::Texture load_texture_file(const std::string& filepath) {
  int width, height, channels;
  unsigned char* data =
      stbi_load(filepath.c_str(), &width, &height, &channels, 4);
  if (!data) {
    std::cerr << "Failed to load texture: " << filepath << std::endl;
    return {};
  }
  std::vector<Detail::Color> pixels(width * height);
  for (int i = 0; i < width * height; ++i) {
    uint8_t* p = data + i * 4;
    // Формат ARGB (альфа на старшем байте)
    pixels[i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
  }
  stbi_image_free(data);
  return Detail::Texture(pixels, Linear::Detail::Height(height),
                         Linear::Detail::Width(width));
}

static std::unordered_map<std::string, Detail::Material> parse_material_file(
    const std::string& mtl_path, const std::string& base_dir) {
  std::cerr << "[DEBUG] parse_material_file: mtl_path=\"" << mtl_path
            << "\", base_dir=\"" << base_dir << "\"\n";
  std::unordered_map<std::string, Detail::Material> material_map;
  std::ifstream file(mtl_path);
  if (!file) {
    std::cerr << "Cannot open MTL: " << mtl_path << std::endl;
    return material_map;
  }
  std::string line;
  std::string material_name;
  Detail::Material material;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream stream(line);
    std::string prefix;
    stream >> prefix;
    if (prefix == "newmtl") {
      if (!material_name.empty()) {
        material_map[material_name] = material;
      }
      stream >> material_name;
      material = Detail::Material();
    } else if (prefix == "Ka") {
      stream >> material.ambient(0) >> material.ambient(1) >>
          material.ambient(2);
    } else if (prefix == "Kd") {
      stream >> material.diffuse(0) >> material.diffuse(1) >>
          material.diffuse(2);
    } else if (prefix == "Ks") {
      stream >> material.specular(0) >> material.specular(1) >>
          material.specular(2);
    } else if (prefix == "Ns") {
      stream >> material.shininess;
    } else if (prefix == "map_Kd") {
      std::string tex_file;
      stream >> tex_file;
      size_t pos = tex_file.find_last_of("/\\");
      std::string name =
          (pos == std::string::npos) ? tex_file : tex_file.substr(pos + 1);
      material.texture = load_texture_file(base_dir + name);
    }
  }
  if (!material_name.empty()) {
    material_map[material_name] = material;
  }
  return material_map;
}

Object ObjParser::Parse(const std::string& filepath) {
  std::ifstream in(filepath);
  if (!in) {
    std::cerr << "Cannot open OBJ: " << filepath << "\n";
    return Object({}, {});
  }
  // базовая папка
  std::string base = filepath.substr(0, filepath.find_last_of("/\\") + 1);

  // geometry buffers
  std::vector<Linear::Point4> vertices;
  std::vector<Linear::Point4> normals;
  std::vector<Linear::Vector4> texcoords;
  std::vector<TriangleData> tris;

  // materials
  std::unordered_map<std::string, Detail::Material> mat_map;
  std::vector<Detail::Material> mat_list;
  std::unordered_map<std::string, int> mat_index;

  std::string current_mtl, line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream ss(line);
    std::string prefix;
    ss >> prefix;
    if (prefix == "mtllib") {
      std::string mtl;
      ss >> mtl;
      mat_map = parse_material_file(base + mtl, base);
    } else if (prefix == "usemtl") {
      ss >> current_mtl;
    } else if (prefix == "v") {
      vertices.push_back(parse_vertex_line(line));
    } else if (prefix == "vn") {
      normals.push_back(parse_normal_line(line));
    } else if (prefix == "vt") {
      texcoords.push_back(parse_tex_coord_line(line));
    } else if (prefix == "f") {
      std::vector<std::string> tokens;
      std::string tok;
      while (ss >> tok)
        tokens.push_back(tok);
      for (size_t i = 1; i + 1 < tokens.size(); ++i) {
        int vi[3], ti[3], ni[3];
        parse_face_token(tokens[0], vi[0], ti[0], ni[0]);
        parse_face_token(tokens[i], vi[1], ti[1], ni[1]);
        parse_face_token(tokens[i + 1], vi[2], ti[2], ni[2]);

        Linear::Triangle tri(vertices[vi[1]], vertices[vi[2]], vertices[vi[0]]);
        Linear::Triangle norm_tri;
        if (!normals.empty() && ni[0] >= 0) {
          norm_tri(0) = normals[ni[0]];
          norm_tri(1) = normals[ni[1]];
          norm_tri(2) = normals[ni[2]];
        } else {
          auto nn = tri.GetNormal();
          norm_tri = Linear::Triangle(nn, nn, nn);
        }
        Linear::Triangle uv_tri;
        if (!texcoords.empty() && ti[0] >= 0) {
          uv_tri(0) = texcoords[ti[0]];
          uv_tri(1) = texcoords[ti[1]];
          uv_tri(2) = texcoords[ti[2]];
        }
        TriangleData td(tri, norm_tri, uv_tri);
        if (!current_mtl.empty()) {
          auto it = mat_map.find(current_mtl);
          if (it != mat_map.end()) {
            auto mit = mat_index.find(current_mtl);
            if (mit == mat_index.end()) {
              int idx = static_cast<int>(mat_list.size());
              mat_list.push_back(it->second);
              mat_index[current_mtl] = idx;
              td.material_index = idx;
            } else {
              td.material_index = mit->second;
            }
          }
        }
        tris.push_back(std::move(td));
      }
    }
  }

  return Object(std::move(tris), std::move(mat_list));
}

}  // namespace Scene

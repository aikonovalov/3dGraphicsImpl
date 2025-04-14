#include "Parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Object.h"

namespace Scene {

static Linear::Point4 ParseVertex(const std::string& line) {
  std::istringstream iss(line);
  std::string prefix;
  Linear::ElemType x, y, z;
  iss >> prefix >> x >> y >> z;
  return Linear::Point4(x, y, z, 1.0f);
}

static Linear::Point4 ParseNormal(const std::string& line) {
  std::istringstream iss(line);
  std::string prefix;
  Linear::ElemType x, y, z;
  iss >> prefix >> x >> y >> z;
  return Linear::Point4(x, y, z, 0.0f);
}

static Linear::Vector4 ParseTexCoord(const std::string& line) {
  std::istringstream iss(line);
  std::string prefix;
  Linear::ElemType u;
  Linear::ElemType v;
  iss >> prefix >> u >> v;
  return Linear::Vector4(u, v, 0, 0);
}

static void ParseFaceVertex(const std::string& token, int& vIdx, int& vtIdx,
                            int& vnIdx) {
  std::string modified = token;
  std::replace(modified.begin(), modified.end(), '/', ' ');
  std::istringstream iss(modified);
  iss >> vIdx >> vtIdx >> vnIdx;
  --vIdx;
  --vtIdx;
  --vnIdx;
}

Object ObjParser::Parse(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error occured while opening file: " << filename << std::endl;
    return Object(std::vector<TriangleData>{});
  }

  std::vector<Linear::Point4> vertices;
  std::vector<Linear::Point4> normals;
  std::vector<Linear::Point4> texCoords;
  std::vector<TriangleData> triangleDatas;

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;

    if (prefix == "v") {
      vertices.push_back(ParseVertex(line));
    } else if (prefix == "vn") {
      normals.push_back(ParseNormal(line));
    } else if (prefix == "vt") {
      texCoords.push_back(ParseTexCoord(line));
    } else if (prefix == "f") {
      std::string token1, token2, token3;
      iss >> token1 >> token2 >> token3;
      int vIdx[3], vtIdx[3], vnIdx[3];
      ParseFaceVertex(token1, vIdx[0], vtIdx[0], vnIdx[0]);
      ParseFaceVertex(token2, vIdx[1], vtIdx[1], vnIdx[1]);
      ParseFaceVertex(token3, vIdx[2], vtIdx[2], vnIdx[2]);
      Linear::Point4 p0 = vertices[vIdx[0]];
      Linear::Point4 p1 = vertices[vIdx[1]];
      Linear::Point4 p2 = vertices[vIdx[2]];

      Linear::Triangle tri(p0, p1, p2);
      TriangleData triangle_data{
          tri,
          {Linear::Point4{0, 0, 0}, Linear::Point4{0, 0, 0},
           Linear::Point4{0, 0, 0}},
          {Linear::Point4{0, 0, 0}, Linear::Point4{0, 0, 0},
           Linear::Point4{0, 0, 0}},
          {},
          {}};

      if (!normals.empty() && vnIdx[0] >= 0 && vnIdx[1] >= 0 && vnIdx[2] >= 0) {
        triangle_data.normals(0) = normals[vnIdx[0]];
        triangle_data.normals(1) = normals[vnIdx[1]];
        triangle_data.normals(2) = normals[vnIdx[2]];
      } else {
        Linear::Point4 calculated_normal = tri.GetNormal();
        triangle_data.normals = {calculated_normal, calculated_normal,
                                 calculated_normal};
      }

      if (!texCoords.empty() && vtIdx[0] >= 0 && vtIdx[1] >= 0 &&
          vtIdx[2] >= 0) {
        triangle_data.texture_coords(0) = texCoords[vtIdx[0]];
        triangle_data.texture_coords(1) = texCoords[vtIdx[1]];
        triangle_data.texture_coords(2) = texCoords[vtIdx[2]];
      }
      triangle_data.material = Detail::Material();
      triangleDatas.push_back(triangle_data);
    }
  }
  file.close();

  return Object(std::move(triangleDatas));
}

}  // namespace Scene

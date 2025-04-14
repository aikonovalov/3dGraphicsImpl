#pragma once

#include <string>
#include "Object.h"

namespace Scene {

class ObjParser {
public:
  static Object Parse(const std::string& filename);
};

}  // namespace Scene

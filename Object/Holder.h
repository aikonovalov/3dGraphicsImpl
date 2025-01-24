#pragma once

#include <cstddef>
#include "Camera.h"
#include "Object.h"

namespace Scene {

struct ObjectHolder {
public:
  Object obj_;
  Linear::Point3d coords_;

  ObjectHolder();
  ObjectHolder(const Object&);
  ObjectHolder(const Object&, const Linear::Point3d&);

  Linear::Triangle GetGlobalCoordsByIndex(size_t);
};

struct CameraHolder {
public:
  Camera cam_;
  Linear::Point3d coords_;

  CameraHolder();
  CameraHolder(const Camera&);
  CameraHolder(const Camera&, const Linear::Point3d&);

  Linear::Triangle GetLocalCoords(const Linear::Triangle&);
};

}  // namespace Scene

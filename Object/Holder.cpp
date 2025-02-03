#include "Holder.h"

namespace Scene {
ObjectHolder::ObjectHolder() : obj_(Object()), coords_(0, 0, 0) {}

ObjectHolder::ObjectHolder(const Object& obj) : obj_(obj), coords_(0, 0, 0) {}

ObjectHolder::ObjectHolder(const Object& obj, const Linear::Point3d& coords)
    : obj_(obj), coords_(coords) {}

Linear::Triangle ObjectHolder::GetGlobalCoordsByIndex(size_t index) {
  Linear::Triangle& result = obj_.GetTriangle(index);
  return Linear::Triangle{result.GetPoint(0) + coords_,
                          result.GetPoint(1) + coords_,
                          result.GetPoint(2) + coords_};
}

CameraHolder::CameraHolder() : cam_(Camera()), coords_(0, 0, 0) {}

CameraHolder::CameraHolder(const Camera& obj) : cam_(obj), coords_(0, 0, 0) {}

CameraHolder::CameraHolder(const Camera& obj, const Linear::Point3d& coords)
    : cam_(obj), coords_(coords) {}

Linear::Triangle CameraHolder::GetLocalCoords(
    const Linear::Triangle& triangle) {
  return Linear::Triangle{triangle.GetPoint(0) - coords_,
                          triangle.GetPoint(1) - coords_,
                          triangle.GetPoint(2) - coords_};
}

}  // namespace Scene

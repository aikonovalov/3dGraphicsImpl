#include "Model.h"
#include <cassert>

namespace Core {

Model::Model(QObject* parent) : QObject(parent) {
}

Scene::Object& Model::operator()(Index index) {
  assert(index < objects_.size() && "Index out of range");
  return objects_[index];
}

Model::Index Model::GetObjectsCount() const {
  return objects_.size();
}

}  // namespace Core

#include "Model.h"
#include <cassert>

namespace Core {

Model::Model(QObject* parent) : QObject(parent) {
}

Scene::Object Model::GetObject(Index index) const {
  assert(index < objects_.size() && "Index out of range");
  return objects_[index];
}

void Model::AddObject(const Object& obj) {
  objects_.push_back(obj);
  port_.NotifyAll();
}

void Model::AddView(Observer& observer) {
  port_.SubscribeObserver(observer);
}

}  // namespace Core

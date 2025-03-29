#pragma once

#include <QObject>
#include <vector>
#include "../Detail/Observer.h"
#include "../Object/Camera.h"
#include "../Object/Object.h"
#include "ViewBase.h"

namespace Core {

class Model : public QObject {
  Q_OBJECT

public:
  using Object = Scene::Object;
  using Camera = Scene::Camera;
  using Observable = Detail::Observable<ViewBase>;
  using Observer = Detail::Observer<ViewBase>;

  explicit Model(QObject* parent = nullptr);

  enum Index : int;
  Object GetObject(Index) const;

  void AddObject(const Object&);

  void AddView(Observer&);

private:
  std::vector<Object> objects_;
  // Camera camera_;
  Observable port_;
};

}  // namespace Core

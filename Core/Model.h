#pragma once

#include <QObject>
#include <list>
#include <vector>
#include "../Detail/Observer.h"
#include "../Detail/Palette.h"
#include "../Renderer/Renderer.h"
#include "Controller.h"

namespace Core {

class Controller;

class Model : public QObject {
  Q_OBJECT

  friend Controller;

public:
  using Object = Scene::Object;
  using Renderer = Rendering::Renderer;
  using Camera = Scene::Camera;
  using Light = Detail::Light;

  using Objects = std::vector<Object>;
  using Cameras = std::vector<Camera>;
  using Lights = Detail::Lights;

  using ScreenPicture = Detail::ScreenPicture;
  using WindowSize = Detail::WindowSize;
  using Observable = Detail::Observable<ScreenPicture, WindowSize>;
  using Observer = Detail::Observer<ScreenPicture, WindowSize>;
  using Index = Linear::Index;

  explicit Model(QObject* parent = nullptr);
  Object& operator()(Index);

  Index GetObjectsCount() const;

private:
  Renderer renderer_;
  Objects objects_;
  Lights lights_;
  Camera camera_;

  Observable port_;
};

}  // namespace Core

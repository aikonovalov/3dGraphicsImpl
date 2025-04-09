#pragma once

#include "../Detail/Palette.h"
#include "Model.h"

namespace Core {

class Controller {
  using ScreenPicture = Detail::ScreenPicture;
  using WindowSize = Detail::WindowSize;
  using Observer = Detail::Observer<ScreenPicture, WindowSize>;

public:
  Controller(Model* model_link);

  void AddObject(Scene::Object& object);

  void AddView(Observer& observer, WindowSize size);

  void ResizeWindow(Observer* observer, WindowSize new_size);

  void UpdateAll();

private:
  Model* model_link_;
};

}  // namespace Core

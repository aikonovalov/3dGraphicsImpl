#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <vector>
#include "../Detail/Observer.h"
#include "../Detail/Palette.h"
#include "Controller.h"

namespace Core {

class View : public QMainWindow {
  Q_OBJECT
  using ScreenPicture = Detail::ScreenPicture;
  using WindowSize = Detail::WindowSize;
  using Observer = Detail::Observer<ScreenPicture, WindowSize>;
  using Camera = Scene::Camera;

public:
  View(Controller* controller_link);

  void Draw(ScreenPicture& screen_picture);

  WindowSize GetWindowSize() const;

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  QLabel* label_;
  Controller* controller_;
  Observer port_;
};

}  // namespace Core

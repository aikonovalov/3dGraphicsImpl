#pragma once

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>
#include "../Detail/Observer.h"
#include "../Detail/Palette.h"
#include "Controller.h"

namespace Core {

class View : public QMainWindow {
  Q_OBJECT
  using ElemType = Linear::ElemType;
  using ScreenPicture = Detail::ScreenPicture;
  using WindowSize = Detail::WindowSize;
  using Observer = Detail::Observer<ScreenPicture, WindowSize>;
  using Camera = Scene::Camera;
  using Index = Linear::Index;

public:
  View(Controller* controller_link);

  void Draw(ScreenPicture& screen_picture);

  WindowSize GetWindowSize() const;

signals:
  void objectMoveRequested(Index index, ElemType dx, ElemType dy, ElemType dz);
  void objectRotateRequested(Index index, ElemType rx, ElemType ry,
                             ElemType rz);
  void cameraMoveRequested(ElemType dx, ElemType dy, ElemType dz);
  void cameraRotateRequested(ElemType delta_pitch, ElemType delta_yaw,
                             ElemType dummy);
  void modelLoadRequested(const QString& fileName);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  QLabel* label_;
  Controller* controller_;
  Observer port_;

  QWidget* control_panel_;
};

}  // namespace Core

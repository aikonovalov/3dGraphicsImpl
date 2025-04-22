#pragma once

#include <QObject>
#include <QString>
#include "../Detail/Palette.h"
#include "Model.h"

namespace Core {

class Controller : public QObject {
  Q_OBJECT
  using ScreenPicture = Detail::ScreenPicture;
  using Index = Linear::Index;
  using WindowSize = Detail::WindowSize;
  using Point4 = Linear::Point4;
  using Observer = Detail::Observer<ScreenPicture, WindowSize>;
  using ElemType = Linear::ElemType;
  using TransformMatrix4x4 = Linear::TransformMatrix4x4;
  using TriangleData = Scene::TriangleData;
  using Object = Scene::Object;
  using Light = Detail::Light;

public:
  Controller(Model* model_link);

  void AddObject(Scene::Object& object);

  void AddView(Observer& observer, WindowSize size);

  void AddLight(const Light& new_light);

  void ResizeWindow(Observer* observer, WindowSize new_size);

  void UpdateAll();

public slots:
  void onMoveObject(Index index, ElemType dx, ElemType dy, ElemType dz);
  void onMoveCamera(ElemType dx, ElemType dy, ElemType dz);

  void onRotateObject(Index index, ElemType rx, ElemType ry, ElemType rz);
  void onRotateCamera(ElemType delta_pitch, ElemType delta_yaw);
  void onModelLoad(const QString& fileName);

private:
  Model* model_link_;
};

}  // namespace Core

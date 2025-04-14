#include "Controller.h"
#include "../Object/Parser.h"
#include "Model.h"

namespace Core {

Controller::Controller(Model* model_link) : model_link_(model_link) {
}

void Controller::AddObject(Scene::Object& object) {
  model_link_->objects_.push_back(object);
  UpdateAll();
}

void Controller::AddView(Observer& observer, WindowSize size) {
  model_link_->port_.SubscribeObserver(observer, size);
  UpdateAll();
}

void Controller::AddLight(const Light& new_light) {
  model_link_->lights_.push_back(new_light);
}

void Controller::ResizeWindow(Observer* observer, WindowSize new_size) {
  model_link_->port_.SetDataByID(observer, new_size);
  UpdateAll();
};

void Controller::onMoveObject(Index index, ElemType dx, ElemType dy,
                              ElemType dz) {
  std::cout << dx << " " << dy << " " << dz << "\n";
  if (index >= 0 && index < static_cast<int>(model_link_->objects_.size())) {
    Point4 pos = (*model_link_)(index).GetPosition();
    pos(0) += dx;
    pos(1) += dy;
    pos(2) += dz;
    (*model_link_)(index).SetPosition(pos);
    UpdateAll();
  }
}

void Controller::onMoveCamera(ElemType dx, ElemType dy, ElemType dz) {
  auto pos = model_link_->camera_.GetPosition();
  pos(0) += dx;
  pos(1) += dy;
  pos(2) += dz;
  model_link_->camera_.SetPosition(pos);
  UpdateAll();
}

void Controller::onRotateObject(Index index, ElemType rx, ElemType ry,
                                ElemType rz) {
  if (index >= 0 && index < model_link_->GetObjectsCount()) {
    auto rotationX = TransformMatrix4x4::MakeRotationX(rx);
    auto rotationY = TransformMatrix4x4::MakeRotationY(ry);
    auto rotationZ = TransformMatrix4x4::MakeRotationZ(rz);

    TransformMatrix4x4 rotationMatrix = rotationZ * rotationY * rotationX;

    Object& object = (*model_link_)(index);
    for (Index i = 0; i < object.GetTrianglesCount(); ++i) {
      object(i).vertices.Transform(rotationMatrix);
    }

    UpdateAll();
  }
}

void Controller::onRotateCamera(ElemType delta_pitch, ElemType delta_yaw) {
  auto current_pitch = model_link_->camera_.GetPitch();
  auto current_yaw = model_link_->camera_.GetYAW();

  model_link_->camera_.SetPitch(current_pitch + delta_pitch);
  model_link_->camera_.SetYAW(current_yaw + delta_yaw);

  UpdateAll();
}

void Controller::onModelLoad(const QString& fileName) {
  std::string file_path = fileName.toStdString();
  Scene::Object new_object = Scene::ObjParser::Parse(file_path);

  AddObject(new_object);
  UpdateAll();
}

void Controller::UpdateAll() {
  for (auto& elem : model_link_->port_.GetObserversList()) {
    ScreenPicture renderer_output = model_link_->renderer_.RenderScene(
        model_link_->objects_, model_link_->camera_, model_link_->lights_,
        elem.second);
    model_link_->port_.NotifyOne(elem.first, renderer_output);
  }
}

}  // namespace Core

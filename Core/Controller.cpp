#include "Controller.h"
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

void Controller::ResizeWindow(Observer* observer, WindowSize new_size) {
  model_link_->port_.SetDataByID(observer, new_size);
  UpdateAll();
};

void Controller::UpdateAll() {
  for (auto& elem : model_link_->port_.GetObserversList()) {
    ScreenPicture renderer_output = model_link_->renderer_.RenderScene(
        model_link_->objects_, model_link_->camera_, elem.second);
    model_link_->port_.NotifyOne(elem.first, renderer_output);
  }
}

}  // namespace Core

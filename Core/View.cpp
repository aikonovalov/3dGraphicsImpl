#include "View.h"
#include "Model.h"
#include "ViewBase.h"

namespace Core {

View::View(Model& model) : port_() {
  model.AddView(port_);
  port_.SetNotifyAction([this]() { this->process(); });
}

void View::process() {
  std::cout << "UPDATED\n";
  return;
}

}  // namespace Core

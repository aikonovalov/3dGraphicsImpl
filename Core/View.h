#pragma once

#include "../Detail/Observer.h"
#include "Model.h"
#include "ViewBase.h"

namespace Core {

class View : public ViewBase {
  using Observer = Detail::Observer<ViewBase>;

public:
  View(Model& model);

  void process() override;

private:
  Observer port_;
};

}  // namespace Core

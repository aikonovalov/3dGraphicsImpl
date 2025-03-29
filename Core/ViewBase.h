#pragma once

namespace Core {

class ViewBase {
public:
  virtual void process() = 0;

  virtual ~ViewBase() = default;
};

}  // namespace Core

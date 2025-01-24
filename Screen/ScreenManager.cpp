#include "ScreenManager.h"
#include <utility>

namespace Application {

App::App(const std::pair<int, char**>& args,
         const std::pair<int, int>& resolution)
    : args_(args), app_(args_.first, args.second), resolution_(resolution) {
  window_.resize(resolution_.first, resolution_.second);
}

void App::SetTitle(const std::string& name) {
  window_.setWindowTitle(QString::fromStdString(name));
}

int App::Execute() {
  window_.show();
  return app_.exec();
}

}  // namespace Application
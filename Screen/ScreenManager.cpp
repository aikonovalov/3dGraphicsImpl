#include "ScreenManager.h"
#include <utility>

namespace Application {

App::App(const std::pair<int, char**>& args,
         const std::pair<int, int>& resolution)
    : args_(args), app_(args_.first, args.second), resolution_(resolution) {
  window_.resize(resolution_.first, resolution_.second);
  QLabel* label = new QLabel(&window_);

  QPixmap show_img;
  show_img.load("Resources/Pukpuk.jpg");

  label->setPixmap(show_img);
  label->setAlignment(Qt::AlignCenter);
  label->setScaledContents(true);
  window_.setCentralWidget(label);
}

void App::SetTitle(const std::string& name) {
  window_.setWindowTitle(QString::fromStdString(name));
}

int App::Execute() {
  window_.show();
  return app_.exec();
}

}  // namespace Application

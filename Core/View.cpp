#include "View.h"
#include <QDebug>
#include <QResizeEvent>
#include <vector>
#include "Controller.h"

namespace Core {

View::View(Controller* controller_link)
    : port_(), controller_(controller_link) {
  label_ = new QLabel(this);
  label_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label_->setScaledContents(true);
  setCentralWidget(label_);
  port_.SetNotifyAction(
      [this](Detail::ScreenPicture& data) { this->Draw(data); });

  controller_->AddView(port_, GetWindowSize());

  controller_->UpdateAll();

  show();
}

void View::resizeEvent(QResizeEvent* event) {
  qDebug() << "Resize event triggered, new size:" << event->size();
  QMainWindow::resizeEvent(event);
  controller_->ResizeWindow(&port_, GetWindowSize());
}

void View::Draw(Detail::ScreenPicture& screen_picture) {
  int imageWidth = label_->width();
  int imageHeight = label_->height();

  QImage image(reinterpret_cast<uchar*>(screen_picture.data()), imageWidth,
               imageHeight, QImage::Format_RGB32);

  QPixmap pixmap = QPixmap::fromImage(image);
  label_->setPixmap(pixmap);

  return;
}

Detail::WindowSize View::GetWindowSize() const {
  return {Linear::Detail::Height{label_->height()},
          Linear::Detail::Width{label_->width()}};
}

}  // namespace Core

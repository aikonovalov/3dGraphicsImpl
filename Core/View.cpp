#include "View.h"
#include <QDebug>
#include <QResizeEvent>
#include <vector>
#include "Controller.h"

namespace Core {

View::View(Controller* controller_link)
    : port_(), controller_(controller_link) {
  QWidget* central_widget = new QWidget(this);
  QHBoxLayout* main_layout = new QHBoxLayout(central_widget);

  label_ = new QLabel(central_widget);
  label_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label_->setScaledContents(true);
  main_layout->addWidget(label_, 1);

  control_panel_ = new QWidget(central_widget);
  QVBoxLayout* panel_layout = new QVBoxLayout(control_panel_);
  main_layout->addWidget(control_panel_);

  auto configureButton = [](QPushButton* button) {
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    button->setMaximumHeight(30);
  };

  auto addButtonRow = [&](std::initializer_list<QPushButton*> buttons) {
    QHBoxLayout* layout = new QHBoxLayout();
    for (auto* btn : buttons) {
      configureButton(btn);
      layout->addWidget(btn);
    }
    panel_layout->addLayout(layout);
  };

  QPushButton *btnObjUp = new QPushButton("Object move up"),
              *btnObjDown = new QPushButton("Object move down"),
              *btnObjLeft = new QPushButton("Object move left"),
              *btnObjRight = new QPushButton("Object move right"),
              *btnObjForward = new QPushButton("Object move forw"),
              *btnObjBackward = new QPushButton("Object move back");
  addButtonRow({btnObjUp, btnObjDown});
  addButtonRow({btnObjLeft, btnObjRight});
  addButtonRow({btnObjForward, btnObjBackward});

  QPushButton *btnCamUp = new QPushButton("Camera move up"),
              *btnCamDown = new QPushButton("Camera move down"),
              *btnCamLeft = new QPushButton("Camera move left"),
              *btnCamRight = new QPushButton("Camera move right"),
              *btnCamForward = new QPushButton("Camera move forw"),
              *btnCamBackward = new QPushButton("Camera move backw");
  addButtonRow({btnCamUp, btnCamDown});
  addButtonRow({btnCamLeft, btnCamRight});
  addButtonRow({btnCamForward, btnCamBackward});

  QPushButton *btnObjRotXPlus = new QPushButton("Object rot aX+"),
              *btnObjRotXMinus = new QPushButton("Object rot aX-"),
              *btnObjRotYPlus = new QPushButton("Object rot aY+"),
              *btnObjRotYMinus = new QPushButton("Object rot aY-"),
              *btnObjRotZPlus = new QPushButton("Object rot aZ+"),
              *btnObjRotZMinus = new QPushButton("Object rot aZ-");
  addButtonRow({btnObjRotXPlus, btnObjRotXMinus});
  addButtonRow({btnObjRotYPlus, btnObjRotYMinus});
  addButtonRow({btnObjRotZPlus, btnObjRotZMinus});

  QPushButton *btnCamRotXPlus = new QPushButton("Camera rotate up"),
              *btnCamRotXMinus = new QPushButton("Camera rotate down"),
              *btnCamRotYPlus = new QPushButton("Camera rotate left"),
              *btnCamRotYMinus = new QPushButton("Camera rotate right");
  addButtonRow({btnCamRotXPlus, btnCamRotXMinus});
  addButtonRow({btnCamRotYPlus, btnCamRotYMinus});

  QPushButton* btnLoadModel = new QPushButton("Load Model");
  configureButton(btnLoadModel);
  panel_layout->addWidget(btnLoadModel);

  setCentralWidget(central_widget);

  constexpr ElemType moveStep = 1;
  constexpr ElemType rotStep = 0.1;
  const Index objectIndex = 0;

  connect(btnObjUp, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, 0, -moveStep, 0); });
  connect(btnObjDown, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, 0, moveStep, 0); });
  connect(btnObjLeft, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, -moveStep, 0, 0); });
  connect(btnObjRight, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, moveStep, 0, 0); });
  connect(btnObjForward, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, 0, 0, moveStep); });
  connect(btnObjBackward, &QPushButton::clicked, this,
          [=]() { emit objectMoveRequested(objectIndex, 0, 0, -moveStep); });

  connect(btnCamUp, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(0, -moveStep, 0); });
  connect(btnCamDown, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(0, moveStep, 0); });
  connect(btnCamLeft, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(-moveStep, 0, 0); });
  connect(btnCamRight, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(moveStep, 0, 0); });
  connect(btnCamForward, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(0, 0, moveStep); });
  connect(btnCamBackward, &QPushButton::clicked, this,
          [=]() { emit cameraMoveRequested(0, 0, -moveStep); });

  connect(btnObjRotXPlus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, rotStep, 0, 0); });
  connect(btnObjRotXMinus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, -rotStep, 0, 0); });
  connect(btnObjRotYPlus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, 0, rotStep, 0); });
  connect(btnObjRotYMinus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, 0, -rotStep, 0); });
  connect(btnObjRotZPlus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, 0, 0, rotStep); });
  connect(btnObjRotZMinus, &QPushButton::clicked, this,
          [=]() { emit objectRotateRequested(objectIndex, 0, 0, -rotStep); });

  connect(btnCamRotXPlus, &QPushButton::clicked, this,
          [=]() { emit cameraRotateRequested(rotStep, 0, 0); });
  connect(btnCamRotXMinus, &QPushButton::clicked, this,
          [=]() { emit cameraRotateRequested(-rotStep, 0, 0); });
  connect(btnCamRotYPlus, &QPushButton::clicked, this,
          [=]() { emit cameraRotateRequested(0, rotStep, 0); });
  connect(btnCamRotYMinus, &QPushButton::clicked, this,
          [=]() { emit cameraRotateRequested(0, -rotStep, 0); });

  // Load model
  connect(btnLoadModel, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Load OBJ Model"), "", tr("OBJ Files (*.obj);;All Files (*)"));
    if (!fileName.isEmpty()) {
      qDebug() << "Loading model from:" << fileName;
      emit modelLoadRequested(fileName);
    }
  });

  connect(this, &View::objectMoveRequested, controller_,
          &Controller::onMoveObject);
  connect(this, &View::objectRotateRequested, controller_,
          &Controller::onRotateObject);
  connect(this, &View::cameraMoveRequested, controller_,
          &Controller::onMoveCamera);
  connect(this, &View::cameraRotateRequested, controller_,
          &Controller::onRotateCamera);
  connect(this, &View::modelLoadRequested, controller_,
          &Controller::onModelLoad);

  port_.SetNotifyAction([this](ScreenPicture& data) { this->Draw(data); });
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

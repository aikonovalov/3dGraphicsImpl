#include "View.h"
#include <QDebug>
#include <QResizeEvent>
#include <vector>
#include "Controller.h"

namespace Core {

View::View(Controller* controller_link)
    : port_(), controller_(controller_link) {
  QWidget* centralWidget = new QWidget(this);
  QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

  label_ = new QLabel(centralWidget);
  label_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label_->setScaledContents(true);
  mainLayout->addWidget(label_, 1);

  controlPanel_ = new QWidget(centralWidget);
  QVBoxLayout* panelLayout = new QVBoxLayout(controlPanel_);

  QHBoxLayout* objTransLayout = new QHBoxLayout();
  QPushButton* btnObjUp = new QPushButton("Объект вверх", controlPanel_);
  QPushButton* btnObjDown = new QPushButton("Объект вниз", controlPanel_);
  QPushButton* btnObjLeft = new QPushButton("Объект влево", controlPanel_);
  QPushButton* btnObjRight = new QPushButton("Объект вправо", controlPanel_);
  QPushButton* btnObjForward = new QPushButton("Объект вперёд", controlPanel_);
  QPushButton* btnObjBackward = new QPushButton("Объект назад", controlPanel_);
  objTransLayout->addWidget(btnObjUp);
  objTransLayout->addWidget(btnObjDown);
  objTransLayout->addWidget(btnObjLeft);
  objTransLayout->addWidget(btnObjRight);
  objTransLayout->addWidget(btnObjForward);
  objTransLayout->addWidget(btnObjBackward);
  panelLayout->addLayout(objTransLayout);

  QHBoxLayout* camTransLayout = new QHBoxLayout();
  QPushButton* btnCamUp = new QPushButton("Камера вверх", controlPanel_);
  QPushButton* btnCamDown = new QPushButton("Камера вниз", controlPanel_);
  QPushButton* btnCamLeft = new QPushButton("Камера влево", controlPanel_);
  QPushButton* btnCamRight = new QPushButton("Камера вправо", controlPanel_);
  QPushButton* btnCamForward = new QPushButton("Камера вперёд", controlPanel_);
  QPushButton* btnCamBackward = new QPushButton("Камера назад", controlPanel_);
  camTransLayout->addWidget(btnCamUp);
  camTransLayout->addWidget(btnCamDown);
  camTransLayout->addWidget(btnCamLeft);
  camTransLayout->addWidget(btnCamRight);
  camTransLayout->addWidget(btnCamForward);
  camTransLayout->addWidget(btnCamBackward);
  panelLayout->addLayout(camTransLayout);

  QHBoxLayout* objRotLayout = new QHBoxLayout();
  QPushButton* btnObjRotXPlus = new QPushButton("Объект рот X+", controlPanel_);
  QPushButton* btnObjRotXMinus =
      new QPushButton("Объект рот X-", controlPanel_);
  QPushButton* btnObjRotYPlus = new QPushButton("Объект рот Y+", controlPanel_);
  QPushButton* btnObjRotYMinus =
      new QPushButton("Объект рот Y-", controlPanel_);
  QPushButton* btnObjRotZPlus = new QPushButton("Объект рот Z+", controlPanel_);
  QPushButton* btnObjRotZMinus =
      new QPushButton("Объект рот Z-", controlPanel_);
  objRotLayout->addWidget(btnObjRotXPlus);
  objRotLayout->addWidget(btnObjRotXMinus);
  objRotLayout->addWidget(btnObjRotYPlus);
  objRotLayout->addWidget(btnObjRotYMinus);
  objRotLayout->addWidget(btnObjRotZPlus);
  objRotLayout->addWidget(btnObjRotZMinus);
  panelLayout->addLayout(objRotLayout);

  // ----- Группа кнопок для вращения камеры -----
  QHBoxLayout* camRotLayout = new QHBoxLayout();
  QPushButton* btnCamRotXPlus =
      new QPushButton("Камера рот гор+", controlPanel_);
  QPushButton* btnCamRotXMinus =
      new QPushButton("Камера рот гор-", controlPanel_);
  QPushButton* btnCamRotYPlus =
      new QPushButton("Камера рот верт+", controlPanel_);
  QPushButton* btnCamRotYMinus =
      new QPushButton("Камера рот верт-", controlPanel_);
  camRotLayout->addWidget(btnCamRotXPlus);
  camRotLayout->addWidget(btnCamRotXMinus);
  camRotLayout->addWidget(btnCamRotYPlus);
  camRotLayout->addWidget(btnCamRotYMinus);

  panelLayout->addLayout(camRotLayout);

  mainLayout->addWidget(controlPanel_);
  setCentralWidget(centralWidget);

  constexpr ElemType moveStep = 1;
  constexpr ElemType rotStep = 0.1;
  const Index objectIndex = 0;

  connect(btnObjUp, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, 0, -moveStep, 0);
          });
  connect(btnObjDown, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, 0, moveStep, 0);
          });
  connect(btnObjLeft, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, -moveStep, 0, 0);
          });
  connect(btnObjRight, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, moveStep, 0, 0);
          });
  connect(btnObjForward, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, 0, 0, moveStep);
          });
  connect(btnObjBackward, &QPushButton::clicked, this,
          [this, moveStep, objectIndex]() {
            emit objectMoveRequested(objectIndex, 0, 0, -moveStep);
          });

  connect(btnCamUp, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(0, -moveStep, 0); });
  connect(btnCamDown, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(0, moveStep, 0); });
  connect(btnCamLeft, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(-moveStep, 0, 0); });
  connect(btnCamRight, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(moveStep, 0, 0); });
  connect(btnCamForward, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(0, 0, moveStep); });
  connect(btnCamBackward, &QPushButton::clicked, this,
          [this, moveStep]() { emit cameraMoveRequested(0, 0, -moveStep); });

  connect(btnObjRotXPlus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, rotStep, 0, 0);
          });
  connect(btnObjRotXMinus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, -rotStep, 0, 0);
          });
  connect(btnObjRotYPlus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, 0, rotStep, 0);
          });
  connect(btnObjRotYMinus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, 0, -rotStep, 0);
          });
  connect(btnObjRotZPlus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, 0, 0, rotStep);
          });
  connect(btnObjRotZMinus, &QPushButton::clicked, this,
          [this, rotStep, objectIndex]() {
            emit objectRotateRequested(objectIndex, 0, 0, -rotStep);
          });

  connect(btnCamRotXPlus, &QPushButton::clicked, this,
          [this, rotStep]() { emit cameraRotateRequested(rotStep, 0, 0); });
  connect(btnCamRotXMinus, &QPushButton::clicked, this,
          [this, rotStep]() { emit cameraRotateRequested(-rotStep, 0, 0); });
  connect(btnCamRotYPlus, &QPushButton::clicked, this,
          [this, rotStep]() { emit cameraRotateRequested(0, rotStep, 0); });
  connect(btnCamRotYMinus, &QPushButton::clicked, this,
          [this, rotStep]() { emit cameraRotateRequested(0, -rotStep, 0); });

  connect(this,
          SIGNAL(objectMoveRequested(Index, ElemType, ElemType, ElemType)),
          controller_, SLOT(onMoveObject(Index, ElemType, ElemType, ElemType)));
  connect(
      this, SIGNAL(objectRotateRequested(Index, ElemType, ElemType, ElemType)),
      controller_, SLOT(onRotateObject(Index, ElemType, ElemType, ElemType)));
  connect(this, SIGNAL(cameraMoveRequested(ElemType, ElemType, ElemType)),
          controller_, SLOT(onMoveCamera(ElemType, ElemType, ElemType)));
  connect(this, SIGNAL(cameraRotateRequested(ElemType, ElemType, ElemType)),
          controller_, SLOT(onRotateCamera(ElemType, ElemType)));

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

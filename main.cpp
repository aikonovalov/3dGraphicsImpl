#include <QApplication>
#include "Core/Controller.h"
#include "Core/Model.h"
#include "Core/View.h"
#include "MathUtils/Point4.h"
#include "Object/Object.h"
#include "Object/TriangleData.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  Core::Model model{};
  Core::Controller controller_{&model};
  Core::View view{&controller_};

  controller_.AddLight(
      {Detail::Light::LightType::Point, {}, Linear::Point4{10, 10, 10, 0}});

  return app.exec();
}

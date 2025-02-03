#pragma once

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <string>
#include <utility>

namespace Application {

class App {
public:
  App(const std::pair<int, char**>& args,
      const std::pair<int, int>& resolution = {800, 600});
  void SetTitle(const std::string&);

  int Execute();

private:
  std::pair<int, char**> args_;
  std::pair<int, int> resolution_;
  QApplication app_;
  QMainWindow window_;
};

}  // namespace Application

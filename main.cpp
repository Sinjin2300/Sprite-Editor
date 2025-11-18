#include "model.h"
#include "view.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Model model;
  View view(model);
  view.show();
  return app.exec();
}

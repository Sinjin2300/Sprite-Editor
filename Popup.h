#ifndef POPUP_H
#define POPUP_H

#include "model.h"
#include "qwidget.h"
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
using std::string;
class Popup : public QWidget {
  Q_OBJECT

public:
  Popup(Model &model, QWidget *parent = 0);
  ~Popup();
  int frameSize;
  Model *m;

private slots:
  void onEightButtonClicked();
  void onSixteenButtonClicked();
  void onThirtyTwoButtonClicked();
  void onSixtyFourButtonClicked();
};

#endif // POPUP_H

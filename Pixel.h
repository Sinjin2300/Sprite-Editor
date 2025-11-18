#ifndef PIXEL_H
#define PIXEL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
class Pixel {
public:
  int r;
  int g;
  int b;
  int a;
  Pixel();
  void read(QJsonObject &json);
  void write(QJsonObject &json);
};

#endif // PIXEL_H

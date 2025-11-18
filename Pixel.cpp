#include "Pixel.h"
///
/// \brief Pixel::Pixel default constructor
///
Pixel::Pixel() {
  r = 0;
  g = 0;
  b = 0;
  a = 0;
}

///
/// \brief Pixel::write writes rgba values to JSON
/// \param json
///
void Pixel::write(QJsonObject &json) {
  json["r"] = r;
  json["g"] = g;
  json["b"] = b;
  json["a"] = a;
}

///
/// \brief Pixel::read reads rgba values from JSON and turns it back into a
/// pixel
/// \param json
///
void Pixel::read(QJsonObject &json) {
  if (json.contains("r") && json["r"].isDouble()) {
    r = json["r"].toInt();
  }
  if (json.contains("g") && json["g"].isDouble()) {
    g = json["g"].toInt();
  }
  if (json.contains("b") && json["b"].isDouble()) {
    b = json["b"].toInt();
  }
  if (json.contains("a") && json["a"].isDouble()) {
    a = json["a"].toInt();
  }
}

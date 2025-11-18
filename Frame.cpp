#include "Frame.h"
#include "Pixel.h"
#include <QPainter>
///
/// \brief Frame Constructor
/// \param size The size in pixels
///
Frame::Frame(int size) {
  Layer newLayer = Layer{size};
  layers.append(newLayer);
  currentLayer = &layers[0];
  frameObjectName = "";
}

///
/// \brief Frame::Frame copy constructor
/// \param other
///
Frame::Frame(Frame &other) {

  for (unsigned int i = 0; i < other.layers.size(); i++) {
    Layer newLayer = Layer{1};
    newLayer.image = other.layers[i].image;
    newLayer.visible = other.layers[i].visible;
    layers.append(newLayer);
  }
  currentLayer = &layers[0];
  frameObjectName = "";
}

///
/// \brief Composites the layers into a single QImage
/// \return The composited QImage
///
QImage Frame::getComposite() {
  if (layers.isEmpty()) {
    return QImage();
  }

  int size = layers[0].image.size().width();
  QImage compImage(size, size, QImage::Format_ARGB32_Premultiplied);
  compImage.fill(QColor{255, 255, 255, 0});
  QPainter painter;

  painter.begin(&compImage);

  for (int i = layers.size() - 1; i > -1; i--) {
    if (layers[i].visible) {
      painter.drawImage(QPoint(0, 0), layers.at(i).image);
    }
  }

  painter.end();
  return compImage;
}

///
/// \brief Frame::write writes the frame into JSON Format
/// \param json
///
void Frame::write(QJsonObject &json) {
  json["name"] = frameObjectName;
  QImage writeImage = getComposite();
  QJsonArray imageArray;
  // for every pixel height wise
  for (int i = 0; i < writeImage.height(); i++) {
    QJsonArray rowArray;
    // for every pixel width wise creates the oixel and writes it to JSON
    for (int j = 0; j < writeImage.width(); j++) {
      QColor pixelColor = writeImage.pixelColor(i, j);
      Pixel pixel;
      pixel.a = pixelColor.alpha();
      pixel.r = pixelColor.red();
      pixel.g = pixelColor.green();
      pixel.b = pixelColor.blue();
      QJsonObject pixelObject;
      pixel.write(pixelObject);
      rowArray.append(pixelObject);
    }
    imageArray.append(rowArray);
  }
  json["arrayOfRows"] = imageArray;
}

///
/// \brief Frame::read reads the frame from JSON format and turns it back into a
/// frame
/// \param json
///
void Frame::read(QJsonObject &json) {
  if (json.contains("name") && json["name"].isString()) {
    frameObjectName = json["name"].toString();
  }
  // builds the array of rows and each row has an array of pixels
  if (json.contains("arrayOfRows") && json["arrayOfRows"].isArray()) {
    QJsonArray imgArray = json["arrayOfRows"].toArray();
    int pixelX = 0;
    int pixelY = 0;

    for (QJsonValue v : imgArray) {
      QJsonArray rwArray = v.toArray();
      // rebuilds the image pixel by pixel
      for (QJsonValue v : rwArray) {
        QJsonObject pixelObject = v.toObject();
        Pixel pixel;
        pixel.read(pixelObject);
        QColor color;
        color.setRed(pixel.r);
        color.setGreen(pixel.g);
        color.setBlue(pixel.b);
        color.setAlpha(pixel.a);
        currentLayer->image.setPixelColor(pixelX, pixelY, color);
        pixelX += 1;
      }
      pixelX = 0;
      pixelY += 1;
    }
    QTransform rotating;
    rotating.rotate(90);
    currentLayer->image = currentLayer->image.transformed(rotating);
  }
}

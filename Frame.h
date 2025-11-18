#include <QApplication>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QMatrix2x2>
#include <QPainter>
#include <QString>
#include <QVector>
struct Layer {
  QImage image;
  QString name;
  bool visible;

  Layer(int size) {
    image = QImage{size, size, QImage::Format_ARGB32_Premultiplied};
    image.fill(QColor{255, 255, 255, 0});
    name = QString("New Layer");
    visible = true;
  }
};

class Frame {
public:
  // Members
  Layer *currentLayer = NULL;
  int currentLayerNum = 0;
  QVector<Layer> layers;
  QString frameObjectName;

  // Methods
  Frame(int size);
  Frame();
  Frame(Frame &other);
  QImage getComposite();
  QImage readImage();
  // void paintEvent(QPaintEvent* event) override;
  void read(QJsonObject &json);
  void write(QJsonObject &json);
};

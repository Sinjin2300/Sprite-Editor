#ifndef MODEL_H
#define MODEL_H

#include "Frame.h"
#include "QPainter"
#include <QColorDialog>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QMouseEvent>
#include <QObject>
#include <QTimer>
#include <iostream>
#include <map>
#include <vector>

using std::map;
using std::string;
using std::vector;
class Model : public QObject {
  Q_OBJECT
public:
  explicit Model(QObject *parent = nullptr);
  QColorDialog dialogBox;

  int currentFrameNum;

  // Frame Stuff
  Frame *currentFrame;
  Frame *copyFrame;
  vector<Frame *> frames;
  unsigned int currentPreviewFrame;
  int addFrameIndex;
  int indexOfFrame(Frame *);
  Frame findFrame(QString name);
  bool draw;

  // Saving methods
  void read(QJsonObject &json);
  void write(QJsonObject &json);
  void setSize(int size);
  void saveProject(QString fileName);
  void loadProject(QString fileName);
  void savePNG(QString fileName);
  void saveGIF(QString fileName);

public slots:
  // Toolbox slots
  void cursorButtonClicked();
  void penButtonClicked();
  void eraserButtonClicked();
  void bucketButtonClicked();

  // Color change slots
  void CustomColorButtonClicked();
  void colorSelected(const QColor &color);
  void setOpacity(int);

  // Tool Bar slots
  void addBlankLayer();
  void RenameLayer(QString name);
  void deleteSelectedLayer();
  void moveSelectedLayerUp();
  void moveSelectedLayerDown();
  void addNewFrameClicked();
  void removeFrameClicked();
  void copyFrameClicked();

  // Frame selector slots
  void leftScrollButtonClicked();
  void rightScrollButtonClicked();
  void receiveFrameRate(int);

  // Frame Editor slots
  void mousePressed(QMouseEvent *);
  void mouseMove(QMouseEvent *);
  void mouseReleased(QMouseEvent *);

  // Layer Slots
  void updateVisibility(int, bool);
  void setLayerSelect(int);

signals:
  void giveWarningMessage();
  void setCursorButton(QString);
  void setPenButton(QString);
  void setEraserButton(QString);
  void setBucketButton(QString);
  void drawTheFrameInSpriteEditorPanel();
  void setFrameHighlight(int idx);
  void addANewFrameOnUi();
  void removeFrameOnUi();
  void setPreviewImage(QPixmap);
  void setImageEditor(QPixmap);
  void newLayer();
  void updateLayerName(QString name);
  void updateLayers(QVector<Layer>, int);
  void changeSelectedLayer(QMouseEvent *event);

private:
  void resetToolButtons();
  void displayNextPreviewFrame();
  void handleLeftScroll();
  void handleRightScroll();
  void resetAllHighlightedFrame();
  void setFrameHighlighted(int);
  void editFramePixels(QMouseEvent *event);
  void updateImageEditor();

  // Tool enum for the toolbox.
  enum Tool { cursor, pen, eraser, bucket };
  Tool currentTool;
  QPainter painter;
  QColor currentColor;
  int currentAlpha; // opacity
  int imageSize;
  int height;
  int width;
  int numOfFrames;
  int frameRate; // For preview
};

#endif // MODEL_H

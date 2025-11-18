#ifndef VIEW_H
#define VIEW_H

#include "Popup.h"
#include "model.h"
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

// Struct Def
struct layerFrame {
  QFrame *frame;
  QCheckBox *visBox;
  QLabel *name;
  QLabel *image;
};

class View : public QMainWindow {
  Q_OBJECT

public:
  explicit View(Model &model, QWidget *parent = nullptr);
  ~View();
  Model *m;
  Popup *frameSizePopup;
  Popup *renamePopup;
  int currentFrameNum;

signals:
  void setframeSelection();
  void viewMouseClick(QMouseEvent *event);
  void viewMouseMovement(QMouseEvent *event);
  void viewMouseReleaseEvent(QMouseEvent *event);
  void addFrameMenuItemClicked();
  void setVis(int, bool);
  void layerSelectionIndex(int);

private slots:
  void addLayer(int pos);
  void extracted(QVector<Layer> &layers);
  void updateLayers(QVector<Layer>, int);
  void selectLayer(QMouseEvent *);
  void boxChecked(bool);
  void showFrameSizePopup();

protected:
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
  Ui::View *ui;
  QVBoxLayout *layerLayout;
  QVector<layerFrame> layerFrames;

  QVector<QLabel *> frameLabels;
  void appendANewFrameOnUi();
  void removeFrameOnUi();
  void appendCopyFrameOnUi();
  void highlightFrameInFrameMenu(int);
  void CanNotDeleteFrameWarningMessageBox();
  void saveFileDialog();
  void loadFileDialog();
  void frameMenuPreview();
  void savePNGDialog();
  void saveGIFDialog();
  void setSelectedLayer(int);
};

#endif // VIEW_H

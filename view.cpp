/**
 * View class that handles all logic concerning the user interface and how to
 * display pixel edits made by the users.
 **/

#include "view.h"
#include "ui_view.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QScrollBar>
#include <string>

using std::string;

View::View(Model &model, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::View) {
  currentFrameNum = 1;

  m = &model;
  ui->setupUi(this);

  // set default background color for ui components
  ui->CursorButton->setStyleSheet(
      QString("QToolButton {background-color: rgb(200, 200, 255);}"));
  ui->PenButton->setStyleSheet(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  ui->EraserButton->setStyleSheet(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  ui->BucketButton->setStyleSheet(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  ui->CustomColorButton->setStyleSheet(QString(
      "QPushButton {background-color: rgb(255, 255, 255);color: black;}"));
  ui->ScrollLeft->setStyleSheet(QString(
      "QPushButton {background-color: rgb(255, 255, 255); color: black;}"));
  ui->ScrollRight->setStyleSheet(QString(
      "QPushButton {background-color: rgb(255, 255, 255);color: black;}"));
  ui->SetFrames->setStyleSheet(QString(
      "QTextEdit {background-color: rgb(255, 255, 255); color: black;}"));
  ui->SetOpacity->setStyleSheet(QString(
      "QTextEdit {background-color: rgb(255, 255, 255); color: black;}"));
  ui->FramesPerSecond->setStyleSheet(
      QString("QSpinBox {background-color: rgb(255, 255, 255);color: black;}"));
  ui->OpacityBox->setStyleSheet(
      QString("QSpinBox {background-color: rgb(255, 255, 255);color: black;}"));
  ui->ImageEditor->setScaledContents(true);
  ui->ImageEditor->setStyleSheet(QString("QFrame {border: 1px solid white;}"));
  appendANewFrameOnUi();

  // setting icons
  QImage penImage(":/icons/Pen.png");
  QPixmap penPixmap = QPixmap::fromImage(penImage.scaled(75, 75));
  ui->PenButton->setIcon(QIcon(penPixmap));
  QImage BucketImage(":/icons/Bucket.png");
  QPixmap BucketPixmap = QPixmap::fromImage(BucketImage.scaled(75, 75));
  ui->BucketButton->setIcon(QIcon(BucketPixmap));
  QImage CursorImage(":/icons/Cursor.png");
  QPixmap CursorPixmap = QPixmap::fromImage(CursorImage.scaled(75, 75));
  ui->CursorButton->setIcon(QIcon(CursorPixmap));
  QImage EraserImage(":/icons/Eraser.png");
  QPixmap EraserPixmap = QPixmap::fromImage(EraserImage.scaled(75, 75));
  ui->EraserButton->setIcon(QIcon(EraserPixmap));

  // Setting up Layer Layout
  layerLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
  layerLayout->setAlignment(Qt::AlignTop);
  layerLayout->setContentsMargins(QMargins(2, 2, 10, 5));
  addLayer(-1);
  setSelectedLayer(0);

  // Connections

  // Layer Menu Connections
  connect(this, &View::setVis, &model, &Model::updateVisibility);
  connect(&model, &Model::updateLayers, this, &View::updateLayers);
  connect(&model, &Model::changeSelectedLayer, this, &View::selectLayer);
  connect(this, &View::layerSelectionIndex, &model, &Model::setLayerSelect);

  // Toolbox Connections
  connect(ui->CursorButton, &QToolButton::clicked, &model,
          &Model::cursorButtonClicked);
  connect(ui->PenButton, &QToolButton::clicked, &model,
          &Model::penButtonClicked);
  connect(ui->EraserButton, &QToolButton::clicked, &model,
          &Model::eraserButtonClicked);
  connect(ui->BucketButton, &QToolButton::clicked, &model,
          &Model::bucketButtonClicked);
  connect(&model, &Model::setCursorButton, ui->CursorButton,
          &QToolButton::setStyleSheet);
  connect(&model, &Model::setPenButton, ui->PenButton,
          &QToolButton::setStyleSheet);
  connect(&model, &Model::setEraserButton, ui->EraserButton,
          &QToolButton::setStyleSheet);
  connect(&model, &Model::setBucketButton, ui->BucketButton,
          &QToolButton::setStyleSheet);

  // Image Editor connections
  connect(this, &View::viewMouseClick, &model, &Model::mousePressed);
  connect(this, &View::viewMouseMovement, &model, &Model::mouseMove);
  connect(this, &View::viewMouseReleaseEvent, &model, &Model::mouseReleased);
  connect(&model, &Model::setImageEditor, ui->ImageEditor, &QLabel::setPixmap);
  connect(&model, &Model::setImageEditor, this, &View::frameMenuPreview);

  // Toolbar Connections
  connect(ui->actionAdd_Blank_Layer_Below, &QAction::triggered, &model,
          &Model::addBlankLayer);
  connect(ui->actionFrame_as_PNG, &QAction::triggered, this,
          &View::savePNGDialog);
  connect(ui->actionGif_Export, &QAction::triggered, this,
          &View::saveGIFDialog);
  connect(ui->actionDelete_Selected_Layer, &QAction::triggered, &model,
          &Model::deleteSelectedLayer);
  connect(ui->actionMove_Selected_Layer_Down, &QAction::triggered, &model,
          &Model::moveSelectedLayerDown);
  connect(ui->actionMove_Selected_Layer_Up, &QAction::triggered, &model,
          &Model::moveSelectedLayerUp);
  connect(ui->NewProjectAction, &QAction::triggered, this,
          &View::showFrameSizePopup);
  connect(ui->SaveProjectAction, &QAction::triggered, this,
          &View::saveFileDialog);
  connect(ui->LoadProjectAction, &QAction::triggered, this,
          &View::loadFileDialog);

  // Color Palette connections
  connect(ui->CustomColorButton, &QPushButton::clicked, &model,
          &Model::CustomColorButtonClicked);
  connect(&model.dialogBox, &QColorDialog::colorSelected, &model,
          &Model::colorSelected);
  connect(ui->OpacityBox, &QSpinBox::valueChanged, &model, &Model::setOpacity);

  // Frame Menu connections
  connect(ui->ScrollLeft, &QPushButton::clicked, &model,
          &Model::leftScrollButtonClicked);
  connect(ui->ScrollRight, &QPushButton::clicked, &model,
          &Model::rightScrollButtonClicked);
  connect(&model, &Model::giveWarningMessage, this,
          &View::CanNotDeleteFrameWarningMessageBox);
  connect(ui->actionAdd_a_new_frame, &QAction::triggered, &model,
          &Model::addNewFrameClicked);
  connect(&model, &Model::addANewFrameOnUi, this, &View::appendANewFrameOnUi);
  connect(&model, &Model::setFrameHighlight, this,
          &View::highlightFrameInFrameMenu);
  connect(ui->actionRemove_current_frame, &QAction::triggered, &model,
          &Model::removeFrameClicked);
  connect(&model, &Model::removeFrameOnUi, this, &View::removeFrameOnUi);
  connect(ui->actionDuplicate_current_frame, &QAction::triggered, &model,
          &Model::copyFrameClicked);
  connect(&model, &Model::addANewFrameOnUi, this, &View::appendCopyFrameOnUi);
  connect(this, &View::viewMouseClick, &model, &Model::mousePressed);

  // Sprite Preview Menu connections
  connect(&model, &Model::setPreviewImage, ui->PreviewLabel,
          &QLabel::setPixmap);
  connect(ui->FramesPerSecond, &QSpinBox::valueChanged, &model,
          &Model::receiveFrameRate);

  // Frame size popup
  frameSizePopup = new Popup(*m);
  frameSizePopup->hide();

  renamePopup = new Popup(*m);
  renamePopup->hide();

  ui->ImageEditor->setScaledContents(true);
  ui->ImageEditor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  ui->PreviewLabel->setScaledContents(true);
  ui->PreviewLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

///
/// \brief pop up a save file dialog to save the sprite project
///
void View::saveFileDialog() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Project"), "", tr("Json Files (*.ssp);;All Files (*)"));
  if (fileName.isEmpty()) {
    return;
  } else {
    // write data to file...
    m->saveProject(fileName);
  }
}

///
/// \brief pop up a file dialog to load the sprite project
///
void View::loadFileDialog() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Import Project"), "", tr("Json Files (*.ssp);;All Files (*)"));

  if (fileName.isEmpty()) {
    return;
  } else {
    // call method to read Json and write data into our model
    m->loadProject(fileName);
  }
}

///
/// \brief pop up a file dialog to save current project to GIF file
///
void View::saveGIFDialog() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Project"), "", tr("GIF Files (*.gif);;All Files (*)"));
  if (fileName.isEmpty()) {
    return;
  } else {
    // write data to file...
    m->saveGIF(fileName);
  }
}

///
/// \brief pop up a file dialog to save current project to PNG files
///
void View::savePNGDialog() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Project"), "", tr("PNG Files (*.png);;All Files (*)"));
  if (fileName.isEmpty()) {
    return;
  } else {
    // write data to file...
    m->savePNG(fileName);
  }
}

///
/// \brief update the preview of current frame on frame menu
///
void View::frameMenuPreview() {
  string currentFrameName = "frame_" + std::to_string(currentFrameNum);

  QLabel *frame = frameLabels[currentFrameNum - 1];
  QPainter painter(frameLabels[currentFrameNum - 1]);

  QPixmap pixmap =
      QPixmap::fromImage(m->currentFrame->getComposite().scaled(89, 89));
  frame->setPixmap(pixmap);

  frame->show();
}

///
/// \brief highlight the selected frame in the frameMenu
/// \param the index of frame in the FrameMenu
///
void View::highlightFrameInFrameMenu(int idx) {
  currentFrameNum = idx;

  // Iterate over the list of QFrame widgets
  if (frameLabels.isEmpty()) {
    return;
  }
  for (QLabel *frame : frameLabels) {
    // reset the border to the default
    frame->setStyleSheet("QLabel {border: 1px solid black;}");
  }
  // update the selected/current frame
  frameLabels[currentFrameNum - 1]->setStyleSheet(
      "QLabel {border: 5px solid red;}");
}

///
/// \brief poping up a warning message when user try to delete the last frame of
/// a project
///
void View::CanNotDeleteFrameWarningMessageBox() {
  QMessageBox::information(nullptr, "Warning Message",
                           "You can not delete current frame, A sprite project "
                           "must have one frame.");
}

///
/// \brief append a new frame on the frame menu
///
void View::appendANewFrameOnUi() {
  string newFrameName = "frame_" + std::to_string(m->frames.size());
  QLabel *newFrame = new QLabel();
  newFrame->setFixedSize(89, 89);

  // edge case: when appending frame is the only frame on the frame menu, make
  // it highlighted
  if (m->frames.size() == 1) {
    newFrame->setStyleSheet(QString("QLabel {border: 5px solid red;}"));
  } else { // otherwise, just addind a normal frame
    newFrame->setStyleSheet(QString("QLabel {border: 1px solid black;}"));
  }
  newFrame->setObjectName(newFrameName);

  frameLabels.push_back(
      newFrame); // Need this for easy access to these labels for later on...
  ui->scrollAreaWidgetContents_2->layout()->addWidget(newFrame);
}

///
/// \brief append a copy of a frame on the frame menu
///
void View::appendCopyFrameOnUi() {
  string newFrameName = "frame_" + std::to_string(m->frames.size());
  QLabel *newFrame = new QLabel();
  newFrame->setFixedSize(89, 89);

  newFrame->setStyleSheet(QString("QLabel {border: 1px solid black;}"));
  newFrame->setObjectName(newFrameName);
  QPainter painter(frameLabels[m->frames.size() - 1]);

  QPixmap pixmap = QPixmap::fromImage(
      m->frames[m->frames.size() - 1]->getComposite().scaled(89, 89));
  frameLabels[m->frames.size() - 1]->setPixmap(pixmap);

  frameLabels[m->frames.size() - 1]->show();
}

///
/// \brief View::removeFrameOnUi - removes the frame from the user display
///
void View::removeFrameOnUi() {
  ui->scrollAreaWidgetContents_2->layout()->removeWidget(
      frameLabels[currentFrameNum - 1]);
  delete frameLabels[currentFrameNum - 1];
  frameLabels.erase(frameLabels.begin() + (currentFrameNum - 1));
  if (currentFrameNum - 1 == 0 && frameLabels.isEmpty()) {
    return;
  }
}

///
/// \brief View::showFrameSizePopup - shows the frame size popup
///
void View::showFrameSizePopup() { frameSizePopup->show(); }

///
/// \brief View::mousePressEvent - sends the mouse click event to be handled by
/// the model
/// \param event
///
void View::mousePressEvent(QMouseEvent *event) { emit viewMouseClick(event); }

///
/// \brief View::mouseMoveEvent - sends the mouse moved event to be handled by
/// the model
/// \param event
///
void View::mouseMoveEvent(QMouseEvent *event) { emit viewMouseMovement(event); }

///
/// \brief View::mouseReleaseEvent - sends the mouse released event to be
/// handled by the model
/// \param event
///
void View::mouseReleaseEvent(QMouseEvent *event) {
  emit viewMouseReleaseEvent(event);
}

View::~View() { delete ui; }

///
/// \brief Used to update the backend when a checkbox is clicked on a layer
/// \param The new state of the checkbox
///
void View::boxChecked(bool newState) {
  QCheckBox *checkbox = qobject_cast<QCheckBox *>(sender());
  for (int i = 0; i < layerFrames.count(); i++) {
    if (layerFrames[i].visBox == checkbox) {
      // Set the appropriate value in layers
      emit setVis(i, newState);
      break;
    }
  }
}

void View::selectLayer(QMouseEvent *event) {
  QPoint position = event->pos();
  // childAt method works off of relative position, so we have to subtract off
  // top left corner.
  QPoint corner(1020, 360);
  position -= corner;
  int scrollValue = ui->layerContainer->verticalScrollBar()->value();
  if (scrollValue != 0) {
    QPoint scrollOffset = QPoint(0, scrollValue);
    position += scrollOffset;
  }

  QWidget *layerClicked = ui->scrollAreaWidgetContents->childAt(position);

  if (layerClicked == nullptr) {
    return;
  }

  int layerIndex = 0;
  for (int i = 0; i < layerFrames.count(); i++) {
    if (layerFrames[i].frame == layerClicked ||
        layerFrames[i].image == layerClicked ||
        layerFrames[i].name == layerClicked) {
      layerIndex = i;
      break;
    }
  }

  setSelectedLayer(layerIndex);
  emit layerSelectionIndex(layerIndex);
}

///
/// \brief View::setSelectedLayer - displays which layer is currently selected
/// \param layerIndex
///
void View::setSelectedLayer(int layerIndex) {
  for (int i = 0; i < layerFrames.count(); i++) {
    if (layerIndex == i) {
      layerFrames[i].frame->setStyleSheet(QString("border: 2px solid red"));
    } else {
      layerFrames[i].frame->setStyleSheet(QString("border: 1px solid gray"));
    }
  }
}

///
/// \brief Used to add a new blank layer to the UI
/// \param The index to be added at
///
void View::addLayer(int pos) {
  // Build a new layer object and add it to the menu
  QFrame *layerFrame = new QFrame{};
  layerFrame->setLayout(new QHBoxLayout());
  layerFrame->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  layerFrame->setStyleSheet(QString("border: 1px solid gray;"));
  QLabel *preview = new QLabel{layerFrame};
  preview->setMaximumSize(QSize(64, 64));
  preview->setMinimumSize(QSize(64, 64));
  preview->setScaledContents(true);
  preview->setStyleSheet(QString("border-style: none;"));
  layerFrame->layout()->addWidget(preview);
  QLabel *layerName = new QLabel{layerFrame};
  layerName->setText(QString("New Layer"));
  layerName->setStyleSheet(QString("border-style: none;"));
  layerFrame->layout()->addWidget(layerName);
  QCheckBox *visBox = new QCheckBox{layerFrame};
  layerFrame->layout()->addWidget(visBox);
  visBox->setStyleSheet(QString("border-style: none;"));

  // Update Frame Specifics
  layerFrame->setMinimumSize(QSize{230, 80});
  layerFrame->setMaximumSize(QSize{230, 80});
  layerLayout->insertWidget(pos, layerFrame);
  struct layerFrame fin;
  fin.frame = layerFrame;
  fin.image = preview;
  fin.visBox = visBox;
  fin.name = layerName;
  layerFrames.append(fin);
  connect(visBox, &QCheckBox::clicked, this, &View::boxChecked);
}

///
/// \brief Helper method to update all of the layer UI boxes with information
/// from the backend
/// \param The list of layers passed from the backend
///
void View::extracted(QVector<Layer> &layers) {
  for (Layer l : layers) {
    addLayer(-1);
    layerFrame curr = layerFrames.last();
    curr.name->setText(l.name);
    curr.image->setPixmap(QPixmap::fromImage(l.image.scaled(QSize(64, 64))));
    curr.visBox->setChecked(l.visible);
  }
}

void View::updateLayers(QVector<Layer> layers, int index) {
  QLayoutItem *item;
  while ((item = layerLayout->takeAt(0)) != nullptr) {
    layerLayout->removeItem(item);
    delete item->widget();
  }
  layerFrames.clear();
  extracted(layers);
  setSelectedLayer(index);
}

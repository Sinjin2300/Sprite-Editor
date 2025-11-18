/**
 *Model class for the sprite editor - handles all of the underlying logic
 **/

#include "model.h"
#include "gif.h"
#include <QFile>
#include <QJsonDocument>
#include <QPointF>
#include <QTimer>
#include <stdlib.h>
#include <unistd.h>

Model::Model(QObject *parent) : QObject{parent} {
  // Presets when application opens.
  imageSize = 8;
  height = imageSize;
  width = imageSize;
  currentFrame = new Frame(imageSize);
  currentColor = QColor{255, 255, 255, 0};
  currentAlpha = 255;

  // Set up first frame:
  currentFrameNum = 1;
  frames.push_back(currentFrame);
  frameRate = 1;
  currentPreviewFrame = 0;
  copyFrame = nullptr;
  addFrameIndex = 1;

  displayNextPreviewFrame();
  emit setFrameHighlight(1);
}

// ***TOOLBOX AND EDITING***

///
/// \brief Model::cursorButtonClicked - sets the current tool to cursor
///
void Model::cursorButtonClicked() {
  currentTool = Tool::cursor;
  resetToolButtons();
  emit setCursorButton(
      QString("QToolButton {background-color: rgb(200, 200, 255);}"));
}

///
/// \brief Model::penButtonClicked - sets the current tool to pen
///
void Model::penButtonClicked() {
  currentTool = Tool::pen;
  resetToolButtons();
  emit setPenButton(
      QString("QToolButton {background-color: rgb(200, 200, 255);}"));
}

///
/// \brief Model::eraserButtonClicked - sets the current tool to eraser
///
void Model::eraserButtonClicked() {
  currentTool = Tool::eraser;
  resetToolButtons();
  emit setEraserButton(
      QString("QToolButton {background-color: rgb(200, 200, 255);}"));
}

///
/// \brief Model::bucketButtonClicked - sets the current tool to bucket
///
void Model::bucketButtonClicked() {
  currentTool = Tool::bucket;
  resetToolButtons();
  emit setBucketButton(
      QString("QToolButton {background-color: rgb(200, 200, 255);}"));
}

///
/// \brief Model::resetToolButtons - visually deselects the tools from the
/// toolbox
///
void Model::resetToolButtons() {
  emit setCursorButton(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  emit setPenButton(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  emit setEraserButton(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
  emit setBucketButton(
      QString("QToolButton {background-color: rgb(255, 255, 255);}"));
}

///
/// \brief Model::editFramePixels - edits the current layer of the current
/// frame. The edits made to the current layer are determined by the current
/// tool that is selected. The cursor tool prevents edits from being made, the
/// bucket makes the entire layer one color, the pen fills selected pixels with
/// the current color selected, and the eraser removes color from selected
/// pixels.
/// \param event
///
void Model::editFramePixels(QMouseEvent *event) {
  // The cursor tool does not edit the pixels.
  if (currentTool == Tool::cursor) {
    return;
  }

  // Get the true color selected by combining the color and the alpha.
  QColor trueColor = QColor{currentColor.red(), currentColor.green(),
                            currentColor.blue(), currentAlpha};

  // Set the entire layer to the selected color.
  if (currentTool == Tool::bucket) {
    currentFrame->currentLayer->image.fill(trueColor);
  }

  // Get the pixel at the position where the mouse was clicked.
  float convertedMouseX = (event->position().x() - 410) / 480;
  float convertedMouseY = (event->position().y() - 30) / 480;
  int pixelX = convertedMouseX * imageSize;
  int pixelY = convertedMouseY * imageSize;

  // Set the pixel at the mouse click to the color selected if using the pen.
  if (currentTool == Tool::pen) {
    currentFrame->currentLayer->image.setPixelColor(pixelX, pixelY, trueColor);
  }

  // Set the pixel color to white and alpha value to transparent if erased.
  if (currentTool == Tool::eraser) {
    currentFrame->currentLayer->image.setPixelColor(pixelX, pixelY,
                                                    QColor{255, 255, 255, 0});
  }
  updateImageEditor();
}

///
/// \brief Model::updateImageEditor - emits a signal to the view to update the
/// image editing window visuals
///
void Model::updateImageEditor() {
  QImage image = currentFrame->currentLayer->image;
  emit setImageEditor(QPixmap::fromImage(image.scaled(480, 480)));
  emit updateLayers(currentFrame->layers, currentFrame->currentLayerNum);
  emit setPreviewImage(QPixmap::fromImage(
      (frames[currentPreviewFrame]->getComposite()).scaled(480, 480)));
}

///
/// \brief Model::mousePressed - this method captures mouse clicks and
/// determines which model method needs to handle the click.
/// \param event
///
void Model::mousePressed(QMouseEvent *event) {
  QPointF mousePosition = event->position();
  // while mouse clicks from editor window section
  if (410 < mousePosition.rx() && mousePosition.rx() < 890 &&
      30 < mousePosition.ry() && mousePosition.ry() < 510) {
    draw = true;
    editFramePixels(event);
    return;
  }
  // while mouse clis from layer menu section
  if (1020 < mousePosition.rx() && mousePosition.rx() < 1270 &&
      370 < mousePosition.ry() && mousePosition.ry() < 620) {
    emit changeSelectedLayer(event);
  }
}

///
/// \brief Model::setLayerSelect helper method used to set the layer
/// \param index of the layer
///
void Model::setLayerSelect(int index) {
  currentFrame->currentLayer = &(currentFrame->layers[index]);
  currentFrame->currentLayerNum = index;
  updateImageEditor();
}

///
/// \brief Model::mouseMove - captures the location of the mouse as it moves in
/// the drawing window. Allows the user to click and draw to edit.
/// \param event
///
void Model::mouseMove(QMouseEvent *event) {
  QPointF mousePosition = event->position();
  if (draw) {
    // while mouse pressed and moved from editor window section
    if (410 < mousePosition.rx() && mousePosition.rx() < 890 &&
        30 < mousePosition.ry() && mousePosition.ry() < 510) {
      editFramePixels(event);
      return;
    }
  }
}

///
/// \brief Model::mouseReleased - sets draw to false so no more pixels will be
/// edited as the mouse moves.
/// \param event
///
void Model::mouseReleased(QMouseEvent *event) { draw = false; }

///
/// \brief Model::CustomColorButtonClicked opens the color dialog when the
/// button is clicked
///
void Model::CustomColorButtonClicked() { dialogBox.open(); }

///
/// \brief Model::colorSelected updates the color
/// \param color is the new color
///
void Model::colorSelected(const QColor &color) { currentColor = color; }

///
/// \brief Model::setOpacity - sets the opacity for the current color and tool
/// to the new value.
/// \param alpha
///
void Model::setOpacity(int alpha) { currentAlpha = alpha; }

// ***LAYERS***

///
/// \brief Adds a blank layer
///
void Model::addBlankLayer() {
  currentFrame->layers.append(Layer{imageSize});
  updateImageEditor();
}

///
/// \brief Deletes the currently selected layer
///
void Model::deleteSelectedLayer() {
  int pos = currentFrame->currentLayerNum;
  if (currentFrame->layers.count() != 1 && pos >= 0 &&
      pos < currentFrame->layers.count()) {
    currentFrame->layers.removeAt(pos);
    currentFrame->currentLayerNum = 0;
    emit setLayerSelect(0);
  }
  updateImageEditor();
}

///
/// \brief Moves the selected layer up if possible
///
void Model::moveSelectedLayerUp() {
  int pos = currentFrame->currentLayerNum;
  if (pos > 0 && pos < currentFrame->layers.count() - 1) {
    currentFrame->layers.move(pos, pos - 1);
  }
  updateImageEditor();
}

///
/// \brief Moves the currently selected layer down if possible
///
void Model::moveSelectedLayerDown() {
  int pos = currentFrame->currentLayerNum;
  if (pos >= 0 && pos < currentFrame->layers.count() - 1) {
    currentFrame->layers.move(pos, pos + 1);
  }
  updateImageEditor();
}

///
/// \brief A slot used to set the name of the currently selected layer
/// \param name The new name for the layer
///
void Model::RenameLayer(QString name) {
  currentFrame->currentLayer->name = name;
  emit updateLayerName(name);
}
///
/// \brief Model::updateVisibility
/// \param i: index of layer
/// \param state: true(visible) or false(invisible)
///
void Model::updateVisibility(int i, bool state) {

  if (i < currentFrame->layers.count()) {
    currentFrame->layers[i].visible = state;
    updateImageEditor();
  }
}

//***FRAMES***:

///
/// \brief a slot that insert a new frame in frames collection(Model)
///
void Model::addNewFrameClicked() {
  frames.push_back(new Frame{imageSize});
  // edge case: when there is no frame before adding the new frame
  if (frames.size() == 1) {
    // update current frame
    currentFrame = frames[0];
  }

  emit addANewFrameOnUi();
}
///
/// \brief a slot that removing the selected frame from frames collection(Model)
///
void Model::removeFrameClicked() {
  if (currentFrameNum - 1 == 0 && frames.size() == 1) {
    emit giveWarningMessage(); // Message to not delete frame warning box
    return;
  }
  frames.erase(frames.begin() + (currentFrameNum - 1));

  if ((ulong)currentFrameNum >
      frames.size()) { // if we remove the end one, we don't want to overflow.
    currentFrameNum = frames.size();
  }
  if (currentFrameNum - 1 > 0 &&
      (ulong)currentFrameNum - 1 <
          frames.size()) { // if we remove the frame within the bound,update
                           // current frame.
    currentFrame = frames[currentFrameNum - 1];
  }
  if (currentFrameNum - 1 == 0 &&
      frames.size() != 0) { // if we remove the first frame
    currentFrame = frames[currentFrameNum - 1];
  }

  emit removeFrameOnUi();
  emit setFrameHighlight(currentFrameNum);
  updateImageEditor();
}

void Model::copyFrameClicked() {
  copyFrame = new Frame(*currentFrame);
  frames.push_back(copyFrame);

  emit addANewFrameOnUi();
}

///
/// \brief Model::handleLeftScroll is a private helper for when when the left
/// button scrolls
///
void Model::handleLeftScroll() {
  if (currentFrameNum == 1) { // when current frame is the first frame, click
                              // the left scrooll should return the last frame
    currentFrameNum = frames.size();
  } else {
    currentFrameNum--;
  }
  currentFrame = frames[currentFrameNum - 1];
}

///
/// \brief Model::handleRightScroll is a private helper for when when the right
/// button scrolls
///
void Model::handleRightScroll() {
  if (currentFrameNum >=
      (int)frames.size()) { // when current frame is the last frame, click the
                            // right scrooll should return the first frame
    currentFrameNum = 1;
  } else {
    currentFrameNum++;
  }

  currentFrame = frames[currentFrameNum - 1];
}

///
/// \brief Model::leftScrollButtonClicked handles when the button to the left of
/// the frame view is clicked
///
void Model::leftScrollButtonClicked() {
  handleLeftScroll();
  emit setFrameHighlight(currentFrameNum);
  updateImageEditor();
}

///
/// \brief Model::rightScrollButtonClicked handles when the button to the right
/// of the frame view is clicked
///
void Model::rightScrollButtonClicked() {
  handleRightScroll();
  emit setFrameHighlight(currentFrameNum);
  updateImageEditor();
}

//***PREVIEW STUFF***:

///
/// \brief Model::receiveFrameRate updates the framerate internally
/// \param newFrameRate is the new framerate
///
void Model::receiveFrameRate(int newFrameRate) { frameRate = newFrameRate; }

///
/// \brief Recursive looping method to write the frames to the preview
///  Is called once in constructor to get started; afterwards it calls itself at
///  the proper timing.
void Model::displayNextPreviewFrame() {
  // Basically, recursively calls every 1000/(frameRate) ms.
  QTimer::singleShot(1000 / frameRate, this, &Model::displayNextPreviewFrame);

  if (currentPreviewFrame >= frames.size() - 1) {
    // Will need to "reset" loop
    currentPreviewFrame = 0;

  } else {
    currentPreviewFrame++;
  }

  emit setPreviewImage(QPixmap::fromImage(
      (frames[currentPreviewFrame]->getComposite()).scaled(480, 480)));
}

//***SAVING/LOADING PROJECT***:

///
/// \brief Model::write writes the project to a JSON file
/// \param json is the JSON object
///
void Model::write(QJsonObject &json) {
  // write height, width, number of frames variables to JSON
  json["height"] = height;
  json["width"] = width;
  numOfFrames = frames.size();
  json["numberOfFrames"] = numOfFrames;

  // write all the frames to JSON
  QJsonArray frameArray;

  for (Frame *frame : frames) {
    QJsonObject frameObject;
    frame->write(frameObject);
    frameArray.append(frameObject);
  }
  json["frames"] = frameArray;
}

///
/// \brief Model::read reads the JSON from a saved project
/// \param json the JSON object being read
///
void Model::read(QJsonObject &json) {
  // Verify height and width equal
  if (json.contains("height") && json["height"].isDouble()) {
    height = json["height"].toInt();
  }
  if (json.contains("width") && json["width"].isDouble()) {
    width = json["width"].toInt();
  }
  if (height != width) {
    qWarning("File does not have square format (height != width)");
  }

  imageSize = height;
  // Check number of frames
  if (json.contains("numberOfFrames") && json["numberOfFrames"].isDouble()) {
    numOfFrames = json["numberOfFrames"].toInt();
  }

  // read all the frames
  if (json.contains("frames") && json["frames"].isArray()) {
    QJsonArray frameArray = json["frames"].toArray();
    frames.clear();
    for (QJsonValue v : frameArray) {
      QJsonObject frameObject = v.toObject();
      currentFrame = new Frame(imageSize);
      currentFrame->read(frameObject);
      frames.push_back(currentFrame);
    }
  }
}

///
/// \brief Model::setSize sets the size internally and updates the appropriate
/// elements
/// \param size of the new project
///
void Model::setSize(int size) {
  // update sizes where needed
  imageSize = size;
  height = imageSize;
  width = imageSize;
  // clear any old frames
  frames.clear();
  currentFrame = new Frame(imageSize);
  // deafult the color
  currentColor = QColor{255, 255, 255, 0};
  currentFrameNum = 1;
  frames.push_back(currentFrame);
  currentPreviewFrame = 0;
  copyFrame = nullptr;
  addFrameIndex = 1;
  // update the ui
  displayNextPreviewFrame();
  emit setFrameHighlight(1);
  updateImageEditor();
}

///
/// \brief Saves the project to the directory
/// \param fileName to save the file to
///
void Model::saveProject(QString fileName) {
  // create and open the save file
  QFile saveFile(fileName);
  if (!saveFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't open save file.");
  }
  QJsonObject gameObject;
  // write the project to JSON
  write(gameObject);
  // save the project to the file
  saveFile.write(QJsonDocument(gameObject).toJson());
}

///
/// \brief Model::loadProject loads a project of the given fileName
/// \param fileName
///
void Model::loadProject(QString fileName) {
  QFile loadFile(fileName);
  if (!loadFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open save file.");
  }
  // create the JSON object to be read
  QByteArray saveData = loadFile.readAll();
  QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
  QJsonObject obj = loadDoc.object();

  read(obj);
  // update the ui based on the loaded project
  updateImageEditor();
}
//***EXPORTING***:
///
/// \brief Model::saveGIF saves all of the frames into a gif using the framerate
/// in the viewer
/// \param fileName The file to save to
///
void Model::saveGIF(QString fileName) {
  // Store the Images temporarily
  QVector<QImage> tempImgs;
  for (Frame *f : frames) {
    tempImgs.append(f->getComposite());
  }

  // Save them to a temp folder on disk
  QDir tempDir;
  tempDir.mkdir(QString("temp"));
  QStringList fileNames;
  for (int i = 0; i < tempImgs.size(); i++) {
    QString fileName = QString("%1/%2.png").arg("temp").arg(i);
    tempImgs[i].save(fileName);
    fileNames.append(fileName);
  }

  // Stitch the GIF together
  int delay = 100 / frameRate; // Converting frames per second into centiseconds
  GifWriter writer;
  GifBegin(&writer, qPrintable(fileName), tempImgs[0].width(),
           tempImgs[0].height(), delay);

  for (int i = 0; i < tempImgs.size(); i++) {
    QImage currImg(fileNames[i]);
    GifWriteFrame(&writer, currImg.bits(), currImg.width(), currImg.height(),
                  delay);
  }

  GifEnd(&writer);

  // Clean up the temp folder
  for (int i = 0; i < fileNames.size(); i++) {
    QFile::remove(fileNames[i]);
  }
  tempDir.rmdir("temp");
}

///
/// \brief Model::savePNG saves the selected frame to a file
/// \param fileName The Filename to save to
///
void Model::savePNG(QString fileName) {
  currentFrame->getComposite().save(fileName + ".png");
}

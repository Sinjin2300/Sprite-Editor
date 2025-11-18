#include "Popup.h"

///
/// \brief Popup::Popup constructor for the popup
/// \param model
/// \param parent
///
Popup::Popup(Model &model, QWidget *parent) : QWidget(parent) {
  m = &model;

  // size and location
  resize(200, 200);
  move(500, 200);
  setWindowIcon(QIcon::fromTheme("info"));

  // button setup
  auto verticalLayout = new QVBoxLayout(this);
  auto textDisplay = new QTextEdit("Select frame size:");
  textDisplay->setReadOnly(true);
  auto createEightPixelSizeButton = new QPushButton("8x8");
  auto createSixteenPixelSizeButton = new QPushButton("16x16");
  auto createThirtyPixelSizeButton = new QPushButton("32x32");
  auto createSixtySizePixelSizeButton = new QPushButton("64x64");
  verticalLayout->addWidget(textDisplay);
  verticalLayout->addWidget(createEightPixelSizeButton);
  verticalLayout->addWidget(createSixteenPixelSizeButton);
  verticalLayout->addWidget(createThirtyPixelSizeButton);
  verticalLayout->addWidget(createSixtySizePixelSizeButton);

  // connections
  connect(createEightPixelSizeButton, &QPushButton::clicked, this,
          &Popup::onEightButtonClicked);
  connect(createSixteenPixelSizeButton, &QPushButton::clicked, this,
          &Popup::onSixteenButtonClicked);
  connect(createThirtyPixelSizeButton, &QPushButton::clicked, this,
          &Popup::onThirtyTwoButtonClicked);
  connect(createSixtySizePixelSizeButton, &QPushButton::clicked, this,
          &Popup::onSixtyFourButtonClicked);
}

Popup::~Popup() {}

///
/// \brief Popup::onEightButtonClicked sets frame size to 8
///
void Popup::onEightButtonClicked() {
  m->setSize(8);
  this->hide();
}

///
/// \brief Popup::onSixteenButtonClicked sets frame size to 16
///
void Popup::onSixteenButtonClicked() {
  m->setSize(16);
  this->hide();
}

///
/// \brief Popup::onThirtyTwoButtonClicked sets frame size to 32
///
void Popup::onThirtyTwoButtonClicked() {
  m->setSize(32);
  this->hide();
}

///
/// \brief Popup::onSixtyFourButtonClicked sets frame size to 64
///
void Popup::onSixtyFourButtonClicked() {
  m->setSize(64);
  this->hide();
}

#include "src/ui/rom-settings-widget.hpp"
#include "ui_rom-settings-widget.h"

#include "src/core/file-controller.hpp"
#include "src/core/hotkeys.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/input/keyboard.hpp"
#include "src/ui/flow-layout.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/manage-sd-cards-dialog.hpp"
#include <QVBoxLayout>

RomSettingsWidget::RomSettingsWidget(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::RomSettingsWidget), m_suppressEvents(true),
      m_showAllPlugins(FileController::loadUiState().showAllPlugins),
      m_accurateDepthCompare(true), m_suggestOverclock(true),
      m_supportsMouse(false) {
  m_ui->setupUi(this);
  refreshSdCards();

#ifdef __APPLE__
  if (FileController::loadAppSettings().theme == "macintosh") {
    m_ui->emulatorTabs->setStyleSheet(QString());
  }
#endif

  m_ui->manageCardsButton->setIcon(Icon::configure());

  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginParallelRadio,
                                        (int)GfxPlugin::ParaLLEl);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginGlideRadio,
                                        (int)GfxPlugin::Glide64);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginGLideN64Radio,
                                        (int)GfxPlugin::GLideN64);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginOgreRadio,
                                        (int)GfxPlugin::OGRE);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginAngrylionRadio,
                                        (int)GfxPlugin::Angrylion);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginRiceRadio,
                                        (int)GfxPlugin::Rice);
  m_ui->parallelPluginRadioGroup->setId(m_ui->pluginGlnRadio,
                                        (int)GfxPlugin::Gln64);

  m_ui->mupenPluginRadioGroup->setId(m_ui->pluginParallelRadio2,
                                     (int)GfxPlugin::ParaLLEl);
  m_ui->mupenPluginRadioGroup->setId(m_ui->pluginAngrylionRadio2,
                                     (int)GfxPlugin::Angrylion);
  m_ui->mupenPluginRadioGroup->setId(m_ui->pluginGlidenRadio2,
                                     (int)GfxPlugin::GLideN64);

  connect(m_ui->parallelPluginRadioGroup, SIGNAL(buttonToggled(int, bool)),
          this, SLOT(parallelPluginRadioChanged(int, bool)));
  connect(m_ui->mupenPluginRadioGroup, SIGNAL(buttonToggled(int, bool)), this,
          SLOT(mupenPluginRadioChanged(int, bool)));

  FlowLayout *checkboxLayout =
      new FlowLayout(m_ui->checkboxContainer, QSize(16, 6));
  m_ui->checkboxContainer->setLayout(checkboxLayout);
  checkboxLayout->addWidget(m_ui->overclockCpuCheckbox);
  checkboxLayout->addWidget(m_ui->overclockViCheckbox);
  checkboxLayout->addWidget(m_ui->bigEepromCheckbox);
  checkboxLayout->addWidget(m_ui->useInterpreterCheckbox);
  checkboxLayout->addWidget(m_ui->widescreenCheckbox);
  checkboxLayout->addWidget(m_ui->trueWidescreenCheckbox);
  checkboxLayout->addWidget(m_ui->removeBordersCheckbox);
  checkboxLayout->addWidget(m_ui->upscaleTexRectsCheckbox);
  checkboxLayout->addWidget(m_ui->emulateFramebufferCheckbox);
  checkboxLayout->addWidget(m_ui->accurateDepthCompareCheckbox);
  checkboxLayout->addWidget(m_ui->rspEmulationCheckbox);
  checkboxLayout->setContentsMargins(6, 0, 6, 0);

#ifdef __APPLE__
  QMargins margins = checkboxLayout->contentsMargins();
  margins.setBottom(margins.bottom() + 4);
  checkboxLayout->setContentsMargins(margins);
#endif

  const AppSettings &settings = FileController::loadAppSettings();
  setSettings(
      DefaultInputModes::Normal.id, settings.defaultEmulator,
      settings.defaultParallelPlugin, settings.defaultMupenPlugin, true, false,
      WidescreenMode::Off, false, false, settings.parallelTexRectUpscaling,
      settings.parallelRemoveBorders, settings.glidenFramebufferEmulation,
      settings.glidenCorrectDepthCompare, false, GfxPlugin::UseDefault, true,
      {N64InputType::Unknown, N64InputType::Unknown, N64InputType::Unknown,
       N64InputType::Unknown},
      std::nullopt, "");

  fixTabWidgetSize();
  updateWarningVisibility();
  updatePluginVisibility();
  updatePluginSettingsVisibility();
  m_suppressEvents = false;
}

RomSettingsWidget::~RomSettingsWidget() { delete m_ui; }

static inline bool isSpecialInputType(N64InputType inputType) {
  return (inputType == N64InputType::Gamecube ||
          inputType == N64InputType::GamecubeKeyboard ||
          inputType == N64InputType::Mouse);
}

static inline bool hasSpecialInputType(const N64InputTypes &inputTypes) {
  return (
      isSpecialInputType(inputTypes[0]) || isSpecialInputType(inputTypes[1]) ||
      isSpecialInputType(inputTypes[2]) || isSpecialInputType(inputTypes[3]));
}

void RomSettingsWidget::setSettings(
    const Uuid &inputMode, EmulatorCore core, GfxPlugin parallelPlugin,
    GfxPlugin mupenPlugin, bool overclockCpu, bool overclockVi,
    WidescreenMode widescreen, bool bigEEPROM, bool useInterpreter,
    bool upscaleTexRects, bool removeBorders, bool emulateFramebuffer,
    bool accurateDepthCompare, bool rspEmulation, GfxPlugin recommendedPlugin,
    bool suggestOverclock, const N64InputTypes &inputTypes,
    const std::optional<bool> &syncSaves, const std::string &sdCard) {
  m_accurateDepthCompare = accurateDepthCompare;
  m_suggestOverclock = suggestOverclock;

  if (core == EmulatorCore::UseDefault) {
    core = FileController::loadAppSettings().defaultEmulator;
  }

  if (m_ui->parallelPluginRadioGroup->button((int)parallelPlugin) == nullptr) {
    parallelPlugin = GfxPlugin::UseDefault;
  }

  if (m_ui->mupenPluginRadioGroup->button((int)mupenPlugin) == nullptr) {
    mupenPlugin = GfxPlugin::UseDefault;
  }

  if (parallelPlugin == GfxPlugin::UseDefault) {
    parallelPlugin = FileController::loadAppSettings().defaultParallelPlugin;
  }

  if (mupenPlugin == GfxPlugin::UseDefault) {
    mupenPlugin = FileController::loadAppSettings().defaultMupenPlugin;
  }

  m_ui->trueWidescreenCheckbox->setVisible(
      (core == EmulatorCore::Mupen64plusNext &&
       mupenPlugin == GfxPlugin::GLideN64) ||
      (core == EmulatorCore::ParallelN64 &&
       parallelPlugin == GfxPlugin::GLideN64) ||
      (core == EmulatorCore::ParallelN64 && parallelPlugin == GfxPlugin::OGRE));

  m_ui->overclockCpuCheckbox->setText(suggestOverclock
                                          ? tr("Overclock CPU (Recommended)")
                                          : tr("Overclock CPU"));

  if (syncSaves.has_value()) {
    m_ui->syncSavesCheckbox->setVisible(true);
    m_ui->syncSavesCheckbox->setChecked(syncSaves.value());
  } else {
    m_ui->syncSavesCheckbox->setVisible(false);
  }

  const bool showAdvanced =
      FileController::loadAppSettings().showAdvancedSettings;
  m_ui->overclockViCheckbox->setVisible(showAdvanced);
  m_ui->bigEepromCheckbox->setVisible(showAdvanced ||
                                      (core == EmulatorCore::Mupen64plusNext));
  m_ui->useInterpreterCheckbox->setVisible(showAdvanced);

  m_suppressEvents = true;
  if (hasSpecialInputType(inputTypes)) {
    m_ui->inputModeSelect->setEnabled(false);
    m_ui->inputModeSelect->setSelected(DefaultInputModes::Normal.id);
  } else {
    m_ui->inputModeSelect->setEnabled(true);
    m_ui->inputModeSelect->setSelected(inputMode);
  }
  m_ui->emulatorTabs->setCurrentIndex((core == EmulatorCore::ParallelN64) ? 0
                                                                          : 1);
  m_ui->parallelPluginRadioGroup->button((int)parallelPlugin)->setChecked(true);
  m_ui->mupenPluginRadioGroup->button((int)mupenPlugin)->setChecked(true);
  m_ui->overclockCpuCheckbox->setChecked(overclockCpu);
  m_ui->overclockViCheckbox->setChecked(overclockVi);
  m_ui->widescreenCheckbox->setChecked(widescreen == WidescreenMode::Stretched);
  m_ui->bigEepromCheckbox->setChecked(bigEEPROM);
  m_ui->useInterpreterCheckbox->setChecked(useInterpreter);
  m_ui->trueWidescreenCheckbox->setChecked(widescreen ==
                                           WidescreenMode::ViewportHack);
  m_ui->upscaleTexRectsCheckbox->setChecked(upscaleTexRects);
  m_ui->removeBordersCheckbox->setChecked(removeBorders);
  m_ui->emulateFramebufferCheckbox->setChecked(emulateFramebuffer);
  m_ui->accurateDepthCompareCheckbox->setChecked(emulateFramebuffer &&
                                                 accurateDepthCompare);
  m_ui->accurateDepthCompareCheckbox->setEnabled(emulateFramebuffer);
  m_ui->rspEmulationCheckbox->setChecked(rspEmulation);

  if (FileController::loadAppSettings().emulateSdCard || !sdCard.empty()) {
    m_ui->sdCardPanel->setVisible(true);

    m_ui->sdCardSelect->setCurrentIndex(0);
    for (int i = 0; i < m_ui->sdCardSelect->count(); i++) {
      if (sdCard == m_ui->sdCardSelect->itemData(i, Qt::UserRole)
                        .toString()
                        .toStdString()) {
        m_ui->sdCardSelect->setCurrentIndex(i);
        break;
      }
    }
  } else {
    m_ui->sdCardPanel->setVisible(false);
    m_ui->sdCardSelect->setCurrentIndex(0);
  }

  m_suppressEvents = false;

  m_supportsMouse = (inputTypes[0] == N64InputType::Mouse ||
                     inputTypes[1] == N64InputType::Mouse ||
                     inputTypes[2] == N64InputType::Mouse ||
                     inputTypes[3] == N64InputType::Mouse);

  updateWarningVisibility();
  updatePluginSettingsVisibility();
  updatePluginLangterms(recommendedPlugin);
  reloadHotkeyInfo();
}

const Uuid &RomSettingsWidget::getInputMode() const {
  return m_ui->inputModeSelect->getSelected().id;
}

EmulatorCore RomSettingsWidget::getEmulatorCore() const {
  return (EmulatorCore)(m_ui->emulatorTabs->currentIndex() + 1);
}

GfxPlugin RomSettingsWidget::getParallelPlugin() const {
  return (GfxPlugin)m_ui->parallelPluginRadioGroup->checkedId();
}

GfxPlugin RomSettingsWidget::getMupenPlugin() const {
  return (GfxPlugin)m_ui->mupenPluginRadioGroup->checkedId();
}

bool RomSettingsWidget::overclockCpu() const {
  return m_ui->overclockCpuCheckbox->isChecked();
}

bool RomSettingsWidget::overclockVi() const {
  return m_ui->overclockViCheckbox->isChecked();
}

WidescreenMode RomSettingsWidget::widescreen() const {
  if (m_ui->trueWidescreenCheckbox->isChecked()) {
    return WidescreenMode::ViewportHack;
  } else if (m_ui->widescreenCheckbox->isChecked()) {
    return WidescreenMode::Stretched;
  } else {
    return WidescreenMode::Off;
  }
}

bool RomSettingsWidget::bigEEPROM() const {
  return m_ui->bigEepromCheckbox->isChecked();
}

bool RomSettingsWidget::useInterpreter() const {
  return m_ui->useInterpreterCheckbox->isChecked();
}

bool RomSettingsWidget::upscaleTexRects() const {
  return m_ui->upscaleTexRectsCheckbox->isChecked();
}

bool RomSettingsWidget::removeBorders() const {
  return m_ui->removeBordersCheckbox->isChecked();
}

bool RomSettingsWidget::emulateFramebuffer() const {
  return m_ui->emulateFramebufferCheckbox->isChecked();
}

bool RomSettingsWidget::glidenRspEmulation() const {
  return m_ui->rspEmulationCheckbox->isChecked();
}

std::string RomSettingsWidget::sdCard() const {
  const int index = m_ui->sdCardSelect->currentIndex();
  if (index <= 0 || index > (int)m_sdCards.size())
    return "";
  return m_sdCards[index - 1].name;
}

void RomSettingsWidget::updateWarningVisibility() {
  m_ui->performanceWarningLabel->setVisible(
      m_suggestOverclock && !m_ui->overclockCpuCheckbox->isChecked());
#ifdef __APPLE__
  m_ui->macDepthWarning->setVisible(
      m_ui->accurateDepthCompareCheckbox->isChecked() &&
      m_ui->pluginGLideN64Radio->isChecked() &&
      m_ui->emulatorTabs->currentIndex() == 0);
#else
  m_ui->macDepthWarning->setVisible(false);
#endif

  reloadHotkeyInfo();
}

void RomSettingsWidget::updatePluginVisibility() {
  m_ui->pluginGlideRadio->setVisible(m_showAllPlugins);
  m_ui->pluginAngrylionRadio->setVisible(m_showAllPlugins);
  m_ui->pluginGlnRadio->setVisible(m_showAllPlugins);
  m_ui->pluginRiceRadio->setVisible(m_showAllPlugins);

  m_ui->showMorePluginsLinks->setText(
      QString("<a href=\"#\">") +
      (m_showAllPlugins ? tr("Show Fewer Plugins") : tr("Show More Plugins")) +
      QString("</a>"));
}

void RomSettingsWidget::reloadHotkeyInfo() {
  if (m_supportsMouse) {
    const KeyId hotkey =
        FileController::loadHotkeys().at((int)Hotkey::GrabMouse);
    const QString keyName = Keycode::getNames(hotkey).displayName;
    m_ui->mouseGrabHotkeyWarning->setVisible(hotkey == KeyId::INVALID);
    m_ui->mouseHintLabel->setVisible(hotkey != KeyId::INVALID);
    m_ui->mouseHintLabel->setText(
        tr("This ROM supports mouse input. After launching the emulator, your "
           "mouse will be captured by the emulator. Press %1 if you need to "
           "free the mouse cursor.")
            .arg(keyName));
  } else {
    m_ui->mouseGrabHotkeyWarning->setVisible(false);
    m_ui->mouseHintLabel->setVisible(false);
  }
}

void RomSettingsWidget::updatePluginSettingsVisibility() {
  m_ui->upscaleTexRectsCheckbox->setVisible(
      (m_ui->emulatorTabs->currentIndex() == 0 &&
       m_ui->pluginParallelRadio->isChecked()) ||
      (m_ui->emulatorTabs->currentIndex() == 1 &&
       m_ui->pluginParallelRadio2->isChecked()));

  m_ui->removeBordersCheckbox->setVisible(
      m_ui->emulatorTabs->currentIndex() == 0 &&
      m_ui->pluginParallelRadio->isChecked());

  const bool gliden64 = ((m_ui->emulatorTabs->currentIndex() == 1 &&
                          m_ui->pluginGlidenRadio2->isChecked()) ||
                         (m_ui->emulatorTabs->currentIndex() == 0 &&
                          m_ui->pluginGLideN64Radio->isChecked()));

  const bool ogre = m_ui->emulatorTabs->currentIndex() == 0 &&
                    m_ui->pluginOgreRadio->isChecked();

  m_ui->emulateFramebufferCheckbox->setVisible(gliden64);
  m_ui->accurateDepthCompareCheckbox->setVisible(gliden64);
  m_ui->rspEmulationCheckbox->setVisible(
      gliden64 && m_ui->emulatorTabs->currentIndex() == 0);

  m_ui->trueWidescreenCheckbox->setVisible(gliden64 || ogre);
}

void RomSettingsWidget::inputModeSelectionChanged() {
  if (m_suppressEvents)
    return;
  emit inputModeChanged();
}

void RomSettingsWidget::overclockCpuToggled(bool checked) {
  m_ui->performanceWarningLabel->setVisible(!checked);
  if (m_suppressEvents)
    return;
  emit overclockCpuChanged(checked);
}

void RomSettingsWidget::overclockViToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit overclockViChanged(checked);
}

void RomSettingsWidget::widescreenToggled(bool checked) {
  if (m_suppressEvents)
    return;
  if (checked) {
    m_suppressEvents = true;
    m_ui->trueWidescreenCheckbox->setChecked(false);
    m_suppressEvents = false;
  }
  emit widescreenChanged(checked ? (ubyte)WidescreenMode::Stretched
                                 : (ubyte)WidescreenMode::Off);
}

void RomSettingsWidget::bigEepromToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit bigEepromChanged(checked);
}

void RomSettingsWidget::useInterpreterToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit useInterpreterChanged(checked);
}

void RomSettingsWidget::trueWidescreenToggled(bool checked) {
  if (m_suppressEvents)
    return;
  if (checked) {
    m_suppressEvents = true;
    m_ui->widescreenCheckbox->setChecked(false);
    m_suppressEvents = false;
  }
  emit widescreenChanged(checked ? (ubyte)WidescreenMode::ViewportHack
                                 : (ubyte)WidescreenMode::Off);
}

void RomSettingsWidget::upscaleTexRectsToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit upscaleTexRectsChanged(checked);
}

void RomSettingsWidget::removeBordersToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit removeBordersChanged(checked);
}

void RomSettingsWidget::emulateFramebufferToggled(bool checked) {
  m_ui->accurateDepthCompareCheckbox->setEnabled(checked);
  if (m_suppressEvents) {
    m_ui->accurateDepthCompareCheckbox->setChecked(checked &&
                                                   m_accurateDepthCompare);
  } else {
    m_suppressEvents = true;
    m_ui->accurateDepthCompareCheckbox->setChecked(checked &&
                                                   m_accurateDepthCompare);
    m_suppressEvents = false;
    emit emulateFramebufferChanged(checked);
  }
}

void RomSettingsWidget::accurateDepthCompareToggled(bool checked) {
  m_accurateDepthCompare = checked;
  if (m_suppressEvents)
    return;
  updateWarningVisibility();
  emit accurateDepthCompareChanged(checked);
}

void RomSettingsWidget::rspEmulationToggled(bool checked) {
  if (m_suppressEvents)
    return;
  emit rspEmulationChanged(checked);
}

void RomSettingsWidget::emulatorChanged() {
  fixTabWidgetSize();
  updateWarningVisibility();
  updatePluginSettingsVisibility();
  emit emulatorCoreChanged();
}

void RomSettingsWidget::parallelPluginRadioChanged(int, bool checked) {
  if (!checked)
    return;
  updateWarningVisibility();
  updatePluginSettingsVisibility();
  emit parallelPluginChanged();
}

void RomSettingsWidget::mupenPluginRadioChanged(int, bool checked) {
  if (!checked)
    return;
  updatePluginSettingsVisibility();
  emit mupenPluginChanged();
}

void RomSettingsWidget::saveSyncingToggled(bool checked) {
  if (m_suppressEvents || !m_ui->syncSavesCheckbox->isVisible())
    return;
  emit saveSyncingChanged(checked);
}

void RomSettingsWidget::morePluginsToggled() {
  m_showAllPlugins = !m_showAllPlugins;
  updatePluginVisibility();
  if (m_suppressEvents)
    return;
  emit showMorePluginsChanged(m_showAllPlugins);
}

void RomSettingsWidget::sdCardChanged(int index) {
  if (m_suppressEvents || !m_ui->sdCardPanel->isVisible())
    return;
  if (index < 0 || index > (int)m_sdCards.size())
    return;

  if (index == 0) {
    emit activeSdCardChanged(std::string());
  } else {
    emit activeSdCardChanged(m_sdCards[index - 1].name);
  }
}

void RomSettingsWidget::manageSdCards() {
  ManageSdCardsDialog dialog(this);
  dialog.exec();

  const std::string selectedCard = sdCard();
  refreshSdCards();

  if (selectedCard.empty() || selectedCard == "?") {
    m_ui->sdCardSelect->setCurrentIndex(0);
    return;
  }

  for (int i = 0; i < m_ui->sdCardSelect->count(); i++) {
    if (selectedCard == m_ui->sdCardSelect->itemData(i, Qt::UserRole)
                            .toString()
                            .toStdString()) {
      m_ui->sdCardSelect->setCurrentIndex(i);
      return;
    }
  }

  m_ui->sdCardSelect->setCurrentIndex(0);
  emit activeSdCardChanged(std::string());
}

void RomSettingsWidget::updatePluginLangterms(GfxPlugin recommendedPlugin) {
  const QString recommendedText =
      QString(" ") + tr("(recommended by hack author)");
  switch (recommendedPlugin) {
  case GfxPlugin::ParaLLEl:
    m_ui->pluginParallelRadio->setText(QString("ParaLLEl") + recommendedText);
    m_ui->pluginGLideN64Radio->setText("GLideN64");
    m_ui->pluginOgreRadio->setText("OGRE");
    break;
  case GfxPlugin::GLideN64:
    m_ui->pluginParallelRadio->setText("ParaLLEl");
    m_ui->pluginGLideN64Radio->setText(QString("GLideN64") + recommendedText);
    m_ui->pluginOgreRadio->setText("OGRE");
    break;
  case GfxPlugin::OGRE:
    m_ui->pluginParallelRadio->setText("ParaLLEl");
    m_ui->pluginGLideN64Radio->setText("GLideN64");
    m_ui->pluginOgreRadio->setText(QString("OGRE") + recommendedText);
    break;
  case GfxPlugin::UseDefault:
    m_ui->pluginParallelRadio->setText(
        tr("ParaLLEl (Recommended - very accurate to console)"));
    m_ui->pluginGLideN64Radio->setText(
        tr("GLideN64 (Recommended for lower end computers)"));
    m_ui->pluginOgreRadio->setText(tr("OGRE (Needed by some older romhacks)"));
    break;
  default:
    m_ui->pluginParallelRadio->setText("ParaLLEl");
    m_ui->pluginGLideN64Radio->setText("GLideN64");
    m_ui->pluginOgreRadio->setText("OGRE");
    break;
  }

  const QString angrylionText =
      QString("Angrylion") + ((recommendedPlugin == GfxPlugin::Angrylion)
                                  ? recommendedText
                                  : QString());
  m_ui->pluginAngrylionRadio->setText(angrylionText);
  m_ui->pluginAngrylionRadio2->setText(angrylionText);

  m_ui->pluginRiceRadio->setText(
      QString("Rice") +
      ((recommendedPlugin == GfxPlugin::Rice) ? recommendedText : QString()));
  m_ui->pluginGlidenRadio2->setText(QString("GLideN64") +
                                    ((recommendedPlugin == GfxPlugin::GLideN64)
                                         ? recommendedText
                                         : QString()));
  m_ui->pluginParallelRadio2->setText(
      QString("ParaLLEl") + ((recommendedPlugin == GfxPlugin::ParaLLEl)
                                 ? recommendedText
                                 : QString()));
}

void RomSettingsWidget::fixTabWidgetSize() {
  m_ui->pluginGroup->setSizePolicy(QSizePolicy::Preferred,
                                   m_ui->emulatorTabs->currentIndex() == 0
                                       ? QSizePolicy::Preferred
                                       : QSizePolicy::Ignored);
  m_ui->pluginGroup->resize(m_ui->pluginGroup->minimumSizeHint());
  m_ui->pluginGroup->adjustSize();

  m_ui->mupenPluginGroup->setSizePolicy(QSizePolicy::Preferred,
                                        m_ui->emulatorTabs->currentIndex() != 0
                                            ? QSizePolicy::Preferred
                                            : QSizePolicy::Ignored);
  m_ui->mupenPluginGroup->resize(m_ui->mupenPluginGroup->minimumSizeHint());
  m_ui->mupenPluginGroup->adjustSize();

  m_ui->emulatorTabs->resize(m_ui->emulatorTabs->minimumSizeHint());
  m_ui->emulatorTabs->adjustSize();

  resize(minimumSizeHint());
  adjustSize();
}

static inline QString makeSdCardText(const SdCardInfo &card) {
  QString text(card.name.c_str());
  text.append(" (");

  switch (card.format) {
  case SdCardFormat::FAT12:
    text.append("FAT12, ");
    break;
  case SdCardFormat::FAT16:
    text.append("FAT16, ");
    break;
  case SdCardFormat::FAT32:
    text.append("FAT32, ");
    break;
  default:
    break;
  }

  if (card.numBlocks >= 1073741824u) {
    text.append(QString::number((double)card.numBlocks / 1073741824.0, 'f', 1));
    text.append(" TiB)");
  } else if (card.numBlocks >= 1048576u) {
    text.append(QString::number((double)card.numBlocks / 1048576.0, 'f', 1));
    text.append(" GiB)");
  } else if (card.numBlocks >= 1024u) {
    text.append(QString::number(card.numBlocks / 1024.0));
    text.append(" MiB)");
  } else {
    text.append(QString::number(card.numBlocks));
    text.append(" KiB)");
  }

  return text;
}

void RomSettingsWidget::refreshSdCards() {
  m_sdCards = SdCardManager::fetchAll();

  m_ui->sdCardSelect->clear();
  m_ui->sdCardSelect->addItem(tr("None"), QVariant(QString()));
  for (const SdCardInfo &card : m_sdCards) {
    m_ui->sdCardSelect->addItem(makeSdCardText(card),
                                QVariant(QString(card.name.c_str())));
  }
}

RomSettingsWidgetSlot::RomSettingsWidgetSlot(QWidget *parent) : QFrame(parent) {
  setContentsMargins(0, 0, 0, 0);
  setFrameShape(QFrame::NoFrame);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setMargin(0);

  setLayout(layout);
}

void RomSettingsWidgetSlot::place(RomSettingsWidget *widget) {
  if (hasWidget())
    return;

  if (widget->parentWidget() != nullptr) {
    widget->parentWidget()->layout()->removeWidget(widget);
  }

  widget->setParent(this);
  layout()->addWidget(widget);
}

RomSettingsWidget *RomSettingsWidgetSlot::widget() const {
  if (layout()->children().isEmpty())
    return nullptr;
  return layout()->children().isEmpty()
             ? nullptr
             : dynamic_cast<RomSettingsWidget *>(layout()->children().first());
}

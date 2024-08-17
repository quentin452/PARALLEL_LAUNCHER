#ifndef SRC_UI_ROM_SETTINGS_WIDGET_HPP_
#define SRC_UI_ROM_SETTINGS_WIDGET_HPP_

#include "src/core/rom.hpp"
#include "src/core/sdcard.hpp"
#include "src/core/settings.hpp"
#include <QFrame>
#include <optional>

namespace Ui {
class RomSettingsWidget;
}

class RomSettingsWidget : public QWidget {
  Q_OBJECT

private:
  Ui::RomSettingsWidget *m_ui;
  bool m_suppressEvents;
  bool m_showAllPlugins;
  bool m_accurateDepthCompare;
  bool m_suggestOverclock;
  bool m_supportsMouse;
  std::vector<SdCardInfo> m_sdCards;

public:
  explicit RomSettingsWidget(QWidget *parent = nullptr);
  virtual ~RomSettingsWidget();

  void reloadHotkeyInfo();
  void setSettings(
      const Uuid &inputMode, EmulatorCore core, GfxPlugin parallelPlugin,
      GfxPlugin mupenPlugin, bool overclockCpu, bool overclockVi,
      WidescreenMode widescreen, bool bigEEPROM, bool useInterpreter,
      bool upscaleTexRects, bool removeBorders, bool emulateFramebuffer,
      bool accurateDepthCompare, bool rspEmulation, GfxPlugin recommendedPlugin,
      bool suggestOverclock, const N64InputTypes &inputTypes,
      const std::optional<bool> &syncSaves, const std::string &sdCard);

  const Uuid &getInputMode() const;
  EmulatorCore getEmulatorCore() const;
  GfxPlugin getParallelPlugin() const;
  GfxPlugin getMupenPlugin() const;
  bool overclockCpu() const;
  bool overclockVi() const;
  WidescreenMode widescreen() const;
  bool bigEEPROM() const;
  bool useInterpreter() const;
  bool upscaleTexRects() const;
  bool removeBorders() const;
  bool emulateFramebuffer() const;
  inline bool accurateDepthCompare() const noexcept {
    return m_accurateDepthCompare;
  }
  bool glidenRspEmulation() const;
  std::string sdCard() const;

private:
  void updateWarningVisibility();
  void updatePluginVisibility();
  void updatePluginSettingsVisibility();
  void updatePluginLangterms(GfxPlugin recommendedPlugin);
  void fixTabWidgetSize();

public:
  void refreshSdCards();

  inline void setShowAllPlugins(bool showAll) {
    m_showAllPlugins = showAll;
    updatePluginVisibility();
  }

  inline bool showingAllPlugins() const noexcept { return m_showAllPlugins; }

private slots:
  void inputModeSelectionChanged();
  void overclockCpuToggled(bool checked);
  void overclockViToggled(bool checked);
  void widescreenToggled(bool checked);
  void bigEepromToggled(bool checked);
  void useInterpreterToggled(bool checked);
  void trueWidescreenToggled(bool checked);
  void upscaleTexRectsToggled(bool checked);
  void removeBordersToggled(bool checked);
  void emulateFramebufferToggled(bool checked);
  void accurateDepthCompareToggled(bool checked);
  void rspEmulationToggled(bool checked);
  void saveSyncingToggled(bool checked);
  void emulatorChanged();
  void parallelPluginRadioChanged(int pluginId, bool checked);
  void mupenPluginRadioChanged(int pluginId, bool checked);
  void morePluginsToggled();
  void sdCardChanged(int index);
  void manageSdCards();

signals:
  void inputModeChanged();
  void emulatorCoreChanged();
  void parallelPluginChanged();
  void mupenPluginChanged();
  void overclockCpuChanged(bool);
  void overclockViChanged(bool);
  void widescreenChanged(unsigned char);
  void bigEepromChanged(bool);
  void useInterpreterChanged(bool);
  void upscaleTexRectsChanged(bool);
  void removeBordersChanged(bool);
  void emulateFramebufferChanged(bool);
  void accurateDepthCompareChanged(bool);
  void rspEmulationChanged(bool);
  void showMorePluginsChanged(bool);
  void saveSyncingChanged(bool);
  void activeSdCardChanged(std::string);
};

class RomSettingsWidgetSlot : public QFrame {
  Q_OBJECT

public:
  RomSettingsWidgetSlot(QWidget *parent = nullptr);
  virtual ~RomSettingsWidgetSlot() {}

  void place(RomSettingsWidget *widget);
  RomSettingsWidget *widget() const;

  inline bool hasWidget() const { return this->widget() != nullptr; }
};

#endif /* SRC_UI_ROM_SETTINGS_WIDGET_HPP_ */

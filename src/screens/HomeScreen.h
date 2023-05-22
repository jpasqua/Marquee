#ifndef HomeScreen_h
#define HomeScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp
#include <gui/Display.h>
#include <screens/matrix/SettingsOwner.h>
//                                  Local Includes
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------

class HSSettings : public BaseSettings {
public:
  HSSettings();

  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  uint32_t displayTime;  // Seconds to stay on the home screen before advancing
  enum {PBO_Horizontal, PBO_Vertical, PBO_None} pbOrientation;
};

class HomeScreen : public Screen, public SettingsOwner {
public:
  HomeScreen();
  virtual void display(bool) override;
  virtual void processPeriodicActivity() override;
  virtual void settingsHaveChanged();
  virtual BaseSettings* getSettings() { return &settings; }

  HSSettings settings;

private:
  uint32_t _colonLastToggledAt = 0;
  uint32_t _nextScreenTime = 0;
  uint16_t _compositeTime = 0;
  bool _colonVisible = true;
  bool _showInfoScreen;

  void toggleColon();
  void drawProgressBar();
};

#endif  // HomeScreen_h

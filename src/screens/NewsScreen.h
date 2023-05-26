/*
 * NewsScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef NewsScreen_h
#define NewsScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
#include <screens/matrix/SettingsOwner.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class NSSettings : public BaseSettings {
public:
  NSSettings();

  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  String lang;
  String country;
  String source;
  String apiKey;
};

class NewsScreen : public ScrollScreen, public SettingsOwner {
public:
  NewsScreen();
  virtual void innerActivation() override;
  virtual void settingsHaveChanged();
  virtual BaseSettings* getSettings() { return &settings; }

  NSSettings settings;

private:
  void updateText();
};

#endif  // NewsScreen_h
#endif
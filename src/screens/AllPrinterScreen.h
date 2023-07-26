/*
 * AllPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef AllPrinterScreen_h
#define AllPrinterScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <vector>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
#include <screens/matrix/SettingsOwner.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class APSSettings : public BaseSettings {
public:
  struct Field {
    Field() = default;
    Field(const String& theID, bool on) : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  APSSettings();

  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  std::vector<Field> fields;
  bool excludeNext;
};

class AllPrinterScreen : public ScrollScreen, public SettingsOwner {
public:
  AllPrinterScreen();
  virtual void innerActivation() override;
  virtual void settingsHaveChanged() override;
  virtual BaseSettings* getSettings() { return &settings; }

  APSSettings settings;

private:
  String    _statusText;

  void updateText();

};

#endif  // AllPrinterScreen_h
#endif
/*
 * NextPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef NextPrinterScreen_h
#define NextPrinterScreen_h

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

class NPSSettings : public BaseSettings {
public:
  struct Field {
    Field() = default;
    Field(const String& theID, bool on) : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  NPSSettings();

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
};


class NextPrinterScreen : public ScrollScreen, public SettingsOwner {
public:
  NextPrinterScreen();
  virtual void innerActivation() override;
  virtual void settingsHaveChanged() override;
  virtual BaseSettings* getSettings() { return &settings; }

  NPSSettings settings;

private:
  static constexpr uint32_t UpdateInterval = 30 * 1000L;
  String    _statusText;

  void updateText();

};

#endif  // NextPrinterScreen_h
#endif
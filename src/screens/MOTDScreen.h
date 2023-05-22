/*
 * MOTDScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef MOTDScreen_h
#define MOTDScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
#include <screens/matrix/SettingsOwner.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

struct TimeMessages {
  uint8_t startHour;
  uint8_t endHour;
  std::vector<String> msgs;
};

struct DayMessages {
  uint8_t month;
  uint8_t day;
  std::vector<String> msgs;
};

class Messages : public BaseSettings {
public:
  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  std::vector<TimeMessages> timeMsgs;
  std::vector<DayMessages> dayMsgs;
  std::vector<String> daysOfTheWeek[7];
  uint8_t dotwCount = 10;
};



class MOTDScreen : public ScrollScreen, public SettingsOwner {
public:
  static constexpr size_t MaxDocSize = 8192;
  
  MOTDScreen();
  virtual void innerActivation() override;
  virtual void settingsHaveChanged() {};
  virtual BaseSettings* getSettings() { return &msgs; }

private:
  Messages msgs;
  uint16_t counter = 0;

  void updateText();
};

#endif  // MOTDScreen_h
#endif
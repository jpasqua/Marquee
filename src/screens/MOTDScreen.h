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
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
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

class Messages {
public:
  std::vector<TimeMessages> timeMsgs;
  std::vector<DayMessages> dayMsgs;
  std::vector<String> daysOfTheWeek[7];
  uint8_t dotwCount = 10;

  void fromJSON(DynamicJsonDocument& doc);
  void toLog();
};



class MOTDScreen : public ScrollScreen {
public:
  static constexpr size_t MaxDocSize = 8192;
  
  MOTDScreen();
  virtual void innerActivation() override;

private:
  bool firstActivation = true;
  Messages msgs;
  uint16_t counter = 0;

  void readMessages(String filePath);

  void updateText();

};

#endif  // MOTDScreen_h
#endif
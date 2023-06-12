/*
 * MQSettings.h
 *    Setting for the MarqueeApp.
 *
 */

#ifndef MQSettings_h
#define MQSettings_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <BPA_PrinterSettings.h>
//                                  WebThingApp Includes
#include <WTAppSettings.h>
//                                  Local Includes
#include "MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


class SensorSettings : public BaseSerializer {
public:
  void fromJSON(const JsonDocument& doc) override;
  void toJSON(JsonDocument& doc) override;
  void logSettings() override;

  float   tempCorrection = 0.0; // Correction to the temp reading - always in Celsius
  float   humiCorrection = 0.0; // Correction to the humidty sensor reading
};

class MQSettings: public WTAppSettings {
public:
  // ----- Constructors and methods
  MQSettings();
  void fromJSON(const JsonDocument &doc) override;
  void toJSON(JsonDocument &doc);
  void logSettings();

  struct {
    String username;
    String key;
    String groupName;
  } aio;

  uint16_t scrollDelay;

  static constexpr uint8_t MaxPrinters = 4;
  bool printMonitorEnabled = false;
  PrinterSettings printer[MaxPrinters];
  uint32_t printerRefreshInterval = 10;

  SensorSettings sensorSettings;

private:
  // ----- Constants -----
  static constexpr uint32_t CurrentVersion = 0x0001;
};
#endif // MQSettings_h
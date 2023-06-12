/*
 * MQSettings
 *    Setting for the MarqueeApp.
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <ArduinoJson.h>
//                                  Local Includes
#include "MarqueeApp.h"
#include "MQSettings.h"
//--------------- End:    Includes ---------------------------------------------


//
// ===== SensorSettings =====
//

void SensorSettings::fromJSON(const JsonDocument &doc) {
  tempCorrection = doc["sensorSettings"]["tempCorrection"];
  humiCorrection = doc["sensorSettings"]["humiCorrection"];
}

void SensorSettings::toJSON(JsonDocument &doc) {
  doc["sensorSettings"]["tempCorrection"] = tempCorrection;
  doc["sensorSettings"]["humiCorrection"] = humiCorrection;
}

void SensorSettings::logSettings() {
  Log.verbose(F("Sensor Settings"));
  Log.verbose(F("  tempCorrection: %d"), tempCorrection);
  Log.verbose(F("  humiCorrection: %d"), humiCorrection);
}

//
// ===== MQSettings =====
//

MQSettings::MQSettings() {
  maxFileSize = 4096;
  version = MQSettings::CurrentVersion;
  for (int i = 0; i < MaxPrinters; i++) { printer[i].init(); }
}

void MQSettings::fromJSON(const JsonDocument &doc) {
  // ----- General Marquee Settings
  scrollDelay = doc["scrollDelay"] | 20;

  // ----- Adafruit IO
  aio.username = String(doc["aioUsername"]|"");
  aio.key = String(doc["aioKey"]|"");
  aio.groupName = String(doc["aioGroup"]|"");

  // ----- Printer Monitor
  printMonitorEnabled = doc[F("printMonitorEnabled")];
  printerRefreshInterval = doc[F("printerRefreshInterval")];
  if (printerRefreshInterval == 0) printerRefreshInterval = 30; // Sanity check

  int i = 0;
  JsonArrayConst osArray = doc[F("printerSettings")];
  for (JsonObjectConst os : osArray) {
    printer[i++].fromJSON(os);
    if (i == MaxPrinters) break;
  }

  // ----- Sensor Settings
  sensorSettings.fromJSON(doc);

  // ----- WTApp Settings
  WTAppSettings::fromJSON(doc);

  logSettings();
}

void MQSettings::toJSON(JsonDocument &doc) {
  // ----- General Marquee Settings
  doc[F("scrollDelay")] = scrollDelay;

  // ----- Adafruit IO
  doc["aioUsername"] = aio.username;
  doc["aioKey"] = aio.key;
  doc["aioGroup"] = aio.groupName;

  // ----- Printer Monitor
  doc[F("printMonitorEnabled")] = printMonitorEnabled;
  doc[F("printerRefreshInterval")] = printerRefreshInterval;
  JsonArray printerSettings = doc.createNestedArray(F("printerSettings"));
  for (int i = 0; i < MaxPrinters; i++) {
    printer[i].toJSON(printerSettings.createNestedObject());
  }

  // ----- Sensor Settings
  sensorSettings.toJSON(doc);

  // ----- WTApp Settings
  WTAppSettings::toJSON(doc);
}

void MQSettings::logSettings() {
  Log.verbose(F("Marquee Settings"));
  Log.verbose(F("  General Settings"));
  Log.verbose(F("    scrollDelay = %d"), scrollDelay);
  Log.verbose(F("  Adafruit IO Settings"));
  Log.verbose(F("    aio.username = %s"), aio.username.c_str());
  Log.verbose(F("    aio.key = %s"), aio.key.c_str());
  Log.verbose(F("    aio.groupName = %s"), aio.groupName.c_str());
  Log.verbose(F("  Print Monitor Settings"));
  Log.verbose(F("    Enabled: %T"), printMonitorEnabled);
  Log.verbose(F("    Refresh interval: %d"), printerRefreshInterval);
  Log.verbose(F("    Printer Configuration"));
  for (int i = 0; i < MaxPrinters; i++) {
    Log.verbose(F("  Printer Settings %d"), i);
    printer[i].logSettings();
  }
  sensorSettings.logSettings();
  WTAppSettings::logSettings();
}

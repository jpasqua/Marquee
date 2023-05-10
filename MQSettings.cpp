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


MQSettings::MQSettings() {
  maxFileSize = 4096;
  version = MQSettings::CurrentVersion;
  for (int i = 0; i < MaxPrinters; i++) { printer[i].init(); }
}

void MQSettings::fromJSON(const JsonDocument &doc) {
  // ----- General Marquee Settings
  scrollDelay = doc["scrollDelay"] | 20;
  homeScreenTime = doc["homeScreenTime"] | 20;

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

  JsonObjectConst singlePrinterFields = doc["singlePrinter"];
  singlePrinter.fromJSON(singlePrinterFields);
  JsonObjectConst allPrintersFields = doc["allPrinters"];
  allPrinters.fromJSON(allPrintersFields);

  String hspType = doc["hsp"];
  homeScreenProgress = HSP_None;
  if (hspType.equalsIgnoreCase("hortizontal")) homeScreenProgress = HSP_Horizontal;
  else if (hspType.equalsIgnoreCase("vertical")) homeScreenProgress = HSP_Vertical;

  // ----- WTApp Settings
  WTAppSettings::fromJSON(doc);
  logSettings();
}

void MQSettings::toJSON(JsonDocument &doc) {
  // ----- General Marquee Settings
  doc[F("scrollDelay")] = scrollDelay;
  doc[F("homeScreenTime")] = homeScreenTime;

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

  JsonObject singleprinterObj = doc.createNestedObject("singlePrinter");
  singlePrinter.toJSON(singleprinterObj);
  JsonObject allPrintersObj = doc.createNestedObject("allPrinters");
  allPrinters.toJSON(allPrintersObj);

  switch (homeScreenProgress) {
    case HSP_Horizontal: doc["hsp"] = "Hortizontal"; break;
    case HSP_Vertical: doc["hsp"] = "Hortizontal"; break;
    default: doc["hsp"] = "None"; break;
  }

  // ----- WTApp Settings
  WTAppSettings::toJSON(doc);
}

void MQSettings::logSettings() {
  Log.verbose(F("Marquee Settings"));
  Log.verbose(F("  General Settings"));
  Log.verbose(F("    scrollDelay = %d"), scrollDelay);
  Log.verbose(F("    homeScreenTime = %d"), homeScreenTime);
  Log.verbose(F("  Adafruit IO Settings"));
  Log.verbose(F("    aio.username = %s"), aio.username.c_str());
  Log.verbose(F("    aio.key = %s"), aio.key.c_str());
  Log.verbose(F("    aio.groupName = %s"), aio.groupName.c_str());
  Log.verbose(F("  Print Monitor Settings"));
  Log.verbose(F("    Enabled: %T"), printMonitorEnabled);
  Log.verbose(F("    Refresh interval: %d"), printerRefreshInterval);
  Log.verbose(F("    Home Screen Progress: %s"), 
      homeScreenProgress == HSP_Horizontal ? "Horizontal" :
      (homeScreenProgress == HSP_Vertical ? "Vertical" : "None"));
  Log.verbose(F("    Single-Printer Display Fields"));
  singlePrinter.logSettings();
  Log.verbose(F("    All-Printers Display Fields"));
  allPrinters.logSettings();
  Log.verbose(F("    Printer Configuration"));
  for (int i = 0; i < MaxPrinters; i++) {
    Log.verbose(F("  Printer Settings %d"), i);
    printer[i].logSettings();
  }

  WTAppSettings::logSettings();
}

void PrinterFields::fromJSON(JsonObjectConst &obj) {
  printerName = obj["printerName"]|true;
  fileName = obj["fileName"]|true;
  pct = obj["pct"]|true;
  completeAt = obj["completeAt"]|true;
}

void PrinterFields::toJSON(JsonObject &obj) {
  obj["printerName"] = printerName;
  obj["fileName"] = fileName;
  obj["pct"] = pct;
  obj["completeAt"] = completeAt;
}

void PrinterFields::logSettings() {
  Log.verbose(F("      printer name: %T"), printerName);
  Log.verbose(F("      file name: %T"), fileName);
  Log.verbose(F("      pct: %T"), pct);
  Log.verbose(F("      completeAt: %T"), completeAt);
}

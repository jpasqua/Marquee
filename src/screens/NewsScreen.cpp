
/*
 * NewsScreen:
 *    Display news headlines 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  WebThingApp Includes
#include <gui/Display.h>
//                                  Local Includes
#include "NewsScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

NewsScreen::NewsScreen() {
  // Default values
  settings.country = "any";
  settings.lang = "any";
  settings.source = "abc-news";
  settings.apiKey = "";

  settings.read();  // If no file exists, one will be created with dflt settings

  nLabels = 0;
  labels = NULL;

  init();
}

void NewsScreen::innerActivation() {
  updateText();
}

void NewsScreen::settingsHaveChanged() {
  updateText();
  settings.write();
}


//
// ----- NewsScreen Private Functions
//

void NewsScreen::updateText() {
  String text = "Source: " + settings.source + " (" + settings.lang + ", " + settings.country + ")";
  setText(text, Display.BuiltInFont_ID);
}

/*------------------------------------------------------------------------------
 *
 * Implementation of the NSSettings Class
 *
 *----------------------------------------------------------------------------*/

NSSettings::NSSettings() {
  maxFileSize = 512;
  version = 1;
  init("/NSSettings.json");
}

void NSSettings::fromJSON(const JsonDocument& doc) {
  lang = doc["lang"].as<String>();
  country = doc["country"].as<String>();
  source = doc["source"].as<String>();
  apiKey = doc["apiKey"].as<String>();
}

void NSSettings::toJSON(JsonDocument& doc) {
  doc["lang"] = lang;
  doc["country"] = country;
  doc["source"] = source;
  doc["apiKey"] = apiKey;
}

void NSSettings::logSettings() {
  Log.verbose("NewsScreen Settings");
  Log.verbose("  language filter: %s", lang.c_str());
  Log.verbose("  country filter: %s", country.c_str());
  Log.verbose("  news source: %s", source.c_str());
  Log.verbose("  API Key: %s", apiKey.c_str());
}

#endif








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
#include "../../MarqueeApp.h"
#include "NewsScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

NewsScreen::NewsScreen() {
  // Default values
  settings.enabled = true;
  settings.country = "any";
  settings.lang = "any";
  settings.source = "abc-news";
  settings.apiKey = "";
  settings.refreshInterval = 15;

  settings.read();  // If no file exists, one will be created with dflt settings

  _wasEnabled = settings.enabled;

  nLabels = 0;
  labels = NULL;

  init();
}

void NewsScreen::innerActivation() {
  updateText();
}

void NewsScreen::settingsHaveChanged() {
  mqApp->newsClient->updateSettings(settings.source, settings.apiKey);
  if (settings.enabled != _wasEnabled) {
    if (_wasEnabled == false) {
      mqApp->app_conditionalUpdate(false);  // Refresh the news
    }
    _wasEnabled = settings.enabled;
  }
  updateText();
  settings.write();
}


//
// ----- NewsScreen Private Functions
//

void NewsScreen::updateText() {
  String text = "";
  if (settings.enabled) {
    if (mqApp->newsClient) {
        if (mqApp->newsClient->stories.size() != 0) {
          text += settings.source;
          if (curStory >= mqApp->newsClient->stories.size()) curStory = 0;
          text += ": ";
          text += mqApp->newsClient->stories[curStory++].title;
        }
    }
  }
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
  enabled = doc["enabled"];
  lang = doc["lang"].as<String>();
  country = doc["country"].as<String>();
  source = doc["source"].as<String>();
  apiKey = doc["apiKey"].as<String>();
  refreshInterval = doc["refreshInterval"];
  if (refreshInterval == 0) refreshInterval = 15;
}

void NSSettings::toJSON(JsonDocument& doc) {
  doc["enabled"] = enabled;
  doc["lang"] = lang;
  doc["country"] = country;
  doc["source"] = source;
  doc["apiKey"] = apiKey;
  doc["refreshInterval"] = refreshInterval;
}

void NSSettings::logSettings() {
  Log.verbose("NewsScreen Settings");
  Log.verbose("  Enabled: %T", enabled);
  Log.verbose("  Language filter: %s", lang.c_str());
  Log.verbose("  Country filter: %s", country.c_str());
  Log.verbose("  News Source: %s", source.c_str());
  Log.verbose("  API Key: %s", apiKey.c_str());
  Log.verbose("  Refresh Interval: %s", refreshInterval);
}

#endif







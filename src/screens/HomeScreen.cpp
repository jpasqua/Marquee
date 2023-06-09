#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <Output.h>
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
#include <gui/Theme.h>
//                                  Local Includes
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


static inline uint16_t compose(int h, int m) { return(h * 100 + m); }

HomeScreen::HomeScreen(bool autoAdvance) {
  _autoAdvance = autoAdvance;

  settings.displayTime = UINT32_MAX;
  settings.read();

  if (settings.displayTime == UINT32_MAX) {
    settings.displayTime = 240;
    settings.pbOrientation = HSSettings::PBO_Vertical;
    settings.write();
  }
}

void HomeScreen::display(bool activating) {
  auto& mtx = Display.mtx;

  if (activating) {
    Display.setFont(Display.BuiltInFont_ID);
    _nextScreenTime = millis() + settings.displayTime*1000L;
  }

  _colonVisible = false;
  mtx->fillScreen(Theme::Color_BLACK);

  int  m = minute();
  int  h = mqSettings->uiOptions.use24Hour ? hour() : hourFormat12();
  _compositeTime = compose(hour(), m);

  char timeBuf[4];
  timeBuf[0] = (h/10) + '0';
  timeBuf[1] = (h%10) + '0';
  if (timeBuf[0] == '0') timeBuf[0] = ' ';
  timeBuf[2] = (m/10) + '0';
  timeBuf[3] = (m%10) + '0';

  mtx->drawChar( 1, 0, timeBuf[0], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar( 6, 0, timeBuf[1], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(14, 0, timeBuf[2], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(20, 0, timeBuf[3], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  if (!mqSettings->uiOptions.use24Hour) {
    mtx->drawChar(26, 0, isAM() ? 'a' : 'p', Theme::Color_WHITE, Theme::Color_BLACK, 1);
    mtx->drawChar(32, 0, 'm', Theme::Color_WHITE, Theme::Color_BLACK, 1);    
  }

  toggleColon();

  if ((Display.width() >= 64) && (mqApp->owmClient != nullptr)) {
    constexpr int charWidthIncludingSpace = 6;
    int temp = 0;
#if defined(HAS_WEATHER_SENSOR)
    if (mqApp->weatherMgr.hasTemp()) temp = std::round(Output::temp(mqApp->weatherMgr.getLastReadings().temp));
#else
    temp = std::round(mqApp->owmClient->weather.readings.temp);
#endif
    int nChars = 1; // For the 'C' or 'F'
    nChars += temp < 0 ? 1 : 0; // Add space for the minus sign if needed
    nChars += temp > 99 ? 2 : (temp > 9 ? 2 : 1); // Space for the digits
    uint16_t x = Display.width() - (nChars * charWidthIncludingSpace);
    uint16_t y = 0;
    mtx->setCursor(x, y);
    mtx->print(temp);
    mtx->print(mqApp->settings->uiOptions.useMetric ? 'C' : 'F');
  }

  if (mqSettings->printMonitorEnabled) drawProgressBar();

  mtx->write();
}

void HomeScreen::processPeriodicActivity() {
  static uint32_t _colonLastToggledAt = 0;
  uint32_t curMillis = millis();
  if (_autoAdvance && curMillis > _nextScreenTime) {
    ScreenMgr.moveThroughSequence(true);
    return;
  }
  if (_compositeTime != compose(hour(), minute())) {
    display(false);
    _colonLastToggledAt = curMillis;
    return;
  }
  if (curMillis >= _colonLastToggledAt + 500L) {
    toggleColon();
    Display.mtx->write();
    _colonLastToggledAt = curMillis;
  }
}

void HomeScreen::settingsHaveChanged() {
  settings.write();
  _nextScreenTime = 0;  // Force an update
}

// ----- Private Functions

void HomeScreen::toggleColon() {
  _colonVisible = !_colonVisible;
  Display.mtx->writePixel(12, 2, _colonVisible);
  Display.mtx->writePixel(12, 4, _colonVisible);
}

void HomeScreen::drawProgressBar() {
  if (settings.pbOrientation == HSSettings::PBO_None) return;
  bool isVert = settings.pbOrientation == HSSettings::PBO_Vertical;

  uint8_t whichPrinter;
  String completionTime;
  uint32_t timeRemaining;

  if (mqApp->printerGroup->nextCompletion(whichPrinter, completionTime, timeRemaining)) {
    PrintClient* p = mqApp->printerGroup->getPrinter(whichPrinter);
    float pct = p->getPctComplete()/100.0;
    uint8_t pctPixels = (isVert ? Display.height() : Display.width()) * pct;
    if (pctPixels == 0) pctPixels = 1;
    if (isVert) {
      Display.mtx->drawFastVLine(0, Display.height()-pctPixels, pctPixels, Theme::Color_WHITE);
    } else {
      Display.mtx->drawFastHLine(0, Display.height()-1, pctPixels, Theme::Color_WHITE);
    }
  }
}

/*------------------------------------------------------------------------------
 *
 * HSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

HSSettings::HSSettings() {
  maxFileSize = 128;
  version = 1;
  init("/HSSettings.json");
}

void HSSettings::fromJSON(const JsonDocument &doc) {
  displayTime = doc["displayTime"];
  String pboAsString = doc["pbOrientation"];
  pbOrientation = PBO_None;
  if (pboAsString.equalsIgnoreCase("Horizontal")) pbOrientation = PBO_Horizontal;
  else if (pboAsString.equalsIgnoreCase("vertical")) pbOrientation = PBO_Vertical;
}

void HSSettings::toJSON(JsonDocument &doc) {
  doc["displayTime"] = displayTime;
  switch (pbOrientation) {
    case PBO_Horizontal: doc["pbOrientation"] = "Horizontal"; break;
    case PBO_Vertical: doc["pbOrientation"] = "Vertical"; break;
    default: doc["pbOrientation"] = "None"; break;
  }
}

void HSSettings::logSettings() {
  Log.verbose(F("HomeScreen Settings"));
  Log.verbose(F("  displayTime: %d"), displayTime);
  Log.verbose(F("  Progress Bar Orientation: %s"), 
      pbOrientation == PBO_Horizontal ? "Horizontal" :
      (pbOrientation == PBO_Vertical ? "Vertical" : "None"));
}

#endif
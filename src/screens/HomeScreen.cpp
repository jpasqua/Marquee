#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
#include <gui/Theme.h>
//                                  Local Includes
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


static inline uint16_t compose(int h, int m) { return(h * 100 + m); }

HomeScreen::HomeScreen() {
  _showInfoScreen = true;
}

void HomeScreen::display(bool activating) {
  auto& mtx = Display.mtx;

  if (_showInfoScreen) {
    _showInfoScreen = false;
    wtAppImpl->screens.infoScreen->goHomeWhenComplete(true);
    ScreenMgr.display(wtAppImpl->screens.infoScreen);
    return;
  }

  if (activating) {
    Display.setFont(Display.BuiltInFont_ID);
    _nextScreenTime = millis() + mqSettings->homeScreenTime*1000L;
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

  mtx->drawChar( 0, 0, timeBuf[0], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar( 6, 0, timeBuf[1], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(14, 0, timeBuf[2], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(20, 0, timeBuf[3], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  if (!mqSettings->uiOptions.use24Hour) {
    mtx->drawChar(26, 0, isAM() ? 'a' : 'p', Theme::Color_WHITE, Theme::Color_BLACK, 1);
    mtx->drawChar(32, 0, 'm', Theme::Color_WHITE, Theme::Color_BLACK, 1);    
  }

  toggleColon();

  if ((mtx->width() >= 64) && (mqApp->owmClient != nullptr)) {
    constexpr int charWidthIncludingSpace = 6;
    int temp = mqApp->owmClient->weather.readings.temp; // Rounds the value
    int nChars = 1; // For the 'C' or 'F'
    nChars += temp < 0 ? 1 : 0; // Add space for the minus sign if needed
    nChars += temp > 99 ? 2 : (temp > 9 ? 2 : 1); // Space for the digits
    uint16_t x = mtx->width() - (nChars * charWidthIncludingSpace);
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
  if (curMillis > _nextScreenTime) {
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


// ----- Private Functions

void HomeScreen::toggleColon() {
  _colonVisible = !_colonVisible;
  Display.mtx->writePixel(12, 2, _colonVisible);
  Display.mtx->writePixel(12, 4, _colonVisible);
}

void HomeScreen::drawProgressBar() {
  if (mqSettings->homeScreenProgress == MQSettings::HSP_None) return;
  bool isVert = mqSettings->homeScreenProgress == MQSettings::HSP_Vertical;

  uint8_t whichPrinter;
  String completionTime;
  uint32_t timeRemaining;

  String printerName;
  String fileName;

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

#endif
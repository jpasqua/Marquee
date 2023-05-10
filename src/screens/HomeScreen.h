#ifndef HomeScreen_h
#define HomeScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
//                                  Local Includes
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


class HomeScreen : public Screen {
public:
  HomeScreen();
  virtual void display(bool) override;
  virtual void processPeriodicActivity() override;

private:
  uint32_t _colonLastToggledAt = 0;
  uint32_t _nextScreenTime = 0;
  uint16_t _compositeTime = 0;
  bool _colonVisible = true;
  bool _showInfoScreen;

  void toggleColon();
  void drawProgressBar();
};

#endif  // HomeScreen_h

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
//                                  Local Includes
#include <gui/Display.h>
//--------------- End:    Includes ---------------------------------------------


class SplashScreen : public Screen {
public:
  virtual void display(bool) override {
    auto& mtx = Display.mtx;

    mtx->fillScreen(Theme::Color_BLACK);
    Display.setFont(Display.BuiltInFont_ID);
    mtx->setCursor(0, 0);
    if (Display.width() >= 64) {
      mtx->print("Starting");
      mtx->setCursor(46, 0); mtx->print(".");
      mtx->setCursor(49, 0); mtx->print(".");
      mtx->setCursor(52, 0); mtx->print(".");
    } else {
      mtx->print("Ready");
    }
    Display.mtx->write();
  }
  virtual void processPeriodicActivity() override { }

};

#endif
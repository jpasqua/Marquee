/*
 * NextPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef NextPrinterScreen_h
#define NextPrinterScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class NextPrinterScreen : public ScrollScreen {
public:
  NextPrinterScreen();
  virtual void innerActivation() override;

private:
  static constexpr uint32_t UpdateInterval = 30 * 1000L;
  String    _statusText;

  void updateText();

};

#endif  // NextPrinterScreen_h
#endif
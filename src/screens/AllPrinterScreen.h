/*
 * AllPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef AllPrinterScreen_h
#define AllPrinterScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class AllPrinterScreen : public ScrollScreen {
public:
  AllPrinterScreen();
  virtual void innerActivation() override;

private:
  String    _statusText;

  void updateText();

};

#endif  // AllPrinterScreen_h
#endif
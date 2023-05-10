/*
 * MarqueeApp:
 *    This is the core of the MultiMon functionality. It is invoked
 *    by the primary sketch ".ino" file.
 *
 * NOTES:
 * o MarqueeApp is derived from WTAppImpl, which is itself derived
 *   from WTApp.
 * o This is a singleton and is made available to the whole app via
 *   an extern declaration in WTApp.h:
 *       WTApp* wtApp;
 * o This must be cast back to MarqueeApp* in order to get to app-specific
 *   data and functions. Technically this should be a dynamic_cast, but for
 *   efficiency, we perform a "C-style" cast.
 * o Macros are provided to easily get the app and settings in their
 *   specialized forms.
 *
 * Customization:
 * o To add a new screen to the app, declare it here and instantiate it
 *   in the associated .cpp file.
 */


#ifndef MarqueeApp_h
#define MarqueeApp_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <BPA_PrinterGroup.h>
//                                  WebThing Includes
#include <WTAppImpl.h>
//                                  Local Includes
#include "MQSettings.h"
#include "src/screens/SplashScreen.h"
#include "src/screens/HomeScreen.h"
#include "src/screens/NextPrinterScreen.h"
#include "src/screens/AllPrinterScreen.h"
#include "src/screens/MOTDScreen.h"
//--------------- End:    Includes ---------------------------------------------


// A few convenience macros:
// mqApp simply yields the wtApp singleton cast as a MarqueeApp*
// mqSettings yields the settings object from wtApp, cast as a MQSettings*
#define mqApp ((MarqueeApp*)wtApp)
#define mqSettings ((MQSettings*)mqApp->settings)


class MarqueeApp : public WTAppImpl {
public:
  static constexpr int MaxPrinters = 4;

  static void create();

  // CUSTOM: Screens implemented by this app
  SplashScreen*       splashScreen;
  HomeScreen*         homeScreen;
  NextPrinterScreen*  nextPrinterScreen;
  AllPrinterScreen*   allPrinterScreen;
  MOTDScreen*         motdScreen;

  // CUSTOM: Data defined by this app which is available to the whole app
  PrinterGroup*   printerGroup;

  // ----- Functions that *must* be provided by subclasses
  virtual void app_registerDataSuppliers() override;
  virtual Screen* app_registerScreens() override;
  virtual void app_initWebUI() override;
  virtual void app_initClients() override;
  virtual void app_conditionalUpdate(bool force = false) override;
  virtual void app_loop() override;

  // ----- Functions that *may* be provided by subclasses
  virtual void app_configureHW() override;

  // ----- Public functions
  MarqueeApp(MQSettings* settings);
  void printerWasActivated(int index);
  void fireUpPrintMonitor();

private:
  void showPrinterActivity(bool busy);

};

#endif	// MarqueeApp_h

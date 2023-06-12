/*
 * MarqueeApp:
 *    A scrolling Marquee of all sorts of useful (and not) information.
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Hardware Definitions
#include "src/hardware/HWConfig.h"
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WebThing.h>
#include <WebUI.h>
#include <DataBroker.h>
#include <clients/AIOMgr.h>
#include <sensors/WeatherUtils.h>
#include <clients/AIO_WeatherPublisher.h>
//                                  WebThingApp Includes
#include <gui/Display.h>
#include <gui/ScreenMgr.h>
#include <screens/matrix/ScrollScreen.h>
#include <plugins/PluginMgr.h>
#include <plugins/common/GenericPlugin.h>
#include <plugins/common/AIOPlugin.h>
//                                  Local Includes
#include "MarqueeApp.h"
#include "MQSettings.h"
#include "MQWebUI.h"
#include "MQDataSupplier.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constants
 *
 *----------------------------------------------------------------------------*/

// CUSTOM: Update these strings for your app:
static const char* VersionString = "0.0.6";
static const char* AppName = "Marquee";
static const char* AppPrefix = "MQ-";


/*------------------------------------------------------------------------------
 *
 * Private Utility Functions
 *
 *----------------------------------------------------------------------------*/

Plugin* pluginFactory(const String& type) {
  Plugin *p = NULL;

  // CUSTOM: Choose which plugins you'd like to load
  if      (type.equalsIgnoreCase("generic")) { p = new GenericPlugin(); }
  else if (type.equalsIgnoreCase("aio"))  { p = new AIOPlugin();  }
  

  if (p == NULL) {
    Log.warning("Unrecognized plugin type: %s", type.c_str());
  }
  return p;
}

/*------------------------------------------------------------------------------
 *
 * Class function to create and start the MarqueeApp singleton
 *
 *----------------------------------------------------------------------------*/

static MQSettings theSettings;  // Allocate storage for the app settings

void MarqueeApp::create() {
  // CUSTOM: Perform any low level initialization that may be required, this
  // is often related to initializing hardware
  
  // Set the display options before we fire up the display!
  Display.setDeviceOptions(&hwConfig.displayDeviceOptions);

  // BOILERPLATE
  PluginMgr::setFactory(pluginFactory);
  MarqueeApp* app = new MarqueeApp(&theSettings);

  app->begin();
}


/*------------------------------------------------------------------------------
 *
 * MarqueeApp Public Functions
 *
 *----------------------------------------------------------------------------*/

MarqueeApp::MarqueeApp(MQSettings* settings) :
    WTAppImpl(AppName, AppPrefix, VersionString, settings)
{
  // CUSTOM: Perform any object initialization here
}

void MarqueeApp::printerWasActivated(int index) {
  if (mqSettings->printMonitorEnabled) {
  // TO DO: May need to cache the IP!!!
    printerGroup->activatePrinter(index);
  }
}


/*------------------------------------------------------------------------------
 *
 * Mandatory WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void MarqueeApp::app_registerDataSuppliers() {
  // BOILERPLATE
  DataBroker::registerMapper(MQDataSupplier::dataSupplier, MQDataSupplier::ERPrefix);
  if (mqSettings->printMonitorEnabled) {
    DataBroker::registerMapper(
        [this](const String& key,String& val) { this->printerGroup->dataSupplier(key, val); },
        PrinterGroup::DataProviderPrefix);
  }
}

void MarqueeApp::app_initWebUI() {
  // BOILERPLATE
  MQWebUI::init();
}

void MarqueeApp::app_loop() {
  // CUSTOM: Perform any app-specific periodic activity
  // Note that app_conditionalUpdate() is called for you automatically on a
  // periodic basis, so no need to do that here.

  // In this case, nothing app-specific is required
}

void MarqueeApp::app_initClients() {
  // CUSTOM: If your app has any app-specific clients, initilize them now

  fireUpPrintMonitor();
  newsClient = new NewsClient(newsScreen->settings.source, newsScreen->settings.apiKey);

  ScreenMgr.showActivityIcon(Theme::Color_WHITE);
  prepAIO();
  ScreenMgr.hideActivityIcon();
}

void MarqueeApp::app_conditionalUpdate(bool force) {
  // CUSTOM: Update any app-specific clients

  #if defined(HAS_WEATHER_SENSOR)
    static bool startingUp = true;
  
    weatherMgr.takeReadings(force);
    if (!startingUp) AIOMgr::publish();
    startingUp = false;
  #endif

  if (mqSettings->printMonitorEnabled) {
    printerGroup->refreshPrinterData(force);
  }

  static uint32_t lastNewsUpdate = 0;
  if (newsScreen->settings.enabled) {
    if (force || (millis() > (lastNewsUpdate + Basics::minutesToMS(newsScreen->settings.refreshInterval)))) {
      Log.verbose("Updating news");
      newsClient->update();
      // newsClient->dump();
      lastNewsUpdate = millis();
    }
  }
}

Screen* MarqueeApp::app_registerScreens() {
  // CUSTOM: Register any app-specific Screen objects
  splashScreen = new SplashScreen();
  homeScreen = new HomeScreen();
  nextPrinterScreen = new NextPrinterScreen();
  allPrinterScreen = new AllPrinterScreen();
  motdScreen = new MOTDScreen();
  newsScreen = new NewsScreen();
  dateScreen = new DateScreen();
  
  ScreenMgr.registerScreen("Splash", splashScreen, true);
  ScreenMgr.registerScreen("Home", homeScreen);
  ScreenMgr.registerScreen("Date", dateScreen);
  ScreenMgr.registerScreen("NextPrint", nextPrinterScreen);
  ScreenMgr.registerScreen("AllPrints", allPrinterScreen);
  ScreenMgr.registerScreen("MOTD", motdScreen);
  ScreenMgr.registerScreen("News", newsScreen);
  ScreenMgr.setAsHomeScreen(homeScreen);

  // CUSTOM: Associate a confirm/cancel buttons with the reboot screen
  screens.rebootScreen->setButtons(hwConfig.advanceButton, hwConfig.previousButton);

  // CUSTOM: Add a sequence of screens that the user can cycle through
  auto& sequence = ScreenMgr.sequence;
  sequence.push_back(homeScreen);
  sequence.push_back(wtAppImpl->screens.weatherScreen);
  sequence.push_back(wtAppImpl->screens.forecastScreen);
  // Add any plugins to the sequence
  uint8_t nPlugins = pluginMgr.getPluginCount();
  for (int i = 0; i < nPlugins; i++) {
    Plugin* p = pluginMgr.getPlugin(i);
    sequence.push_back(p->getFlexScreen());
  }
  // sequence.push_back(wtAppImpl->screens.infoScreen);

  ScreenMgr.reconcileScreenSequence(mqSettings->screenSettings);
if (mqSettings->scrollDelay == 0) {
  Log.warning("MarqueeApp::app_registerScreens: mqSettings->scrollDelay == 0");
  mqSettings->scrollDelay = 40;
}
  ScrollScreen::setDefaultFrameDelay(mqSettings->scrollDelay);

  return splashScreen;
}

/*------------------------------------------------------------------------------
 *
 * Optional WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void MarqueeApp::app_configureHW() {
  // CUSTOM: Register any physical buttons that are connected

  // Initialize the synthetic grounds
  for (int i = 0; i < hwConfig.nSyntheticGrounds; i++) {
    Basics::Pin pin = hwConfig.syntheticGrounds[i];
    if (pin != Basics::UnusedPin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);      
    }
  }

  // Initialize the buttons
  for (int i = 0; i < hwConfig.nPhysicalButtons; i++) {
    Basics::Pin pin = hwConfig.physicalButtons[i];
    if (pin != Basics::UnusedPin) {
      WebThing::buttonMgr.addButton(pin);
    }
  }

  ScreenMgr.setSequenceButtons(hwConfig.advanceButton, hwConfig.previousButton);

  #if defined(HAS_WEATHER_SENSOR)
    auto weatherBusyCallBack = [this](bool busy) {
      // if (busy) busyIndicator->setColor(0, 255, 0);
      // else busyIndicator->off();
    };

    WeatherUtils::configureAvailableSensors(weatherMgr);
    weatherMgr.init(
      mqSettings->sensorSettings.tempCorrection,
      mqSettings->sensorSettings.humiCorrection,
      WebThing::settings.elevation,
      weatherBusyCallBack);
  #endif
}

/*------------------------------------------------------------------------------
 *
 * MarqueeApp Private Functions
 *
 *----------------------------------------------------------------------------*/

void MarqueeApp::prepAIO() {
  if (mqSettings->aio.username.isEmpty() || mqSettings->aio.key.isEmpty()) {
    Log.trace("MarqueeApp::prepAIO: AIO username or key is empty");
    return;
  }

  auto aioBusyCallBack = [this](bool busy) {
    if (busy) ScreenMgr.showActivityIcon(Theme::Color_WHITE);
    else ScreenMgr.hideActivityIcon();
  };

  AIOMgr::init(mqSettings->aio.username, mqSettings->aio.key);
  AIOMgr::setBusyCB(aioBusyCallBack);
  AIOMgr::aio->setDefaultGroup(mqSettings->aio.groupName.c_str());

  // ----- Register the BME Publisher
  #if defined(HAS_WEATHER_SENSOR)
    AIOMgr::registerPublisher(new AIO_WeatherPublisher(&weatherMgr));
  #endif
}  

void MarqueeApp::showPrinterActivity(bool busy) {
 if (busy) ScreenMgr.showActivityIcon(Theme::Color_WHITE);
 else ScreenMgr.hideActivityIcon();
}

void MarqueeApp::fireUpPrintMonitor() {
  if (mqSettings->printMonitorEnabled) {
    printerGroup = new PrinterGroup(
      MaxPrinters, mqSettings->printer,
      mqSettings->printerRefreshInterval,
      [this](bool busy){this->showPrinterActivity(busy);});
    for (int i = 0; i < MaxPrinters; i++) {
      printerGroup->activatePrinter(i);
    }
  }
}

/*
 * MQWebUI:
 *    Implements a simple WebUI that displays and updates settings
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
#include <Output.h>
//                                  WebThing Includes
#include <WebThing.h>
#include <WebUI.h>
#include <WebUIHelper.h>
#include <gui/Display.h>
#include <screens/matrix/ScrollScreen.h>
//                                  Local Includes
#include "src/hardware/HWConfig.h"
#include "MarqueeApp.h"
#include "MQWebUI.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: MQWebUI namespace
namespace MQWebUI {

  namespace Internal {
    const __FlashStringHelper* APP_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/presentMQConfig'>"
      "<i class='fa fa-cog'></i> Configure Marquee</a>"
      "<a class='w3-bar-item w3-button' href='/presentScreenConfig'>"
      "<i class='fa fa-window-restore'></i> Select Screens</a>"
      "<a class='w3-bar-item w3-button' href='/presentPrinterConfig'>"
      "<i class='fa fa-glass'></i> Configure Printers</a>");

    void updateSinglePrinter(int i) {
      PrinterSettings* printer = &(mqSettings->printer[i]);
      String prefix = "_p" + String(i) + "_";
      printer->isActive = WebUI::hasArg(prefix + "enabled");
      printer->apiKey = WebUI::arg(prefix + "key");
      printer->server = WebUI::arg(prefix + "server");
      printer->port = WebUI::arg(prefix + "port").toInt();
      printer->user =  WebUI::arg(prefix + "user");
      printer->nickname =  WebUI::arg(prefix + "nick");
      printer->type =  WebUI::arg(prefix + "type");
      if (printer->type.equals("Duet3D")) {
        printer->pass =  WebUI::arg(prefix + "duet_pass");
      } else {
        printer->pass =  WebUI::arg(prefix + "pass");
      }
    }
  } // ----- END: MQWebUI::Internal

  // ----- BEGIN: MQWebUI::Pages
  namespace Pages {
    void presentHomePage() {
      auto mapper =[](const String& key, String& val) -> void {
        // ----- Printer-related items
        if (key.equals(F("PM_ENABLED"))) val = mqSettings->printMonitorEnabled;
        else if (key.equals(F("PRINTER_INFO"))) {
          if (!mqSettings->printMonitorEnabled) { val = "[]"; }
          else { mqApp->printerGroup->printerInfo(val); }
        }

        // ----- News-related items
        else if (key.equals(F("NS_SETTINGS"))) mqApp->newsScreen->settings.toJSON(val);

        // ----- Weather-related items
        else if (key.equals(F("CITYID"))) {
          if (wtApp->settings->owmOptions.enabled) val = wtApp->settings->owmOptions.cityID;
          else val.concat("5380748");  // Palo Alto, CA, USA
        }
        else if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
        else if (key.equals(F("UNITS"))) val = wtApp->settings->uiOptions.useMetric ? "metric" : "imperial";
      };

      WebUI::wrapWebPage("/", "/HomePage.html", mapper);
    }

    void presentMOTDPage() {
      auto mapper =[](const String &key, String& val) -> void {
        if (key == "HEADER")          val = "Message of the Day";
        else if (key == "ADJ")        val = "MOTD";
      };

      WebUI::wrapWebPage("/presentMOTDPage", "/MOTDForm.html", mapper);
    }

#define TEMP_TO_STRING(t, s) {                                    \
  if (isnan(wReadings.temp)) s = "N/A";                           \
  else s = String(Output::temp(t), 1) + Output::tempUnits();      \
}
#define HUMI_TO_STRING(h, s) {                                    \
  if (isnan(wReadings.humidity)) s = "N/A";                       \
  else s = String(wReadings.humidity, 1) + '%';                   \
}

    bool mapsensorSettings(const String& key, String& val) {
      #if defined(HAS_WEATHER_SENSOR)
        const WeatherReadings& wReadings = mqApp->weatherMgr.getLastReadings();
        if (key == "HAS_WTHR")  val = "true";
        else if (key == "TEMP_CORRECT") { 
          float tempCorrection = wtApp->settings->uiOptions.useMetric ? 
            mqSettings->sensorSettings.tempCorrection :
            Basics::delta_c_to_f(mqSettings->sensorSettings.tempCorrection);
          val.concat(tempCorrection);
        }
        else if (key == "HUMI_CORRECT") val.concat(mqSettings->sensorSettings.humiCorrection);
        else if (key == "RAW_TEMP") {
          float rawTemp = wReadings.temp - mqSettings->sensorSettings.tempCorrection;
          TEMP_TO_STRING(rawTemp, val);
        }
        else if (key == "RAW_HUMI") {
          float rawHumi = wReadings.humidity - mqSettings->sensorSettings.humiCorrection;
          HUMI_TO_STRING(wReadings.humidity, val);
        }
        else if (key == "TEMP") { TEMP_TO_STRING(wReadings.temp, val); }
        else if (key == "HUMI") { HUMI_TO_STRING(wReadings.humidity, val); }
        else return false;
        return true;
      #else
        if (key == "HAS_WTHR") { val = "false"; return true; }
        return false;
      #endif
    }

    void presentMQconfig() {
      auto mapper =[&](const String &key, String& val) -> void {
        if (key == "SCROLL_DELAY")    val = mqSettings->scrollDelay;

        else if (key == "AIO_KEY")    val = mqSettings->aio.key;
        else if (key == "AIO_USER")   val = mqSettings->aio.username;
        else if (key == "AIO_GROUP")  val = mqSettings->aio.groupName;

        else if (key.equals(F("WS_SETTINGS"))) wtAppImpl->screens.weatherScreen->settings.toJSON(val);
        else if (key.equals(F("FS_SETTINGS"))) wtAppImpl->screens.forecastScreen->settings.toJSON(val);
        else if (key.equals(F("HS_SETTINGS"))) mqApp->homeScreen->settings.toJSON(val);
        else if (key.equals(F("APS_SETTINGS"))) mqApp->allPrinterScreen->settings.toJSON(val);
        else if (key.equals(F("NPS_SETTINGS"))) mqApp->nextPrinterScreen->settings.toJSON(val);
        else if (key.equals(F("NS_SETTINGS"))) mqApp->newsScreen->settings.toJSON(val);

        else if (key.equals("PM_ENABLED")) val = mqSettings->printMonitorEnabled;

        else mapsensorSettings(key, val);
      };

      WebUI::wrapWebPage("/presentMQconfig", "/ConfigForm.html", mapper);
    }

    void presentPrinterConfig() {
      auto mapper =[](const String& key, String& val) -> void {
        if (key.startsWith("_P")) {
          int i = (key.charAt(2) - '0');
          PrinterSettings* printer = &(mqSettings->printer[i]);
          const char* subkey = &(key.c_str()[4]); // Get rid of the prefix; e.g. _P1_
          String type = "T_" + printer->type;
          if (strcmp(subkey, "ENABLED") == 0) val = WebUIHelper::checkedOrNot[printer->isActive];
          else if (strcmp(subkey, "KEY") == 0) val = printer->apiKey;
          else if (strcmp(subkey, "HOST") == 0) val =  printer->server;
          else if (strcmp(subkey, "PORT") == 0) val.concat(printer->port);
          else if (strcmp(subkey, "USER") == 0) val = printer->user;
          else if (strcmp(subkey, "PASS") == 0) val = printer->pass;
          else if (strcmp(subkey, "NICK") == 0) val = printer->nickname;
          else if (strcmp(subkey, "MOCK") == 0) val = WebUIHelper::checkedOrNot[printer->mock];
          else if (type.equals(subkey)) { val = "selected"; } 
        }
        else if (key.equals("PM_ENABLED")) val = WebUIHelper::checkedOrNot[mqSettings->printMonitorEnabled];
        else if (key.equals("SHOW_DEV")) val = WebThing::settings.showDevMenu ? "true" : "false";
          // SHOW_DEV seems odd to have here, but it's needed to control whether the 
          // "Mock"-related items are displayed.
        else if (key.equals(F("RFRSH"))) val.concat(mqSettings->printerRefreshInterval);
      };

      WebUI::wrapWebPage("/presentPrinterConfig", "/ConfigPrinters.html", mapper);
    }
  } // ----- END: MQWebUI::Pages


  // ----- BEGIN: MQWebUI::Endpoints
  namespace Endpoints {

    void ackPrinterDone() {
      auto action = [&]() {
        if (!mqApp->printerGroup) {
          Log.warning("ackPrinterDone: printerGroup is null");
          WebUI::sendStringContent("text/plain", "printer index out of range", "400 Bad Request");
          return;
        }

        int printerIndex = WebUI::arg("pi").toInt();
        if (printerIndex < 0 || printerIndex >= mqApp->printerGroup->numberOfPrinters()) {
          Log.warning("ackPrinterDone: index out of range: %d vs %d",
            printerIndex, mqApp->printerGroup->numberOfPrinters());
          WebUI::sendStringContent("text/plain", "printer index out of range", "400 Bad Request");
          return;
        }

        auto printer = mqApp->printerGroup->getPrinter(printerIndex);
        if (printer) {
          printer->acknowledgeCompletion();
          WebUI::sendStringContent("text/plain", "Printer Completion Acknowledged");
        } else {
          Log.warning("ackPrinterDone: no printer for index %d", printerIndex);
          WebUI::sendStringContent("text/plain", "printer index out of range", "400 Bad Request");
        }
      };

      WebUI::wrapWebAction("/ackPrinterDone", action);
    }

    void resetScreen() {
      auto action = []() {
        Display.mtx->reset();
        WebUI::redirectHome();
        delay(500);
      };

      WebUI::wrapWebAction("/resetScreen", action, false);
    }

    void updateSettingsForScreen(
        Screen* screen, SettingsOwner* so, const char* actionName) {
      auto action = [&]() {
        // We are handling an HTTP POST with a JSON payload. There isn't a specific function
        // to get the payload from the request, instead ask for the arg named "plain"
        String newSettings = WebUI::arg("plain");
        if (newSettings.isEmpty()) {
          WebUI::sendStringContent("text/plain", "Settings are empty", "400 Bad Request");
          return;
        }
        so->getSettings()->fromJSON(newSettings);
        so->settingsHaveChanged();
        WebUI::sendStringContent("text/plain", "New settings were saved");
      };

      WebUI::wrapWebAction(actionName, action);
      if (ScreenMgr.curScreen() == screen) {
        ScreenMgr.moveThroughSequence(true);
        // If we happen to be in the middle of the current screen we  
        // move to the next screen to avoid confusion of the fields changing.
        // Do this after wrapWebAction so the newly displayed screen doesn't end
        // up having the ActivityIcon restored from the previous screen.
      }

    }

    void updateWSSettings() {
      updateSettingsForScreen(
          wtAppImpl->screens.weatherScreen, wtAppImpl->screens.weatherScreen, "/updateWSSettings");
    }

    void updateFSSettings() {
      updateSettingsForScreen(
          wtAppImpl->screens.forecastScreen, wtAppImpl->screens.forecastScreen, "/updateFSSettings");
    }

    void updateNPSSettings() {
      updateSettingsForScreen(
          mqApp->nextPrinterScreen, mqApp->nextPrinterScreen, "/updateNPSSettings");
    }

    void updateNSSettings() {
      updateSettingsForScreen(
          mqApp->newsScreen, mqApp->newsScreen, "/updateNSSettings");
    }

    void updateAPSSettings() {
      updateSettingsForScreen(
          mqApp->allPrinterScreen, mqApp->allPrinterScreen, "/updateAPSSettings");
    }

    void updateHSSettings() {
      updateSettingsForScreen(
          mqApp->homeScreen, mqApp->homeScreen, "/updateHSSettings");
    }

    // Handler for the "/updatePHConfig" endpoint. This is invoked as the target
    // of the form presented by "/displayMQConfig". It updates the values of the
    // corresponding settings and writes the settings to EEPROM.
    //
    // TO DO: Call a function to let the main app know that settings may have changed
    //        so that it can take any appropriate actions
    //
    // Form:
    //    GET /updatePHConfig?description=DESC&iBright=INT&...
    //
    void updateMQConfig() {
      auto action = []() {
        // General Settings
        mqSettings->scrollDelay = WebUI::arg("scrollDelay").toInt();
        
        // AIO Settings
        mqSettings->aio.key = WebUI::arg("aioKey");
        mqSettings->aio.username = WebUI::arg("aioUsername");
        mqSettings->aio.groupName = WebUI::arg("aioGroup");

        // Sensor Settings
        float tempCorrection = WebUI::arg("tempCorrection").toFloat();
        mqSettings->sensorSettings.tempCorrection = wtApp->settings->uiOptions.useMetric ?
            tempCorrection : Basics::delta_f_to_c(tempCorrection);
        mqSettings->sensorSettings.humiCorrection = WebUI::arg("humiCorrection").toFloat();
        mqApp->weatherMgr.setAttributes(
          mqSettings->sensorSettings.tempCorrection,
          mqSettings->sensorSettings.humiCorrection,
          WebThing::settings.elevation);

        mqSettings->write();

        ScrollScreen::setDefaultFrameDelay(mqSettings->scrollDelay);

        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateMQConfig", action, false);
    }

    void updatePrinterConfig() {
      bool printMonitoringAlreadyEnabled = mqSettings->printMonitorEnabled;

      auto action = [printMonitoringAlreadyEnabled]() {
        for (int i = 0; i < 4; i++) {
          PrinterSettings* printer = &(mqSettings->printer[i]);
          bool wasActive = printer->isActive;
          Internal::updateSinglePrinter(i);
          if (!wasActive && printer->isActive) mqApp->printerWasActivated(i);
        }
        mqSettings->printerRefreshInterval = WebUI::arg(F("refreshInterval")).toInt();
        mqSettings->printMonitorEnabled = WebUI::hasArg(F("pmEnabled"));

        if (WebThing::settings.showDevMenu) {
          PrinterSettings* printer = &(mqSettings->printer[0]);
          printer[0].mock = WebUI::hasArg(F("_p0_mock"));
          printer[1].mock = WebUI::hasArg(F("_p1_mock"));
          printer[2].mock = WebUI::hasArg(F("_p2_mock"));
          printer[3].mock = WebUI::hasArg(F("_p3_mock"));
        }

        // Act on changed settings...
        if (mqSettings->printMonitorEnabled && !printMonitoringAlreadyEnabled) {
          mqApp->fireUpPrintMonitor();
        }
        wtAppImpl->configMayHaveChanged();
        mqSettings->write();
        WebUI::redirectHome();
      };
  
      WebUI::wrapWebAction("/updatePrinterConfig", action);
    }
  }   // ----- END: MQWebUI::Endpoints


  void init() {
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

    WebUI::registerHandler("/",                       Pages::presentHomePage);
    WebUI::registerHandler("/presentMQConfig",        Pages::presentMQconfig);
    WebUI::registerHandler("/presentPrinterConfig",   Pages::presentPrinterConfig);
    WebUI::registerHandler("/presentMOTDPage",        Pages::presentMOTDPage);

    WebUI::registerHandler("/updateHSSettings",       Endpoints::updateHSSettings);
    WebUI::registerHandler("/updateWSSettings",       Endpoints::updateWSSettings);
    WebUI::registerHandler("/updateFSSettings",       Endpoints::updateFSSettings);
    WebUI::registerHandler("/updateNPSSettings",      Endpoints::updateNPSSettings);
    WebUI::registerHandler("/updateAPSSettings",      Endpoints::updateAPSSettings);
    WebUI::registerHandler("/updateNSSettings",       Endpoints::updateNSSettings);
    WebUI::registerHandler("/updatePrinterConfig",    Endpoints::updatePrinterConfig);
    WebUI::registerHandler("/updateMQConfig",         Endpoints::updateMQConfig);
    WebUI::registerHandler("/ackPrinterDone",         Endpoints::ackPrinterDone);

    WebUI::registerHandler("/resetScreen",            Endpoints::resetScreen);
  }

}
// ----- END: MQWebUI

/*
 * MQWebUI:
 *    Implements a simple WebUI that displays and updates settings
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WebThing.h>
#include <WebUI.h>
#include <WebUIHelper.h>
#include <gui/Display.h>
#include <screens/matrix/ScrollScreen.h>
//                                  Local Includes
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
      "<i class='fa fa-window-restore'></i> Configure Screens</a>"
      "<a class='w3-bar-item w3-button' href='/presentPrinterConfig'>"
      "<i class='fa fa-glass'></i> Configure Printers</a>"
      "<a class='w3-bar-item w3-button' href='/uploadPage?targetName=/motd.json'>"
      "<i class='fa fa-commenting'></i> Configure MOTD</a>");

    void updateSinglePrinter(int i) {
      PrinterSettings* printer = &(mqSettings->printer[i]);
      String prefix = "_p" + String(i) + "_";
      printer->isActive = WebUI::hasArg(prefix + "enabled");
      printer->apiKey = WebUI::arg(prefix + "key");
      printer->server = WebUI::arg(prefix + "server");
      printer->port = WebUI::arg(prefix + "port").toInt();
      printer->user =  WebUI::arg(prefix + "user");
      printer->pass =  WebUI::arg(prefix + "pass");
      printer->nickname =  WebUI::arg(prefix + "nick");
      printer->type =  WebUI::arg(prefix + "type");
    }
  } // ----- END: MQWebUI::Internal

  // ----- BEGIN: MQWebUI::Pages
  namespace Pages {
    void presentMOTDPage() {
      auto mapper =[](const String &key, String& val) -> void {
        if (key == "HEADER")          val = "Message of the Day";
        else if (key == "ADJ")        val = "MOTD";
      };

      WebUI::wrapWebPage("/presentMOTDPage", "/MOTDForm.html", mapper);
    }

    void presentMQconfig() {
      auto mapper =[&](const String &key, String& val) -> void {
        if (key == "SCROLL_DELAY")    val = mqSettings->scrollDelay;
        else if (key == "HS_TIME")    val = mqSettings->homeScreenTime;
        else if (key == "AIO_KEY")    val = mqSettings->aio.key;
        else if (key == "AIO_USER")   val = mqSettings->aio.username;
        else if (key == "AIO_GROUP")  val = mqSettings->aio.groupName;
        else if (key == "LAT")        val = WebThing::settings.latAsString();
        else if (key == "LNG")        val = WebThing::settings.lngAsString();
        else if (key == "GMAPS_KEY")  val = WebThing::settings.googleMapsKey;
        else if (key.equals(F("CITYID"))) {
          if (wtApp->settings->owmOptions.enabled) val = wtApp->settings->owmOptions.cityID;
          else val.concat("5380748");  // Palo Alto, CA, USA
        }
        else if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
        else if (key.equals(F("UNITS"))) val = wtApp->settings->uiOptions.useMetric ? "metric" : "imperial";
        else if (key == "VLTG") {
          float voltage = WebThing::measureVoltage();
          if (voltage == -1) val = "N/A";
          else val = (String(voltage, 2) + "V");
        }
      };

      WebUI::wrapWebPage("/presentMQconfig", "/ConfigForm.html", mapper);
    }

    void presentPrinterConfig() {
      String hspType = 
        mqSettings->homeScreenProgress == MQSettings::HSP_Horizontal ? "HSP_HRZ" :
        (mqSettings->homeScreenProgress == MQSettings::HSP_Vertical ? "HSP_VRT" : "HSP_NONE");

      auto mapper =[hspType](const String& key, String& val) -> void {
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
          else if (strcmp(subkey, "T_") == 0) {
            subkey = &subkey[2];
            if (strcmp(subkey, printer->type.c_str()) == 0) val = "selected";
          } 
        }
        else if (key.equals(hspType)) val = "selected";
        else if (key.equals("SP_PNAME")) val = WebUIHelper::checkedOrNot[mqSettings->singlePrinter.printerName];
        else if (key.equals("SP_FNAME")) val = WebUIHelper::checkedOrNot[mqSettings->singlePrinter.fileName];
        else if (key.equals("SP_PCT")) val = WebUIHelper::checkedOrNot[mqSettings->singlePrinter.pct];
        else if (key.equals("SP_CMPLT")) val = WebUIHelper::checkedOrNot[mqSettings->singlePrinter.completeAt];
        else if (key.equals("AP_PNAME")) val = WebUIHelper::checkedOrNot[mqSettings->allPrinters.printerName];
        else if (key.equals("AP_FNAME")) val = WebUIHelper::checkedOrNot[mqSettings->allPrinters.fileName];
        else if (key.equals("AP_PCT")) val = WebUIHelper::checkedOrNot[mqSettings->allPrinters.pct];
        else if (key.equals("AP_CMPLT")) val = WebUIHelper::checkedOrNot[mqSettings->allPrinters.completeAt];
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
        mqSettings->aio.key = WebUI::arg("aioKey");
        mqSettings->aio.username = WebUI::arg("aioUsername");
        mqSettings->aio.groupName = WebUI::arg("aioGroup");
        mqSettings->scrollDelay = WebUI::arg("scrollDelay").toInt();
        mqSettings->homeScreenTime = WebUI::arg("homeScreenTime").toInt();
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

        String hspType = WebUI::arg(F("hsp"));
        if (hspType.equalsIgnoreCase("hortizontal")) mqSettings->homeScreenProgress = MQSettings::HSP_Horizontal;
        else if (hspType.equalsIgnoreCase("vertical")) mqSettings->homeScreenProgress = MQSettings::HSP_Vertical;
        else mqSettings->homeScreenProgress = MQSettings::HSP_None;

        if (WebThing::settings.showDevMenu) {
          PrinterSettings* printer = &(mqSettings->printer[0]);
          printer[0].mock = WebUI::hasArg(F("_p0_mock"));
          printer[1].mock = WebUI::hasArg(F("_p1_mock"));
          printer[2].mock = WebUI::hasArg(F("_p2_mock"));
          printer[3].mock = WebUI::hasArg(F("_p3_mock"));
        }

        mqSettings->singlePrinter.printerName = WebUI::hasArg("spPrinterName");
        mqSettings->singlePrinter.fileName = WebUI::hasArg("spFileName");
        mqSettings->singlePrinter.pct = WebUI::hasArg("spPct");
        mqSettings->singlePrinter.completeAt = WebUI::hasArg("spCompleteAt");
        mqSettings->allPrinters.printerName = WebUI::hasArg("apPrinterName");
        mqSettings->allPrinters.fileName = WebUI::hasArg("apFileName");
        mqSettings->allPrinters.pct = WebUI::hasArg("apPct");
        mqSettings->allPrinters.completeAt = WebUI::hasArg("apCompleteAt");

        // Act on changed settings...
        if (mqSettings->printMonitorEnabled && !printMonitoringAlreadyEnabled) {
          mqApp->fireUpPrintMonitor();
        }
        wtAppImpl->configMayHaveChanged();
        wtApp->settings->write();
        WebUI::redirectHome();
      };
  
      WebUI::wrapWebAction("/updatePrinterConfig", action);
    }
  }   // ----- END: MQWebUI::Endpoints


  void init() {
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

    WebUI::registerHandler("/",                       WebUIHelper::Default::homePage);
    WebUI::registerHandler("/presentMQConfig",        Pages::presentMQconfig);
    WebUI::registerHandler("/presentPrinterConfig",   Pages::presentPrinterConfig);
    WebUI::registerHandler("/presentMOTDPage",        Pages::presentMOTDPage);

    WebUI::registerHandler("/updatePrinterConfig",    Endpoints::updatePrinterConfig);
    WebUI::registerHandler("/updateMQConfig",         Endpoints::updateMQConfig);
  }

}
// ----- END: MQWebUI

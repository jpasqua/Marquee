
/*
 * AllPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <BPA_PrinterGroup.h>
//                                  WebThingApp Includes
#include <gui/Display.h>
//                                  Local Includes
#include "AllPrinterScreen.h"
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

AllPrinterScreen::AllPrinterScreen() {
  settings.read();

  if (settings.fields.size() == 0) {
    settings.fields.emplace_back(APSSettings::Field("Printer Name", true));
    settings.fields.emplace_back(APSSettings::Field("File Name", true));
    settings.fields.emplace_back(APSSettings::Field("Percentage", true));
    settings.fields.emplace_back(APSSettings::Field("Completion Time", false));
    settings.write();
  }

  nLabels = 0;
  labels = NULL;

  init();
  _statusText.clear();
}

void AllPrinterScreen::innerActivation() {
  updateText();
}

void AllPrinterScreen::settingsHaveChanged() {
  settings.write();
}


// ----- Private Methods

void AllPrinterScreen::updateText() {
  _statusText = "";
  String printerName;
  int pctComplete;
  String completionTime;
  String fileName;
  uint32_t timeLeft;

  if (mqSettings->printMonitorEnabled) {
    for (int i = mqApp->printerGroup->numberOfPrinters()-1; i >= 0; i--) {
      if (mqSettings->printer[i].isActive) {
        PrintClient* p = mqApp->printerGroup->getPrinter(i);
        if (p->getState() >= PrintClient::State::Complete) {
          printerName = mqApp->printerGroup->getDisplayName(i);
          pctComplete = p->getPctComplete();

          timeLeft = p->getPrintTimeLeft();
          fileName = p->getFilename();
          int extIndex = fileName.lastIndexOf(".gcode");
          if (extIndex != -1) fileName.remove(extIndex);

          if (!_statusText.isEmpty()) _statusText += ", ";

          String delim = "";
          for (APSSettings::Field f : settings.fields) {
            if (!f.enabled) continue;
            _statusText += delim;
            delim = " ";
            String key = f.id;
            key.toLowerCase();
            if (key.startsWith("printer")) {
              _statusText += printerName;
              delim = ":";
            } else if (key.startsWith("file")) {
              _statusText += fileName;
            } else if (key.startsWith("percent")) {
              _statusText += pctComplete;
              _statusText += "%";
            } else if (key.startsWith("completion")) {
              if (timeLeft) {
                mqApp->printerGroup->completionTime(completionTime, timeLeft);
                _statusText += completionTime;
              }
            }
          }

          int len = _statusText.length();
          if (len && _statusText[len-1] == ' ') _statusText.remove(len-1, 1);
        }
      }
    }
  }
  setText(_statusText, Display.BuiltInFont_ID);
}

/*------------------------------------------------------------------------------
 *
 * APSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

APSSettings::APSSettings() {
  maxFileSize = 512;
  version = 1;
  init("/APSSettings.json");
}

void APSSettings::fromJSON(const JsonDocument &doc) {
  fields.clear();

  Field field;

  const JsonArrayConst& jsonFields = doc["fields"].as<JsonArray>();
  for (const auto& jsonField : jsonFields) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     fields.push_back(field);
  }
}

void APSSettings::toJSON(JsonDocument &doc) {
  JsonArray jsonFields = doc.createNestedArray("fields");
  for (Field& field : fields) {
    JsonObject jsonField = jsonFields.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }
}

void APSSettings::logSettings() {
  Log.verbose(F("AllPrinterScreen Settings"));
  Log.verbose(F("  Fields"));
  for (const Field& field : fields) {
    Log.verbose(F("    %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
}

#endif







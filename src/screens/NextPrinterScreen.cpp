
/*
 * NextPrinterScreen:
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
#include "NextPrinterScreen.h"
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

NextPrinterScreen::NextPrinterScreen() {
  settings.read();

  if (settings.fields.size() == 0) {
    settings.fields.emplace_back(NPSSettings::Field("Printer Name", true));
    settings.fields.emplace_back(NPSSettings::Field("File Name", true));
    settings.fields.emplace_back(NPSSettings::Field("Percentage", true));
    settings.fields.emplace_back(NPSSettings::Field("Completion Time", false));
    settings.write();
  }

  nLabels = 0;
  labels = NULL;

  init();
  _statusText.clear();
}

void NextPrinterScreen::innerActivation() {
  updateText();
}

void NextPrinterScreen::settingsHaveChanged() {
  settings.write();
}


// ----- Private Methods

void NextPrinterScreen::updateText() {
  _statusText = "";
  if (mqSettings->printMonitorEnabled) {
    uint8_t whichPrinter;
    String completionTime;
    uint32_t timeRemaining;

    if (mqApp->printerGroup->nextCompletion(whichPrinter, completionTime, timeRemaining)) {
      PrintClient* p = mqApp->printerGroup->getPrinter(whichPrinter);
      String printerName = mqApp->printerGroup->getDisplayName(whichPrinter);
      int pctComplete = p->getPctComplete();
      String fileName = p->getFilename();

      // Remove file extension
      int extIndex = fileName.lastIndexOf(".gcode");
      if (extIndex != -1) fileName.remove(extIndex);

      _statusText = "";
      String delim = "";
      for (NPSSettings::Field f : settings.fields) {
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
          _statusText += completionTime;
        }
      }

      int len = _statusText.length();
      if (len && _statusText[len-1] == ' ') _statusText.remove(len-1, 1);
    }
  }
  setText(_statusText, Display.BuiltInFont_ID);
}


/*------------------------------------------------------------------------------
 *
 * NPSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

NPSSettings::NPSSettings() {
  maxFileSize = 512;
  version = 1;
  init("/NPSSettings.json");
}

void NPSSettings::fromJSON(const JsonDocument &doc) {
  fields.clear();

  Field field;

  const JsonArrayConst& jsonFields = doc["fields"].as<JsonArray>();
  for (const auto& jsonField : jsonFields) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     fields.push_back(field);
  }
}

void NPSSettings::toJSON(JsonDocument &doc) {
  JsonArray jsonFields = doc.createNestedArray("fields");
  for (Field& field : fields) {
    JsonObject jsonField = jsonFields.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }
}

void NPSSettings::logSettings() {
  Log.verbose(F("AllPrinterScreen Settings"));
  Log.verbose(F("  Fields"));
  for (const Field& field : fields) {
    Log.verbose(F("    %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
}
#endif







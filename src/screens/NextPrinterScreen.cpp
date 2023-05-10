
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
  nLabels = 0;
  labels = NULL;

  init();
  _statusText.clear();
}

void NextPrinterScreen::innerActivation() {
  updateText();
}


// ----- Private Methods

void NextPrinterScreen::updateText() {
  _statusText = "";
  if (mqSettings->printMonitorEnabled) {
    uint8_t whichPrinter;
    String completionTime;
    uint32_t timeRemaining;

    String printerName;
    String fileName;
    int pctComplete;

    if (mqApp->printerGroup->nextCompletion(whichPrinter, completionTime, timeRemaining)) {
      PrintClient* p = mqApp->printerGroup->getPrinter(whichPrinter);
      printerName = mqApp->printerGroup->getDisplayName(whichPrinter);
      pctComplete = p->getPctComplete();
      fileName = p->getFilename();
      int extIndex = fileName.lastIndexOf(".gcode");
      if (extIndex != -1) fileName.remove(extIndex);

      _statusText = "";
      if (mqSettings->singlePrinter.printerName) {
        _statusText += printerName;
        _statusText += ": ";
      }
      if (mqSettings->singlePrinter.pct) {
        _statusText += pctComplete;
        _statusText += "% ";
      }
      if (mqSettings->singlePrinter.fileName) {
        _statusText += fileName;
        _statusText += " ";
      }
      if (mqSettings->singlePrinter.completeAt) {
        _statusText += completionTime;
      }
      int len = _statusText.length();
      if (len && _statusText[len-1] == ' ') _statusText.remove(len-1, 1);
    }
  }
  setText(_statusText, Display.BuiltInFont_ID);
}
#endif







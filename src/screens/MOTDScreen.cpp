
/*
 * MOTDScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ESP_FS.h>
#include <GenericESP.h>
//                                  WebThingApp Includes
#include <gui/Display.h>
//                                  Local Includes
#include "MOTDScreen.h"
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

MOTDScreen::MOTDScreen() {
  nLabels = 0;
  labels = NULL;

  firstActivation = true;
  init();
}

void MOTDScreen::innerActivation() {
  if (firstActivation) {
    firstActivation = false;
    readMessages("/motd.json");
  }
  updateText();
}

//
// ----- MOTDScreen Private Functions
//

char *makeAHole() {
  // The following call to malloc is actually there to *reduce* fragmentation! What happens normally
  // is we allocate a huge chunk for the the JSON document, read it in, and then allocate the various
  // FlexScreen objects. They are allocated "later" on the heap than the JSON document, so when we free
  // it, there is a big hole left over (hence the fragmentation). By allocating the placeholder first,
  // then allocating the JSON doc, then freeing the placeholder, we leave space "before" the JSON doc
  // for the FlexScreen items to consume. Then when the JSON doc is freed, it doesnt leave a hole since
  // it is freed from the end of the heap.
  constexpr uint16_t ReserveSize = 2000;
  constexpr uint16_t PlaceHolderSize = 2000;
  char *placeHolder = nullptr;

  if (GenericESP::getMaxFreeBlockSize() > PlaceHolderSize + ReserveSize) {
    placeHolder = (char*)malloc(ReserveSize);
    placeHolder[1] = 'C';   // Touch the memory or the compiler may optimize away the malloc
  }
  return placeHolder;
}

void MOTDScreen::updateText() {
  time_t timeNow = now();
  int theDay = day(timeNow);
  int theMonth = month(timeNow);
  int theHour = hour(timeNow);

  // Check whether we're on a special day
  for (auto d: msgs.dayMsgs) {
    if (d.month == theMonth && d.day == theDay) {
      // Pick a message at random
      int i = random(0, d.msgs.size());
      setText(d.msgs[i], Display.BuiltInFont_ID);
      return;
    }
  }

  // Find a matching time interval
  for (auto t: msgs.timeMsgs) {
    if (theHour >= t.startHour && theHour < t.endHour) {
      // Pick a message at random
      int i = random(0, t.msgs.size());
      setText(t.msgs[i], Display.BuiltInFont_ID);
      return;
    }
  }

  setText("Have a nice day!", Display.BuiltInFont_ID);
}

void MOTDScreen::readMessages(String filePath) {
  File motdFile = ESP_FS::open(filePath, "r");
  if (!motdFile) {
    Log.warning("No messages file was found: %s", filePath.c_str());
    return;
  }

  char* hole = makeAHole();
  DynamicJsonDocument doc(MaxDocSize);
  if (hole) free(hole);

  auto error = deserializeJson(doc, motdFile);
  if (error) {
    Log.warning(F("Error parsing messages: %s"), error.c_str());
    return;
  }

  msgs.fromJSON(doc);
  msgs.toLog();
  Log.verbose("Successfully read message file: %s", filePath.c_str());
}

/*------------------------------------------------------------------------------
 *
 * Implementation of the Messages Class
 *
 *----------------------------------------------------------------------------*/

void Messages::fromJSON(DynamicJsonDocument& doc) {
  int currentYear = 2023; // year();

  for (JsonObject day : doc["days"].as<JsonArray>()) {
    for (JsonArray when : day["when"].as<JsonArray>()) {
      int theYear = when[0];

      if (theYear == 0 || theYear == currentYear) {
        DayMessages d;
        d.month = when[1];
        d.day = when[2];
        for (JsonVariant m : day["msgs"].as<JsonArray>()) {
          const char* msg = m;
          d.msgs.push_back(msg);
        }
        dayMsgs.push_back(d);
        break;
      }
    }
  }

  for (JsonObject time : doc["times"].as<JsonArray>()) {
    TimeMessages t;

    t.startHour = time["start"];
    t.endHour = time["end"];
    for (JsonVariant m : time["msgs"].as<JsonArray>()) {
      const char* msg = m.as<char*>();
      t.msgs.push_back(msg);
    }
    timeMsgs.push_back(t);
  }
}

void Messages::toLog() {
  for (DayMessages d: dayMsgs) {
    Log.verbose("Messages for %d/%d", d.month, d.day);
    for (String m: d.msgs) {
      Log.verbose("    %s", m.c_str());
    }
  }
  for (TimeMessages t: timeMsgs) {
    Log.verbose("Messages for (%d, %d)", t.startHour, t.endHour);
    for (String m: t.msgs) {
      Log.verbose("    %s", m.c_str());
    }
  }
}






#endif







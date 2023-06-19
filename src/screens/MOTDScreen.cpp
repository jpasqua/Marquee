
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

  init();
}

void MOTDScreen::innerActivation() {
  if (!msgsRead) {
    msgs.begin();
    msgsRead = true;
  }
  updateText();
}

//
// ----- MOTDScreen Private Functions
//

void MOTDScreen::updateText() {
  time_t timeNow = now();
  int theDay = day(timeNow);
  int theMonth = month(timeNow);
  int theHour = hour(timeNow);
Log.verbose("special day: month: %d, day %d", theMonth, theDay);

  counter++;

  // Check whether we're on a special day
  for (auto d: msgs.dayMsgs) {
    if (d.month == theMonth && d.day == theDay) {
Log.verbose("special day: month: %d, day %d", d.month, d.day);
      setText(d.msgs[random(0, d.msgs.size())], Display.BuiltInFont_ID);
      return;
    }
  }

  if (counter > msgs.dotwCount) {
    counter = 0;
    std::vector<const char*>& v = msgs.daysOfTheWeek[weekday()-1];
    setText(v[random(0, v.size())], Display.BuiltInFont_ID);
    return;
  }

  for (auto t: msgs.timeMsgs) {
    if (theHour >= t.startHour && theHour < t.endHour) {
      setText(t.msgs[random(0, t.msgs.size())], Display.BuiltInFont_ID);
      return;
    }
  }

  setText("Have a nice day!", Display.BuiltInFont_ID);
}

/*------------------------------------------------------------------------------
 *
 * Implementation of the Messages Class
 *
 *----------------------------------------------------------------------------*/

Messages::Messages() { }

void Messages::begin() {
  maxFileSize = 8192;
  version = 1;
  init("/motd.json");

Log.verbose(F("\nBefore MOTD: Heap: free=%d, frag=%d%%"), ESP.getFreeHeap(), GenericESP::getHeapFragmentation());
  read();
Log.verbose(F("After MOTD: Heap: free=%d, frag=%d%%\n"), ESP.getFreeHeap(), GenericESP::getHeapFragmentation());
}

void Messages::fromJSON(const JsonDocument& doc) {
  int currentYear = year();
  int totalMsgBytes = 0;
Log.verbose("currentYear: %d", currentYear);

  for (JsonObjectConst day : doc["days"].as<JsonArray>()) {
    for (JsonArrayConst when : day["when"].as<JsonArray>()) {
      int theYear = when[0];
Log.verbose("theYear: %d", theYear);
      if (theYear == 0 || theYear == currentYear) {
        DayMessages d;
        d.month = when[1];
        d.day = when[2];
        for (JsonVariantConst m : day["msgs"].as<JsonArray>()) {
Log.verbose("Entering loop on day[msgs], day = %d", d.day);
          const char* msgRef = m;
          d.msgs.push_back(strdup(msgRef));
          totalMsgBytes += (strlen(msgRef)+1);
if (theYear == 2023) Log.verbose("Adding %s", msgRef);
        }
        dayMsgs.push_back(d);
        break;
      }
    }
  }

  for (JsonObjectConst time : doc["times"].as<JsonArrayConst>()) {
    TimeMessages t;

    t.startHour = time["start"];
    t.endHour = time["end"];
    for (JsonVariantConst m : time["msgs"].as<JsonArrayConst>()) {
      const char* msgRef = m.as<char*>();
      t.msgs.push_back(strdup(msgRef));
      totalMsgBytes += (strlen(msgRef)+1);
    }
    timeMsgs.push_back(t);
  }

  int dayIndex = 0;
  JsonArrayConst dotw = doc["dotw"];
  for (JsonArrayConst messages : dotw) {
    for (const char* msgRef : messages) {
      daysOfTheWeek[dayIndex].push_back(strdup(msgRef));
      totalMsgBytes += (strlen(msgRef)+1);
    }
    dayIndex++;
  }
logSettings();
  dotwCount = doc["dotwCount"];
}

void Messages::toJSON(JsonDocument&) {
  // TO DO: Implement if ever needed. Right now we just upload entire MOTD.json files
}

void Messages::logSettings() {
  for (DayMessages d: dayMsgs) {
    Log.verbose("Messages for %d/%d", d.month, d.day);
    for (const char* m: d.msgs) {
      Log.verbose("    %s", m);
    }
  }
  for (TimeMessages t: timeMsgs) {
    Log.verbose("Messages for (%d, %d)", t.startHour, t.endHour);
    for (const char* m: t.msgs) {
      Log.verbose("    %s", m);
    }
  }
  for (int i = 0; i < 7; i++) {
    Log.verbose("%s", dayStr(i+1));
    for (const char* m : daysOfTheWeek[i]) {
      Log.verbose("  %s", m);
    }
  }
}

#endif







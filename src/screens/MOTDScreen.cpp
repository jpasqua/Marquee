
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
  msgs.read();

  nLabels = 0;
  labels = NULL;

  init();
}

void MOTDScreen::innerActivation() {
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

  counter++;

  // Check whether we're on a special day
  for (auto d: msgs.dayMsgs) {
    if (d.month == theMonth && d.day == theDay) {
      setText(d.msgs[random(0, d.msgs.size())], Display.BuiltInFont_ID);
Log.verbose("Using special day messages");
      return;
    }
  }

  if (counter > msgs.dotwCount) {
    counter = 0;
    std::vector<String>& v = msgs.daysOfTheWeek[weekday()-1];
    setText(v[random(0, v.size())], Display.BuiltInFont_ID);
Log.verbose("Using dotw messages");
    return;
  }

  for (auto t: msgs.timeMsgs) {
    if (theHour >= t.startHour && theHour < t.endHour) {
      setText(t.msgs[random(0, t.msgs.size())], Display.BuiltInFont_ID);
Log.verbose("Using time interval messages");
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

Messages::Messages() {
  maxFileSize = 8192;
  version = 1;
  init("/motd.json");
}


void Messages::fromJSON(const JsonDocument& doc) {
  int currentYear = year();

  for (JsonObjectConst day : doc["days"].as<JsonArray>()) {
    for (JsonArrayConst when : day["when"].as<JsonArray>()) {
      int theYear = when[0];

      if (theYear == 0 || theYear == currentYear) {
        DayMessages d;
        d.month = when[1];
        d.day = when[2];
        for (JsonVariantConst m : day["msgs"].as<JsonArray>()) {
          const char* msg = m;
          d.msgs.push_back(msg);
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
      const char* msg = m.as<char*>();
      t.msgs.push_back(msg);
    }
    timeMsgs.push_back(t);
  }

  int dayIndex = 0;
  JsonArrayConst dotw = doc["dotw"];
  for (JsonArrayConst messages : dotw) {
    for (const char* message : messages) {
      daysOfTheWeek[dayIndex].push_back(message);
    }
    dayIndex++;
  }

  dotwCount = doc["dotwCount"];
}

void Messages::toJSON(JsonDocument&) {
  // TO DO: Implement if ever needed. Right now we just upload entire MOTD.json files
}

void Messages::logSettings() {
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
  for (int i = 0; i < 7; i++) {
    Log.verbose("%s", dayStr(i+1));
    for (String m : daysOfTheWeek[i]) {
      Log.verbose("  %s", m.c_str());
    }
  }
}






#endif







#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
#include <TimeLib.h>
//                                  Local Includes
#include "DateScreen.h"
#include "../../MarqueeApp.h"
//--------------- End:    Includes ---------------------------------------------


DateScreen::DateScreen() {
  init();
}

void DateScreen::innerActivation() {
  String info;

  info += dayStr(weekday());
  info += ", ";
  info += monthStr(month());
  info += ' ';
  info += day() ;
  info += dateOrdinal(day()) ;

  setText(info);
}


String DateScreen::dateOrdinal(int dom) {
    if (dom == 31 || dom == 21 || dom == 1) return "st";
    else if (dom == 22 || dom == 2) return "nd";
    else if (dom == 23 || dom == 3) return "rd";
    else return "th";
}

#endif
/*
 * MQDataSupplier:
 *    Supplies app-specific data to the WebThing DataBroker
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <DataBroker.h>
//                                  Local Includes
#include "MarqueeApp.h"
#include "MQDataSupplier.h"
//--------------- End:    Includes ---------------------------------------------


namespace MQDataSupplier {

  // CUSTOM: If your app has a custom data source, publish that data to
  // plugins by implementing a data supplier that maps keys to values.
  // In this case we publish the data from the RateClient
  void dataSupplier(const String& key, String& val) {
    // Key of the form: "aqiDesc_$E.AW.0.aqi"
    //                   0123456789012345678
    if (key.startsWith("aqiDesc_")) {
      String whichAQI = key.substring(8);
      String baseVal;
      DataBroker::map(whichAQI, val);
      if (!val.isEmpty()) {
        int aqi = val.toInt();
        if (aqi < 51) val = "Good";
        else if (aqi < 101) val = "Moderate";
        else if (aqi < 151) val = "Poor";
        else if (aqi < 201) val = "Unhealthy";
        else if (aqi < 301) val = "Harmful";
        else val = "Hazardous";
      } else val = "--";
    }
  }

}


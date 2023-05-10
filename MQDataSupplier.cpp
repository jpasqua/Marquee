/*
 * MQDataSupplier:
 *    Supplies app-specific data to the WebThing DataBroker
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  Local Includes
#include "MarqueeApp.h"
#include "MQDataSupplier.h"
//--------------- End:    Includes ---------------------------------------------


namespace MQDataSupplier {

  // CUSTOM: If your app has a custom data source, publish that data to
  // plugins by implementing a data supplier that maps keys to values.
  // In this case we publish the data from the RateClient
  void dataSupplier(const String&, String&) {

  }

}


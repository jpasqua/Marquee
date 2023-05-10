/*
 * MQDataSupplier:
 *    Supplies app-specific data to the WebThing DataBroker
 *
 * NOTES:
 * o Uses 'R' as the prefix for data 
 * o Currently there is no app-specific data supplied to the Broker
 *
 */

#ifndef MQDataSupplier_h
#define MQDataSupplier_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


namespace MQDataSupplier {
  // CUSTOM: Choose a single character prefix to use for this data source
  constexpr char ERPrefix = 'R';
  
  extern void dataSupplier(const String& key, String& value);
}

#endif  // MQDataSupplier_h
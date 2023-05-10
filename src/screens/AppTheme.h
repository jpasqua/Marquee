#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MATRIX

/*
 * AppTheme
 *     Define theme elements that are specific to this app
 *
 * NOTES:
 * o Theme.h from the gui package of WebThingApp defines many common colors
 *   and fonts that are used by the implementation of Screen classes.
 * o An app-specific screen may want to use thematic elements that are
 *   not generic. Those can be defined here in the AppTheme namespace.
 *
 */

#ifndef AppTheme_h
#define AppTheme_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <TFT_eSPI.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

namespace AppTheme {
  // CUSTOM: Define any theme color or fonts that are used throught the app
};

#endif	// AppTheme_h
#endif
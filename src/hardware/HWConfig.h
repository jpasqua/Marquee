#ifndef HWConfig_h
#define HWConfig_h

#include <BPABasics.h>
#include <GenericESP.h>
#include <gui/devices/DeviceSelect.h>

//
// A Hardware configuration is a bundle of definitions that describe a
// particular collection of hardware such as what kind of display is attached,
// which pins are being used as buttons, which pins to use for standard
// functions (e.g. SCL and SDA). Most of the definitions are runtime constants,
// but some may be compile-time constants (#define's)
//
// This file contains a number of predefined configurations. You can:
// a) choose from one of the predefined configurations
// b) add a new configuration if you think it is common
// c) modify the Custom config to match your unique configuration
//    your hardware. 
//
// This file is specific to hard3are configurations which contain a
// DEVICE_TYPE_OLED display device. It can be used as a basis for
// HWConfig files with other types of display, or no display
//

// SECTION 3: [BOILERPLATE] Define a list of the predefined hardware configs
// that we can choose from. If you add a new configuration, list it here.
#define Config_D1Mini        1
#define Config_ESP32Mini     2

// SECTION 4: [CUSTOMIZE] Choose a specific configuration
#define SelectedConfig Config_D1Mini

// SECTION 5: The definitions of the available configurations
// Add new configs below if you add an option
#if (SelectedConfig == Config_D1Mini)
  /*------------------------------------------------------------------------------
   *
   * Config Info for D1Mini
   *
   *----------------------------------------------------------------------------*/
  #define PROCESSOR_ASSERT PROCESSOR_ESP8266

  // ----- Matrix Settings
  constexpr Basics::Pin Wiring_MOSI = D7;  // DIN
  constexpr Basics::Pin Wiring_CLK  = D5;
  constexpr Basics::Pin Wiring_CS   = D6;
  constexpr uint8_t hDisplays = 8;
  constexpr uint8_t vDisplays = 1;

  // ----- Buttons
  // constexpr Basics::Pin physicalButtons[] =  { D3 }; // This is the "Flash" button
  constexpr Basics::Pin physicalButtons[] =  { Basics::UnusedPin };
  constexpr Basics::Pin syntheticGrounds[] = { Basics::UnusedPin };

#elif (SelectedConfig == Config_ESP32Mini)
  /*------------------------------------------------------------------------------
   *
   * Config Info for ESP32D1Mini
   *
   *----------------------------------------------------------------------------*/
  #define PROCESSOR_ASSERT PROCESSOR_ESP32

  // ----- Matrix Settings
  constexpr Basics::Pin Wiring_MOSI = 23;  // DIN
  constexpr Basics::Pin Wiring_CLK  = 18;
  constexpr Basics::Pin Wiring_CS   = 5;
  constexpr uint8_t hDisplays = 8;
  constexpr uint8_t vDisplays = 1;

  // ----- Buttons
  constexpr Basics::Pin physicalButtons[] = { 13 };
  constexpr Basics::Pin syntheticGrounds[] = { Basics::UnusedPin };
#else
    #error "Please set SelectedConfig"
#endif


class HWConfig {
public:
  struct CorePins {
    Basics::Pin Wiring_SCL;
    Basics::Pin Wiring_SDA;
    Basics::Pin Wiring_MOSI;
    Basics::Pin Wiring_MISO;
    Basics::Pin Wiring_CLK;
  };

  const CorePins corePins;
  const DisplayDeviceOptions displayDeviceOptions;
  const Basics::Pin* physicalButtons;
  const uint8_t  nPhysicalButtons;
  const Basics::Pin* syntheticGrounds;
  const uint8_t  nSyntheticGrounds;

  const Basics::Pin advanceButton;
  const Basics::Pin previousButton;
};

constexpr HWConfig hwConfig {
  { Basics::UnusedPin, Basics::UnusedPin, Wiring_MOSI, Basics::UnusedPin, Wiring_CLK },
  { Wiring_CS, Wiring_MOSI, Wiring_CLK, hDisplays, vDisplays },
  physicalButtons, ARRAY_SIZE(physicalButtons),  
  syntheticGrounds, ARRAY_SIZE(syntheticGrounds),
  physicalButtons[0], Basics::UnusedPin
};


/*------------------------------------------------------------------------------
 *
 * Sanity Checks
 *
 *----------------------------------------------------------------------------*/

#if DEVICE_TYPE != DEVICE_TYPE_MTX
  #error ERROR: DEVICE_TYPE must be DEVICE_TYPE_MTX
#endif

#if defined(PROCESSOR_ASSERT)
  #if defined(ESP8266)
    #if PROCESSOR_ASSERT != PROCESSOR_ESP8266
      #error Processor set in IDE is ESP8266, expexted something else
    #endif
  #elif defined(ESP32)
    #if PROCESSOR_ASSERT != PROCESSOR_ESP32
      #error Processor set in IDE is ESP32, expexted something else
    #endif
  #endif
#endif

#endif  // HWConfig_h

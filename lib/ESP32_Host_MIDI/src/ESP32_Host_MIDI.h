#ifndef ESP32_HOST_MIDI_H
#define ESP32_HOST_MIDI_H

// --- Feature detection macros ---
// These macros determine which transport layers are available at compile time
// based on the ESP32 target and SDK configuration.

// USB Host requires USB-OTG hardware.
//   ESP32-S2 / ESP32-S3 : Full-Speed (12 Mbps)
//   ESP32-P4            : High-Speed (480 Mbps) — multiple devices via hub
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3) || \
    defined(CONFIG_IDF_TARGET_ESP32P4)
  #define ESP32_HOST_MIDI_HAS_USB 1
#else
  #define ESP32_HOST_MIDI_HAS_USB 0
#endif

// BLE requires Bluetooth support (ESP32, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2).
// ESP32-S2 and ESP32-P4 do NOT have native Bluetooth.
#if defined(CONFIG_BT_ENABLED)
  #define ESP32_HOST_MIDI_HAS_BLE 1
#else
  #define ESP32_HOST_MIDI_HAS_BLE 0
#endif

// PSRAM support (for history buffer)
#if defined(CONFIG_SPIRAM) || defined(CONFIG_SPIRAM_SUPPORT)
  #define ESP32_HOST_MIDI_HAS_PSRAM 1
#else
  #define ESP32_HOST_MIDI_HAS_PSRAM 0
#endif

// Native Ethernet MAC — ESP32-P4 only (requires an external PHY, e.g. LAN8720).
// Other chips can use a SPI Ethernet module (W5500) without this macro.
#if defined(CONFIG_IDF_TARGET_ESP32P4)
  #define ESP32_HOST_MIDI_HAS_ETH_MAC 1
#else
  #define ESP32_HOST_MIDI_HAS_ETH_MAC 0
#endif

// --- Includes ---

#include "MIDITransport.h"

#if ESP32_HOST_MIDI_HAS_USB
  #include "USBConnection.h"
#endif

#if ESP32_HOST_MIDI_HAS_BLE
  #include "BLEConnection.h"
#endif

#include "MIDIHandlerConfig.h"
#include "MIDIHandler.h"

#endif  // ESP32_HOST_MIDI_H

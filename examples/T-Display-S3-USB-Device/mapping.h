#ifndef MAPPING_H
#define MAPPING_H

// Pin mapping for T-Display S3

// USB Host
#define USB_DP_PIN   19
#define USB_DN_PIN   18

// ST7789 Display
#define TFT_CS_PIN    6
#define TFT_DC_PIN    16
#define TFT_RST_PIN   5
#define TFT_BL_PIN    38

// Board hardware
#define PIN_POWER_ON 15
#define PIN_BUTTON_1  0
#define PIN_BUTTON_2 14

// ---- USB MIDI device name ---------------------------------------------
// This is the name shown in macOS "Audio MIDI Setup", Windows Device Manager,
// and every DAW's MIDI port list. Max 32 characters.
#define USB_MIDI_DEVICE_NAME  "ESP32 MIDI"

#endif // MAPPING_H

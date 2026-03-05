// Hardware Mapping
//
// SPDX-License-Identifier: MIT
//
// Pin assignments and hardware constants for:
// LilyGo T-Display-S3 + MIDI Shield V1.1 (PCM5102 DAC)
//
// Edit this file to adapt the sketch to a different board or wiring.

#ifndef MAPPING_H
#define MAPPING_H

// ---------------------------------------------------------------------------
// Buttons
// ---------------------------------------------------------------------------
#define BTN_LEFT   0   // BOOT button — switch page
#define BTN_RIGHT  14  // KEY  button — cycle items

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------
#define TFT_BL_PIN  38  // TFT backlight
#define PWR_EN_PIN  15  // battery power enable

#define SCR_W 320
#define SCR_H 170

// ---------------------------------------------------------------------------
// I2S Audio (PCM5102 DAC — 3-wire mode, MCLK not used)
// ---------------------------------------------------------------------------
#define I2S_BCK         11  // bit clock   (via R15 470ohm → PCM5102 pin 13)
#define I2S_LRCK        13  // word select (via R13 470ohm → PCM5102 pin 15)
#define I2S_DOUT        12  // data out    (via R14 470ohm → PCM5102 pin 14)
#define SAMPLE_RATE     44100
#define I2S_BUFFER_SIZE 256          // frames per DMA buffer

#endif // MAPPING_H

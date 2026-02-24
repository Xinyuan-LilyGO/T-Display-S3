// Gingoduino — Music Theory Library for Embedded Systems
// Platform detection and feature tier configuration.
//
// SPDX-License-Identifier: MIT

#ifndef GINGODUINO_CONFIG_H
#define GINGODUINO_CONFIG_H

// ---------------------------------------------------------------------------
// Allow user override BEFORE including this header:
//   #define GINGODUINO_TIER 1
//   #include <Gingoduino.h>
// ---------------------------------------------------------------------------

#ifndef GINGODUINO_TIER

#if defined(__AVR__)
  #define GINGODUINO_TIER 1
#elif defined(ESP8266)
  #define GINGODUINO_TIER 2
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(TEENSYDUINO) || defined(ARDUINO_DAISY_SEED)
  #define GINGODUINO_TIER 3
#else
  #define GINGODUINO_TIER 2
#endif

#endif // GINGODUINO_TIER

// ---------------------------------------------------------------------------
// Feature flags derived from tier
// ---------------------------------------------------------------------------

// Tier 1: Note, Interval, Chord
#define GINGODUINO_HAS_NOTE      1
#define GINGODUINO_HAS_INTERVAL  1
#define GINGODUINO_HAS_CHORD     1

// Tier 2: + Scale, Field, Duration, Tempo, TimeSignature, Fretboard
#if GINGODUINO_TIER >= 2
  #define GINGODUINO_HAS_SCALE         1
  #define GINGODUINO_HAS_FIELD         1
  #define GINGODUINO_HAS_DURATION      1
  #define GINGODUINO_HAS_TEMPO         1
  #define GINGODUINO_HAS_TIMESIG       1
  #define GINGODUINO_HAS_FRETBOARD     1
#else
  #define GINGODUINO_HAS_SCALE         0
  #define GINGODUINO_HAS_FIELD         0
  #define GINGODUINO_HAS_DURATION      0
  #define GINGODUINO_HAS_TEMPO         0
  #define GINGODUINO_HAS_TIMESIG       0
  #define GINGODUINO_HAS_FRETBOARD     0
#endif

// Tier 3: + Sequence, Events, Tree, Progression, Comparisons
#if GINGODUINO_TIER >= 3
  #define GINGODUINO_HAS_SEQUENCE      1
  #define GINGODUINO_HAS_EVENT         1
  #define GINGODUINO_HAS_TREE          1
  #define GINGODUINO_HAS_PROGRESSION   1
  #define GINGODUINO_HAS_COMPARISON    1
#else
  #define GINGODUINO_HAS_SEQUENCE      0
  #define GINGODUINO_HAS_EVENT         0
  #define GINGODUINO_HAS_TREE          0
  #define GINGODUINO_HAS_PROGRESSION   0
  #define GINGODUINO_HAS_COMPARISON    0
#endif

// ---------------------------------------------------------------------------
// PROGMEM portability
// ---------------------------------------------------------------------------

#ifdef ARDUINO
  #include <Arduino.h>
#else
  // For native testing without Arduino framework
  #include <stdint.h>
  #include <string.h>
  #include <math.h>
  #ifndef PROGMEM
    #define PROGMEM
  #endif
  #ifndef pgm_read_byte
    #define pgm_read_byte(addr)   (*(const uint8_t*)(addr))
  #endif
  #ifndef pgm_read_word
    #define pgm_read_word(addr)   (*(const uint16_t*)(addr))
  #endif
  #ifndef pgm_read_dword
    #define pgm_read_dword(addr)  (*(const uint32_t*)(addr))
  #endif
  #ifndef pgm_read_float
    #define pgm_read_float(addr)  (*(const float*)(addr))
  #endif
  #ifndef pgm_read_ptr
    #define pgm_read_ptr(addr)    (*(const void* const*)(addr))
  #endif
  #ifndef strcmp_P
    #define strcmp_P(a, b) strcmp((a), (b))
  #endif
  #ifndef strcpy_P
    #define strcpy_P(a, b) strcpy((a), (b))
  #endif
  #ifndef strlen_P
    #define strlen_P(s) strlen(s)
  #endif
  #ifndef memcpy_P
    #define memcpy_P(d, s, n) memcpy((d), (s), (n))
  #endif
#endif

// ---------------------------------------------------------------------------
// Configurable limits
// ---------------------------------------------------------------------------

#ifndef GINGODUINO_MAX_CHORD_NOTES
  #define GINGODUINO_MAX_CHORD_NOTES  7
#endif

#ifndef GINGODUINO_MAX_SCALE_NOTES
  #define GINGODUINO_MAX_SCALE_NOTES  12
#endif

#if GINGODUINO_HAS_SEQUENCE
  #ifndef GINGODUINO_MAX_EVENTS
    #define GINGODUINO_MAX_EVENTS  64
  #endif
#endif

#if GINGODUINO_HAS_FRETBOARD
  #ifndef GINGODUINO_MAX_STRINGS
    #define GINGODUINO_MAX_STRINGS         6
  #endif
  #ifndef GINGODUINO_MAX_FRET_POSITIONS
    #define GINGODUINO_MAX_FRET_POSITIONS  48
  #endif
  #ifndef GINGODUINO_MAX_FINGERINGS
    #define GINGODUINO_MAX_FINGERINGS      5
  #endif
#endif

#endif // GINGODUINO_CONFIG_H

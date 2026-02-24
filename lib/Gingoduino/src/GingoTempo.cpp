// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoTempo.
//
// SPDX-License-Identifier: MIT

#include "GingoTempo.h"

#if GINGODUINO_HAS_TEMPO

#include "GingoDuration.h"
#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GingoTempo::GingoTempo(float bpm) : bpm_(bpm) {
    if (bpm_ <= 0.0f) bpm_ = 120.0f;
}

GingoTempo::GingoTempo(const char* marking) : bpm_(120.0f) {
    float val = markingToBpm(marking);
    if (val > 0.0f) bpm_ = val;
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

const char* GingoTempo::marking(char* buf, uint8_t maxLen) const {
    bpmToMarking(bpm_, buf, maxLen);
    return buf;
}

float GingoTempo::seconds(const GingoDuration& duration) const {
    // Quarter note = 1 beat → Duration.beats() returns beats relative to quarter
    // seconds = beats * (60 / BPM)
    float beats = duration.beats();
    return beats * (60.0f / bpm_);
}

// ---------------------------------------------------------------------------
// Static utilities
// ---------------------------------------------------------------------------

void GingoTempo::bpmToMarking(float bpm, char* buf, uint8_t maxLen) {
    // Find the closest marking by comparing to midpoint of each range
    float minDist = 10000.0f;
    uint8_t bestIdx = 4; // default: Moderato

    for (uint8_t i = 0; i < data::TEMPO_MARKINGS_SIZE; i++) {
        uint8_t mid = pgm_read_byte(&data::TEMPO_MARKINGS[i].bpm_mid);
        float dist = bpm - (float)mid;
        if (dist < 0) dist = -dist;
        if (dist < minDist) {
            minDist = dist;
            bestIdx = i;
        }
    }

    data::readPgmStr(buf, data::TEMPO_MARKINGS[bestIdx].name, maxLen);
}

float GingoTempo::markingToBpm(const char* marking) {
    for (uint8_t i = 0; i < data::TEMPO_MARKINGS_SIZE; i++) {
        char buf[14];
        data::readPgmStr(buf, data::TEMPO_MARKINGS[i].name, sizeof(buf));
        if (strcmp(marking, buf) == 0) {
            return (float)pgm_read_byte(&data::TEMPO_MARKINGS[i].bpm_mid);
        }
    }
    return -1.0f;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_TEMPO

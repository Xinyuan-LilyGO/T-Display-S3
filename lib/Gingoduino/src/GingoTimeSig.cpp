// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoTimeSig.
//
// SPDX-License-Identifier: MIT

#include "GingoTimeSig.h"

#if GINGODUINO_HAS_TIMESIG

#include "GingoDuration.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GingoTimeSig::GingoTimeSig(uint8_t beatsPerBar, uint8_t beatUnit)
    : beatsPerBar_(beatsPerBar), beatUnit_(beatUnit)
{
    // Validate beat unit is a power of 2 (1, 2, 4, 8, 16, 32)
    if (beatUnit_ == 0 || (beatUnit_ & (beatUnit_ - 1)) != 0) {
        beatUnit_ = 4;
    }
    if (beatsPerBar_ == 0) {
        beatsPerBar_ = 4;
    }
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

GingoDuration GingoTimeSig::barDuration() const {
    // Total duration of one bar:
    // 4/4 → 4 quarter notes → 4/4 = whole note
    // 6/8 → 6 eighth notes  → 6/8 = 3/4
    return GingoDuration((int16_t)beatsPerBar_, (int16_t)beatUnit_);
}

bool GingoTimeSig::isCompound() const {
    // Compound: beats divisible by 3 and beat_unit = 8
    return (beatsPerBar_ % 3 == 0) && (beatUnit_ == 8);
}

const char* GingoTimeSig::classification(char* buf, uint8_t maxLen) const {
    const char* s = isCompound() ? "compound" : "simple";
    uint8_t i = 0;
    while (s[i] && i < maxLen - 1) { buf[i] = s[i]; i++; }
    buf[i] = '\0';
    return buf;
}

const char* GingoTimeSig::commonName(char* buf, uint8_t maxLen) const {
    if (beatsPerBar_ == 4 && beatUnit_ == 4) {
        const char* s = "common time";
        uint8_t i = 0;
        while (s[i] && i < maxLen - 1) { buf[i] = s[i]; i++; }
        buf[i] = '\0';
        return buf;
    }
    if (beatsPerBar_ == 2 && beatUnit_ == 2) {
        const char* s = "cut time";
        uint8_t i = 0;
        while (s[i] && i < maxLen - 1) { buf[i] = s[i]; i++; }
        buf[i] = '\0';
        return buf;
    }
    return toString(buf, maxLen);
}

const char* GingoTimeSig::toString(char* buf, uint8_t maxLen) const {
    // Write "X/Y" format
    if (maxLen < 4) { buf[0] = '\0'; return buf; }

    uint8_t pos = 0;

    // Write beatsPerBar digits
    if (beatsPerBar_ >= 100 && pos < maxLen - 1) buf[pos++] = '0' + (beatsPerBar_ / 100);
    if (beatsPerBar_ >= 10 && pos < maxLen - 1) buf[pos++] = '0' + ((beatsPerBar_ / 10) % 10);
    if (pos < maxLen - 1) buf[pos++] = '0' + (beatsPerBar_ % 10);

    // Slash
    if (pos < maxLen - 1) buf[pos++] = '/';

    // Write beatUnit digits
    if (beatUnit_ >= 100 && pos < maxLen - 1) buf[pos++] = '0' + (beatUnit_ / 100);
    if (beatUnit_ >= 10 && pos < maxLen - 1) buf[pos++] = '0' + ((beatUnit_ / 10) % 10);
    if (pos < maxLen - 1) buf[pos++] = '0' + (beatUnit_ % 10);

    buf[pos] = '\0';
    return buf;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_TIMESIG

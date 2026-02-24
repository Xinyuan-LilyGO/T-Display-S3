// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoNote.
//
// SPDX-License-Identifier: MIT

#include "GingoNote.h"
#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// MIDI conversion
// ---------------------------------------------------------------------------

GingoNote GingoNote::fromMIDI(uint8_t midiNote) {
    uint8_t semitone = midiNote % 12;
    char noteName[3];
    data::readChromaticName(semitone, noteName, sizeof(noteName));
    return GingoNote(noteName);
}

int8_t GingoNote::octaveFromMIDI(uint8_t midiNote) {
    return (int8_t)(midiNote / 12) - 1;
}

// ---------------------------------------------------------------------------
// Static utilities
// ---------------------------------------------------------------------------

void GingoNote::toNatural(const char* noteName, char* output, uint8_t maxLen) {
    if (!noteName || maxLen == 0) { if (maxLen) output[0] = '\0'; return; }

    int8_t idx = data::findEnharmonic(noteName);
    if (idx >= 0) {
        data::readPgmStr(output, data::ENHARMONIC_MAP[idx].output, maxLen);
    } else {
        // Not in map — copy as-is (already natural)
        uint8_t i = 0;
        while (i < maxLen - 1 && noteName[i]) {
            output[i] = noteName[i];
            i++;
        }
        output[i] = '\0';
    }
}

uint8_t GingoNote::toSemitone(const char* noteName) {
    // First resolve to natural form
    char natural[5];
    toNatural(noteName, natural, sizeof(natural));

    // Search in chromatic table
    for (uint8_t i = 0; i < 12; i++) {
        char chromName[3];
        data::readPgmStr(chromName, data::CHROMATIC_NAMES[i], sizeof(chromName));
        if (strcmp(natural, chromName) == 0) {
            return i;
        }
    }
    return 0; // default to C
}

char GingoNote::extractSound(const char* name) {
    if (!name) return 'C';
    for (uint8_t i = 0; name[i]; i++) {
        if (name[i] >= 'A' && name[i] <= 'G') {
            return name[i];
        }
    }
    return 'C';
}

void GingoNote::extractRoot(const char* chordName, char* output, uint8_t maxLen) {
    if (!chordName || maxLen < 2) { if (maxLen) output[0] = '\0'; return; }

    // Find first uppercase letter A-G
    uint8_t letterPos = 255;
    for (uint8_t i = 0; chordName[i]; i++) {
        if (chordName[i] >= 'A' && chordName[i] <= 'G') {
            letterPos = i;
            break;
        }
    }
    if (letterPos == 255) { output[0] = '\0'; return; }

    uint8_t out = 0;
    output[out++] = chordName[letterPos];

    // Check for accidentals after the letter
    uint8_t after = letterPos + 1;
    if (chordName[after] == '#' || chordName[after] == 'b') {
        char acc = chordName[after];
        if (out < maxLen - 1) output[out++] = acc;
        // Check for double accidental (same type)
        if (chordName[after + 1] == acc) {
            if (out < maxLen - 1) output[out++] = acc;
        }
    }
    // Check for accidentals before the letter
    else if (letterPos > 0 && (chordName[letterPos - 1] == 'b' || chordName[letterPos - 1] == '#')) {
        char acc = chordName[letterPos - 1];
        if (out < maxLen - 1) output[out++] = acc;
        if (letterPos >= 2 && chordName[letterPos - 2] == acc) {
            if (out < maxLen - 1) output[out++] = acc;
        }
    }

    output[out] = '\0';
}

void GingoNote::extractType(const char* chordName, char* output, uint8_t maxLen) {
    if (!chordName || maxLen < 1) { if (maxLen) output[0] = '\0'; return; }

    // Find first uppercase letter A-G
    uint8_t letterPos = 255;
    for (uint8_t i = 0; chordName[i]; i++) {
        if (chordName[i] >= 'A' && chordName[i] <= 'G') {
            letterPos = i;
            break;
        }
    }
    if (letterPos == 255) { output[0] = '\0'; return; }

    // Skip past the letter
    uint8_t after = letterPos + 1;

    // Skip trailing accidentals
    if (chordName[after] == '#' || chordName[after] == 'b') {
        char acc = chordName[after];
        after++;
        if (chordName[after] == acc) after++;
    }

    // Copy the rest as chord type
    uint8_t out = 0;
    while (chordName[after] && out < maxLen - 1) {
        output[out++] = chordName[after++];
    }
    output[out] = '\0';
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GingoNote::GingoNote()
    : name_("C"), natural_("C"), sound_('C'), semitone_(0)
{}

GingoNote::GingoNote(const char* name) {
    name_.set(name);

    // Resolve natural form
    char natBuf[5];
    toNatural(name, natBuf, sizeof(natBuf));
    natural_.set(natBuf);

    // Extract sound (base letter)
    sound_ = extractSound(name);

    // Compute semitone
    semitone_ = toSemitone(name);
}

// ---------------------------------------------------------------------------
// Instance methods
// ---------------------------------------------------------------------------

float GingoNote::frequency(int8_t octave, float tuning) const {
    float exponent = (float)(semitone_ - 9 + 12 * (octave - 4)) / 12.0f;
    // pow(2, x) = exp(x * ln2)
    float power = 1.0f;
    // Simple iterative pow2 for embedded compatibility
    if (exponent >= 0) {
        float base = 2.0f;
        int intPart = (int)exponent;
        float fracPart = exponent - intPart;
        for (int i = 0; i < intPart; i++) power *= base;
        // Approximate 2^frac using Taylor series around 0
        // 2^x ≈ 1 + x*ln2 + (x*ln2)^2/2 + (x*ln2)^3/6
        float ln2f = 0.693147f * fracPart;
        power *= (1.0f + ln2f + ln2f * ln2f * 0.5f + ln2f * ln2f * ln2f * 0.166667f);
    } else {
        float posExp = -exponent;
        float base = 2.0f;
        int intPart = (int)posExp;
        float fracPart = posExp - intPart;
        for (int i = 0; i < intPart; i++) power *= base;
        float ln2f = 0.693147f * fracPart;
        power *= (1.0f + ln2f + ln2f * ln2f * 0.5f + ln2f * ln2f * ln2f * 0.166667f);
        power = 1.0f / power;
    }
    return tuning * power;
}

GingoNote GingoNote::transpose(int8_t semitones) const {
    int8_t newIdx = (int8_t)(((int16_t)semitone_ + semitones % 12 + 12) % 12);
    char name[3];
    data::readChromaticName((uint8_t)newIdx, name, sizeof(name));
    return GingoNote(name);
}

uint8_t GingoNote::distance(const GingoNote& other) const {
    int8_t posA = (int8_t)((semitone_ * 7) % 12);
    int8_t posB = (int8_t)((other.semitone_ * 7) % 12);
    int8_t raw = posA - posB;
    if (raw < 0) raw = -raw;
    return (uint8_t)((raw < 12 - raw) ? raw : 12 - raw);
}

} // namespace gingoduino

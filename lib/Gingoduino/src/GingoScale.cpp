// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoScale.
//
// SPDX-License-Identifier: MIT

#include "GingoScale.h"

#if GINGODUINO_HAS_SCALE

#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Parse type name
// ---------------------------------------------------------------------------

ScaleType GingoScale::parseTypeName(const char* name, uint8_t* outMode) {
    *outMode = 1;
    if (!name) return SCALE_MAJOR;

    // Lowercase compare helper (inline)
    auto toLower = [](char c) -> char {
        return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
    };

    // Make lowercase copy for comparison
    char lower[24];
    uint8_t i = 0;
    while (name[i] && i < 23) {
        lower[i] = toLower(name[i]);
        i++;
    }
    lower[i] = '\0';

    // Direct parent names
    if (strcmp(lower, "major") == 0)           return SCALE_MAJOR;
    if (strcmp(lower, "natural minor") == 0)   return SCALE_NATURAL_MINOR;
    if (strcmp(lower, "harmonic minor") == 0)  return SCALE_HARMONIC_MINOR;
    if (strcmp(lower, "melodic minor") == 0)   return SCALE_MELODIC_MINOR;
    if (strcmp(lower, "diminished") == 0)      return SCALE_DIMINISHED;
    if (strcmp(lower, "harmonic major") == 0)  return SCALE_HARMONIC_MAJOR;
    if (strcmp(lower, "whole tone") == 0)      return SCALE_WHOLE_TONE;
    if (strcmp(lower, "augmented") == 0)       return SCALE_AUGMENTED;
    if (strcmp(lower, "blues") == 0)           return SCALE_BLUES;
    if (strcmp(lower, "chromatic") == 0)       return SCALE_CHROMATIC;

    // Mode names (Major family)
    if (strcmp(lower, "ionian") == 0)     { *outMode = 1; return SCALE_MAJOR; }
    if (strcmp(lower, "dorian") == 0)     { *outMode = 2; return SCALE_MAJOR; }
    if (strcmp(lower, "phrygian") == 0)   { *outMode = 3; return SCALE_MAJOR; }
    if (strcmp(lower, "lydian") == 0)     { *outMode = 4; return SCALE_MAJOR; }
    if (strcmp(lower, "mixolydian") == 0) { *outMode = 5; return SCALE_MAJOR; }
    if (strcmp(lower, "aeolian") == 0)    { *outMode = 6; return SCALE_MAJOR; }
    if (strcmp(lower, "locrian") == 0)    { *outMode = 7; return SCALE_MAJOR; }

    // Mode names (Melodic Minor family)
    if (strcmp(lower, "altered") == 0)          { *outMode = 7; return SCALE_MELODIC_MINOR; }
    if (strcmp(lower, "lydian dominant") == 0)   { *outMode = 4; return SCALE_MELODIC_MINOR; }
    if (strcmp(lower, "lydian augmented") == 0)  { *outMode = 3; return SCALE_MELODIC_MINOR; }

    // Mode names (Harmonic Minor family)
    if (strcmp(lower, "phrygian dominant") == 0) { *outMode = 5; return SCALE_HARMONIC_MINOR; }

    // Pentatonic
    if (strcmp(lower, "major pentatonic") == 0) { *outMode = 1; return SCALE_MAJOR; }
    if (strcmp(lower, "minor pentatonic") == 0) { *outMode = 6; return SCALE_MAJOR; }

    return SCALE_MAJOR;
}

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GingoScale::GingoScale(const char* tonic, ScaleType type, uint8_t modeNum, bool penta)
    : tonic_(tonic), parent_(type), modeNumber_(modeNum), pentatonic_(penta)
{}

GingoScale::GingoScale(const char* tonic, const char* typeName)
    : tonic_(tonic), pentatonic_(false)
{
    // Check for "pentatonic" suffix
    if (typeName) {
        const char* p = strstr(typeName, "pentatonic");
        if (p || strstr(typeName, "Pentatonic")) {
            pentatonic_ = true;
        }
    }

    uint8_t mode = 1;
    parent_ = parseTypeName(typeName, &mode);
    modeNumber_ = mode;
}

// ---------------------------------------------------------------------------
// Compute 12-bit mask
// ---------------------------------------------------------------------------

uint16_t GingoScale::computeMask12() const {
    // Read the 24-bit scale mask
    uint32_t mask24 = pgm_read_dword(&data::SCALE_MASKS[parent_]);

    // Extract only the first 12 bits (one octave)
    uint16_t mask12 = (uint16_t)(mask24 & 0x0FFF);

    // Rotate by mode offset: mode 2 starts at the 2nd active note
    if (modeNumber_ > 1) {
        // Find the semitone offset for this mode
        uint8_t activeCount = 0;
        uint8_t offset = 0;
        for (uint8_t i = 0; i < 12; i++) {
            if (mask12 & (1 << i)) {
                activeCount++;
                if (activeCount == modeNumber_) {
                    offset = i;
                    break;
                }
            }
        }
        // Rotate the 12-bit mask
        mask12 = (uint16_t)(((mask12 >> offset) | (mask12 << (12 - offset))) & 0x0FFF);
    }

    // Apply pentatonic filter if needed
    if (pentatonic_) {
        // Keep only degrees 1, 2, 3, 5, 6 (skip 4 and 7)
        // This means keep the 1st, 2nd, 3rd, 5th, 6th active notes
        uint16_t filtered = 0;
        uint8_t active = 0;
        for (uint8_t i = 0; i < 12; i++) {
            if (mask12 & (1 << i)) {
                active++;
                // Keep notes 1, 2, 3, 5, 6 — skip 4 and 7
                if (active != 4 && active != 7) {
                    filtered |= (1 << i);
                }
            }
        }
        mask12 = filtered;
    }

    return mask12;
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

uint8_t GingoScale::size() const {
    uint16_t mask = computeMask12();
    uint8_t count = 0;
    for (uint8_t i = 0; i < 12; i++) {
        if (mask & (1 << i)) count++;
    }
    return count;
}

uint8_t GingoScale::notes(GingoNote* output, uint8_t maxNotes) const {
    uint16_t mask = computeMask12();
    uint8_t rootSt = tonic_.semitone();
    uint8_t written = 0;

    for (uint8_t i = 0; i < 12 && written < maxNotes; i++) {
        if (mask & (1 << i)) {
            uint8_t noteSt = (rootSt + i) % 12;
            char name[3];
            data::readChromaticName(noteSt, name, sizeof(name));
            output[written++] = GingoNote(name);
        }
    }
    return written;
}

GingoNote GingoScale::degree(uint8_t n) const {
    uint16_t mask = computeMask12();
    uint8_t rootSt = tonic_.semitone();
    uint8_t activeCount = 0;

    for (uint8_t i = 0; i < 12; i++) {
        if (mask & (1 << i)) {
            activeCount++;
            if (activeCount == n) {
                uint8_t noteSt = (rootSt + i) % 12;
                char name[3];
                data::readChromaticName(noteSt, name, sizeof(name));
                return GingoNote(name);
            }
        }
    }
    return tonic_; // fallback
}

bool GingoScale::contains(const GingoNote& note) const {
    uint16_t mask = computeMask12();
    uint8_t rootSt = tonic_.semitone();
    uint8_t offset = (note.semitone() - rootSt + 12) % 12;
    return (mask & (1 << offset)) != 0;
}

GingoScale GingoScale::mode(uint8_t degreeNumber) const {
    // Get the note at the target degree
    GingoNote newTonic = degree(degreeNumber);
    // Compute new mode number relative to parent
    uint8_t newMode = (uint8_t)(((modeNumber_ - 1 + degreeNumber - 1) %
                       pgm_read_byte(&data::SCALE_SIZES[parent_])) + 1);
    return GingoScale(newTonic.name(), parent_, newMode, pentatonic_);
}

GingoScale GingoScale::pentatonic() const {
    return GingoScale(tonic_.name(), parent_, modeNumber_, true);
}

const char* GingoScale::quality() const {
    // Check if the third degree is major (4 semitones) or minor (3 semitones)
    uint16_t mask = computeMask12();
    if (mask & (1 << 4)) return "major";   // major third present
    if (mask & (1 << 3)) return "minor";   // minor third present
    return "major"; // default
}

const char* GingoScale::modeName(char* buf, uint8_t maxLen) const {
    if (parent_ == SCALE_MAJOR && modeNumber_ >= 1 && modeNumber_ <= 7) {
        const char* ptr = (const char*)pgm_read_ptr(&data::MODE_NAMES_MAJOR[modeNumber_ - 1]);
        data::readPgmStr(buf, ptr, maxLen);
    } else if (parent_ == SCALE_HARMONIC_MINOR && modeNumber_ >= 1 && modeNumber_ <= 7) {
        const char* ptr = (const char*)pgm_read_ptr(&data::MODE_NAMES_HARMONIC_MINOR[modeNumber_ - 1]);
        data::readPgmStr(buf, ptr, maxLen);
    } else if (parent_ == SCALE_MELODIC_MINOR && modeNumber_ >= 1 && modeNumber_ <= 7) {
        const char* ptr = (const char*)pgm_read_ptr(&data::MODE_NAMES_MELODIC_MINOR[modeNumber_ - 1]);
        data::readPgmStr(buf, ptr, maxLen);
    } else {
        // Use the parent scale name
        const char* ptr = (const char*)pgm_read_ptr(&data::SCALE_TYPE_NAMES[parent_]);
        data::readPgmStr(buf, ptr, maxLen);
    }
    return buf;
}

// ---------------------------------------------------------------------------
// New methods: signature, relative, parallel, degreeOf, modeByName,
//              brightness, mask
// ---------------------------------------------------------------------------

int8_t GingoScale::signature() const {
    // Key signature based on circle-of-fifths position.
    // Position of tonic in the circle: C=0, G=1, D=2, ... F=-1, Bb=-2, ...
    static const int8_t FIFTHS_POS[12] PROGMEM = {
    //  C   C#  D   D#  E   F   F#  G   G#  A   A#  B
        0,  7,  2, -3,  4, -1,  6,  1, -4,  3, -2,  5
    };
    // Mode offsets: how many sharps/flats each mode adds relative to major
    static const int8_t MODE_FIFTHS_OFFSET[7] PROGMEM = {
    //  Ion  Dor  Phr  Lyd  Mix  Aeo  Loc
        0,  -2,  -4,   1,  -1,  -3,  -5
    };

    int8_t pos = (int8_t)pgm_read_byte(&FIFTHS_POS[tonic_.semitone()]);

    // Only the 7-note diatonic families have meaningful mode offsets
    if (parent_ <= SCALE_MELODIC_MINOR && modeNumber_ >= 1 && modeNumber_ <= 7) {
        pos += (int8_t)pgm_read_byte(&MODE_FIFTHS_OFFSET[modeNumber_ - 1]);
    }

    // Clamp to -7..+7
    if (pos > 7) pos -= 12;
    if (pos < -7) pos += 12;
    return pos;
}

GingoScale GingoScale::relative() const {
    const char* q = quality();
    if (strcmp(q, "major") == 0) {
        return mode(6);  // major → aeolian (relative minor)
    }
    return mode(3);      // minor → ionian (relative major)
}

GingoScale GingoScale::parallel() const {
    const char* q = quality();
    if (strcmp(q, "major") == 0) {
        // Parallel minor: same tonic, aeolian mode of parent
        return GingoScale(tonic_.name(), parent_, 6, pentatonic_);
    }
    // Parallel major: same tonic, ionian mode of parent
    return GingoScale(tonic_.name(), parent_, 1, pentatonic_);
}

uint8_t GingoScale::degreeOf(const GingoNote& note) const {
    uint16_t m = computeMask12();
    uint8_t rootSt = tonic_.semitone();
    uint8_t offset = (note.semitone() - rootSt + 12) % 12;

    if (!(m & (1 << offset))) return 0;  // not in scale

    uint8_t deg = 0;
    for (uint8_t i = 0; i <= offset; i++) {
        if (m & (1 << i)) deg++;
    }
    return deg;
}

GingoScale GingoScale::modeByName(const char* modeName) const {
    uint8_t modeNum = 1;
    ScaleType st = parseTypeName(modeName, &modeNum);

    // If the parsed type matches our parent, use that mode
    if (st == parent_) {
        return GingoScale(tonic_.name(), parent_, modeNum, pentatonic_);
    }
    // Otherwise build from the target scale type
    return GingoScale(tonic_.name(), st, modeNum, pentatonic_);
}

uint8_t GingoScale::brightness() const {
    if (parent_ == SCALE_MAJOR && modeNumber_ >= 1 && modeNumber_ <= 7) {
        return pgm_read_byte(&data::MODE_BRIGHTNESS_MAJOR[modeNumber_ - 1]);
    }
    return 0;
}

uint16_t GingoScale::mask() const {
    return computeMask12();
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_SCALE

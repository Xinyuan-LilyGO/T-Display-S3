// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoField.
//
// SPDX-License-Identifier: MIT

#include "GingoField.h"

#if GINGODUINO_HAS_FIELD

#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Harmonic function table per scale type (up to 8 degrees)
// ---------------------------------------------------------------------------

// Major:        T  S  T  S  D  T  D
// NaturalMinor: T  S  T  S  D  S  D
// HarmonicMinor:T  S  T  S  D  S  D
// MelodicMinor: T  S  T  S  D  S  D
// Diminished:   T  D  T  D  T  D  T  D

static const uint8_t FUNC_TABLE[][8] PROGMEM = {
    {0, 1, 0, 1, 2, 0, 2, 0},  // Major
    {0, 1, 0, 1, 2, 1, 2, 0},  // NaturalMinor
    {0, 1, 0, 1, 2, 1, 2, 0},  // HarmonicMinor
    {0, 1, 0, 1, 2, 1, 2, 0},  // MelodicMinor
    {0, 2, 0, 2, 0, 2, 0, 2},  // Diminished
    {0, 1, 0, 1, 2, 1, 2, 0},  // HarmonicMajor
    {0, 0, 0, 0, 0, 0, 0, 0},  // WholeTone (all tonic-like)
    {0, 0, 0, 0, 0, 0, 0, 0},  // Augmented
    {0, 1, 2, 0, 1, 2, 0, 0},  // Blues
    {0, 0, 0, 0, 0, 0, 0, 0},  // Chromatic
};

// Role names
static const char R_PRIMARY[]   PROGMEM = "primary";
static const char R_REL_I[]     PROGMEM = "relative of I";
static const char R_REL_IV[]    PROGMEM = "relative of IV";
static const char R_REL_V[]     PROGMEM = "relative of V";
static const char R_TRANS[]     PROGMEM = "transitive";

// Role table for Major scale (7 degrees)
static const char* const ROLE_TABLE_MAJOR[7] PROGMEM = {
    R_PRIMARY, R_REL_IV, R_TRANS, R_PRIMARY,
    R_PRIMARY, R_REL_I, R_REL_V
};

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GingoField::GingoField()
    : scale_("C", SCALE_MAJOR)
{}

GingoField::GingoField(const char* tonic, ScaleType type)
    : scale_(tonic, type)
{}

GingoField::GingoField(const char* tonic, const char* typeName)
    : scale_(tonic, typeName)
{}

// ---------------------------------------------------------------------------
// Build chords by stacking scale degrees
// ---------------------------------------------------------------------------

uint8_t GingoField::buildChords(GingoChord* output, uint8_t maxChords,
                                const uint8_t* offsets, uint8_t offsetCount) const {
    // Get all scale notes
    GingoNote scaleNotes[12];
    uint8_t scaleSize = scale_.notes(scaleNotes, 12);
    if (scaleSize == 0) return 0;

    uint8_t written = 0;
    for (uint8_t i = 0; i < scaleSize && written < maxChords; i++) {
        // Collect chord tones by picking scale notes at the offsets
        GingoNote chordNotes[7];
        uint8_t chordNoteCount = 0;

        for (uint8_t j = 0; j < offsetCount && chordNoteCount < 7; j++) {
            uint8_t idx = (i + offsets[j]) % scaleSize;
            chordNotes[chordNoteCount++] = scaleNotes[idx];
        }

        // Try to identify the chord
        char chordName[16];
        if (GingoChord::identify(chordNotes, chordNoteCount, chordName, sizeof(chordName))) {
            output[written++] = GingoChord(chordName);
        } else {
            // Fallback: root + "M"
            char fallback[8];
            uint8_t pos = 0;
            const char* rn = chordNotes[0].name();
            while (rn[pos] && pos < 6) { fallback[pos] = rn[pos]; pos++; }
            fallback[pos++] = 'M';
            fallback[pos] = '\0';
            output[written++] = GingoChord(fallback);
        }
    }
    return written;
}

// ---------------------------------------------------------------------------
// Public accessors
// ---------------------------------------------------------------------------

uint8_t GingoField::chords(GingoChord* output, uint8_t maxChords) const {
    const uint8_t triadOffsets[] = {0, 2, 4}; // 1st, 3rd, 5th
    return buildChords(output, maxChords, triadOffsets, 3);
}

uint8_t GingoField::sevenths(GingoChord* output, uint8_t maxChords) const {
    const uint8_t seventhOffsets[] = {0, 2, 4, 6}; // 1st, 3rd, 5th, 7th
    return buildChords(output, maxChords, seventhOffsets, 4);
}

GingoChord GingoField::chord(uint8_t degree) const {
    GingoChord buf[12];
    uint8_t n = chords(buf, 12);
    if (degree >= 1 && degree <= n) return buf[degree - 1];
    return GingoChord(); // fallback
}

GingoChord GingoField::seventh(uint8_t degree) const {
    GingoChord buf[12];
    uint8_t n = sevenths(buf, 12);
    if (degree >= 1 && degree <= n) return buf[degree - 1];
    return GingoChord();
}

HarmonicFunc GingoField::function(uint8_t degree) const {
    if (degree < 1 || degree > size()) return FUNC_TONIC;
    uint8_t typeIdx = (uint8_t)scale_.parent();
    if (typeIdx >= SCALE_TYPE_COUNT) typeIdx = 0;
    return (HarmonicFunc)pgm_read_byte(&FUNC_TABLE[typeIdx][degree - 1]);
}

const char* GingoField::role(uint8_t degree, char* buf, uint8_t maxLen) const {
    if (scale_.parent() == SCALE_MAJOR && degree >= 1 && degree <= 7) {
        const char* ptr = (const char*)pgm_read_ptr(&ROLE_TABLE_MAJOR[degree - 1]);
        data::readPgmStr(buf, ptr, maxLen);
    } else {
        data::readPgmStr(buf, R_PRIMARY, maxLen);
    }
    return buf;
}

HarmonicFunc GingoField::functionOf(const GingoChord& chord) const {
    uint8_t deg = scale_.degreeOf(chord.root());
    if (deg == 0) return FUNC_TONIC;
    return function(deg);
}

HarmonicFunc GingoField::functionOf(const char* chordName) const {
    GingoChord c(chordName);
    return functionOf(c);
}

const char* GingoField::roleOf(const GingoChord& chord, char* buf, uint8_t maxLen) const {
    uint8_t deg = scale_.degreeOf(chord.root());
    if (deg == 0) {
        data::readPgmStr(buf, R_PRIMARY, maxLen);
        return buf;
    }
    return role(deg, buf, maxLen);
}

const char* GingoField::roleOf(const char* chordName, char* buf, uint8_t maxLen) const {
    GingoChord c(chordName);
    return roleOf(c, buf, maxLen);
}

// ---------------------------------------------------------------------------
// deduce — infer probable harmonic fields from notes or chords
// ---------------------------------------------------------------------------

// Helper: detect if string looks like a bare note (1-2 chars, A-G + optional #/b)
static bool looksLikeNote(const char* s) {
    if (!s || !s[0]) return false;
    char c = s[0];
    if (c < 'A' || c > 'G') return false;
    if (s[1] == '\0') return true;
    if ((s[1] == '#' || s[1] == 'b') && s[2] == '\0') return true;
    if (s[1] == '#' && s[2] == '#' && s[3] == '\0') return true;
    if (s[1] == 'b' && s[2] == 'b' && s[3] == '\0') return true;
    return false;
}

// Helper: simple insertion sort for FieldMatch by (matched desc, scaleType asc)
static void sortMatches(FieldMatch* arr, uint8_t n) {
    for (uint8_t i = 1; i < n; i++) {
        FieldMatch key = arr[i];
        int8_t j = (int8_t)(i - 1);
        while (j >= 0 && (arr[j].matched < key.matched ||
               (arr[j].matched == key.matched && arr[j].scaleType > key.scaleType))) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

uint8_t GingoField::deduce(const char* const* items, uint8_t itemCount,
                           FieldMatch* output, uint8_t maxResults) {
    if (itemCount == 0 || maxResults == 0) return 0;

    // Detect input type from first item
    bool noteMode = looksLikeNote(items[0]);

    // Candidate scale types (same 5 as gingo)
    static const ScaleType DEDUCE_TYPES[] = {
        SCALE_MAJOR, SCALE_NATURAL_MINOR, SCALE_HARMONIC_MINOR,
        SCALE_MELODIC_MINOR, SCALE_HARMONIC_MAJOR
    };

    // Chromatic tonic names
    static const char* const TONICS[12] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    // Collect all candidates in a temporary buffer (max 60)
    // We use maxResults as output cap but need to compare all 60 internally
    FieldMatch candidates[60];
    uint8_t candCount = 0;

    for (uint8_t t = 0; t < 5; t++) {
        ScaleType st = DEDUCE_TYPES[t];
        for (uint8_t k = 0; k < 12; k++) {
            GingoField field(TONICS[k], st);
            uint8_t matched = 0;
            uint8_t roleCount = 0;

            FieldMatch& fm = candidates[candCount];
            fm.tonicName = TONICS[k];
            fm.scaleType = st;
            fm.total = itemCount;
            fm.roleCount = 0;

            if (noteMode) {
                // Note mode: check if each note's pitch class belongs to the scale
                for (uint8_t i = 0; i < itemCount; i++) {
                    GingoNote n(items[i]);
                    uint8_t deg = field.scale().degreeOf(n);
                    if (deg > 0) {
                        matched++;
                        if (roleCount < 7) {
                            // Write roman numeral degree
                            static const char* const ROMAN[] = {
                                "", "I", "II", "III", "IV", "V", "VI", "VII"
                            };
                            if (deg <= 7) {
                                uint8_t ri = 0;
                                const char* rom = ROMAN[deg];
                                while (rom[ri] && ri < 7) {
                                    fm.roles[roleCount][ri] = rom[ri];
                                    ri++;
                                }
                                fm.roles[roleCount][ri] = '\0';
                                roleCount++;
                            }
                        }
                    }
                }
            } else {
                // Chord mode: check if each chord matches a triad or seventh
                GingoChord triads[7];
                GingoChord seventhsArr[7];
                uint8_t nTriads = field.chords(triads, 7);
                uint8_t nSevenths = field.sevenths(seventhsArr, 7);

                for (uint8_t i = 0; i < itemCount; i++) {
                    GingoChord inputChord(items[i]);
                    uint8_t inputRoot = inputChord.root().semitone();
                    const char* inputType = inputChord.type();
                    bool found = false;

                    // Check triads
                    for (uint8_t d = 0; d < nTriads && !found; d++) {
                        if (triads[d].root().semitone() == inputRoot &&
                            strcmp(triads[d].type(), inputType) == 0) {
                            found = true;
                            if (roleCount < 7) {
                                static const char* const ROMAN[] = {
                                    "I", "II", "III", "IV", "V", "VI", "VII"
                                };
                                uint8_t ri = 0;
                                const char* rom = ROMAN[d];
                                while (rom[ri] && ri < 7) {
                                    fm.roles[roleCount][ri] = rom[ri];
                                    ri++;
                                }
                                fm.roles[roleCount][ri] = '\0';
                                roleCount++;
                            }
                        }
                    }

                    // Check sevenths
                    for (uint8_t d = 0; d < nSevenths && !found; d++) {
                        if (seventhsArr[d].root().semitone() == inputRoot &&
                            strcmp(seventhsArr[d].type(), inputType) == 0) {
                            found = true;
                            if (roleCount < 7) {
                                static const char* const ROMAN7[] = {
                                    "I7", "II7", "III7", "IV7", "V7", "VI7", "VII7"
                                };
                                uint8_t ri = 0;
                                const char* rom = ROMAN7[d];
                                while (rom[ri] && ri < 7) {
                                    fm.roles[roleCount][ri] = rom[ri];
                                    ri++;
                                }
                                fm.roles[roleCount][ri] = '\0';
                                roleCount++;
                            }
                        }
                    }

                    if (found) matched++;
                }
            }

            fm.matched = matched;
            fm.roleCount = roleCount;

            // Only keep candidates with at least one match
            if (matched > 0) {
                candCount++;
            }
        }
    }

    // Sort by matched desc, then scaleType asc
    sortMatches(candidates, candCount);

    // Copy top results
    uint8_t written = (candCount < maxResults) ? candCount : maxResults;
    for (uint8_t i = 0; i < written; i++) {
        output[i] = candidates[i];
    }
    return written;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_FIELD

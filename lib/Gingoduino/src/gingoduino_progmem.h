// Gingoduino — Music Theory Library for Embedded Systems
// All music theory lookup data stored in PROGMEM (flash memory).
//
// SPDX-License-Identifier: MIT

#ifndef GINGODUINO_PROGMEM_H
#define GINGODUINO_PROGMEM_H

#include "gingoduino_config.h"

namespace gingoduino {
namespace data {

// ===================================================================
// 1. CHROMATIC SCALE — 12 pitch classes
// ===================================================================

static const char CHROMATIC_NAMES[12][3] PROGMEM = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
};

// Base letter (sound) for each chromatic index
static const char CHROMATIC_SOUND[12] PROGMEM = {
    'C', 'C', 'D', 'D', 'E', 'F', 'F', 'G', 'G', 'A', 'A', 'B'
};

// Circle of fifths order
static const uint8_t FIFTHS_ORDER[12] PROGMEM = {
    0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5
    // C, G, D, A, E, B, F#, C#, G#, D#, A#, F
};

// ===================================================================
// 2. ENHARMONIC MAP — sorted for binary search (ASCII only)
// ===================================================================

struct EnharmonicEntry {
    char input[5];    // max "C##" + null
    char output[3];   // max "F#" + null
};

// Sorted alphabetically by input for binary search.
// Unicode entries (♭) omitted — embedded uses ASCII notation.
static const EnharmonicEntry ENHARMONIC_MAP[] PROGMEM = {
    {"#B",  "C" }, {"#E",  "F" },
    {"##A", "B" }, {"##B", "C#"}, {"##C", "D" }, {"##D", "E" },
    {"##E", "F#"}, {"##F", "G" }, {"##G", "A" },
    {"A##", "B" }, {"Ab",  "G#"}, {"Abb", "G" },
    {"B##", "C#"}, {"B#",  "C" }, {"Bb",  "A#"}, {"Bbb", "A" },
    {"C##", "D" }, {"Cb",  "B" }, {"Cbb", "B" },
    {"D##", "E" }, {"Db",  "C#"}, {"Dbb", "C" },
    {"E##", "F#"}, {"E#",  "F" }, {"Eb",  "D#"}, {"Ebb", "D" },
    {"F##", "G" }, {"Fb",  "E" }, {"Fbb", "E" },
    {"G##", "A" }, {"Gb",  "F#"}, {"Gbb", "F" },
    {"bA",  "G#"}, {"bB",  "A#"}, {"bC",  "B" },
    {"bD",  "C#"}, {"bE",  "D#"}, {"bF",  "E" }, {"bG",  "F#"},
    {"bbA", "G" }, {"bbB", "A" }, {"bbC", "B" },
    {"bbD", "C" }, {"bbE", "D" }, {"bbF", "E" }, {"bbG", "F" },
};

static const uint8_t ENHARMONIC_MAP_SIZE = sizeof(ENHARMONIC_MAP) / sizeof(ENHARMONIC_MAP[0]);

// ===================================================================
// 3. INTERVAL TABLE — 24 intervals across two octaves
// ===================================================================

struct IntervalData {
    char    label[5];     // "P1", "5J", "#11", "b13"
    char    anglo[5];     // "P1", "P5", "d11", "mi13"
    uint8_t degree;       // diatonic degree 1-14
    uint8_t octave;       // 1 or 2
    // semitones = array index (0-23)
};

static const IntervalData INTERVAL_TABLE[24] PROGMEM = {
    // idx  label   anglo   degree octave
    /*  0*/ {"P1",  "P1",   1,  1},
    /*  1*/ {"2m",  "mi2",  2,  1},
    /*  2*/ {"2M",  "ma2",  2,  1},
    /*  3*/ {"3m",  "mi3",  3,  1},
    /*  4*/ {"3M",  "ma3",  3,  1},
    /*  5*/ {"4J",  "P4",   4,  1},
    /*  6*/ {"d5",  "d5",   5,  1},
    /*  7*/ {"5J",  "P5",   5,  1},
    /*  8*/ {"#5",  "mi6",  6,  1},
    /*  9*/ {"M6",  "ma6",  6,  1},
    /* 10*/ {"7m",  "mi7",  7,  1},
    /* 11*/ {"7M",  "ma7",  7,  1},
    /* 12*/ {"8J",  "P8",   8,  2},
    /* 13*/ {"b9",  "mi9",  9,  2},
    /* 14*/ {"9",   "ma9",  9,  2},
    /* 15*/ {"#9",  "mi10", 10, 2},
    /* 16*/ {"b11", "ma10", 10, 2},
    /* 17*/ {"11",  "P11",  11, 2},
    /* 18*/ {"#11", "d11",  11, 2},
    /* 19*/ {"5",   "P12",  12, 2},
    /* 20*/ {"b13", "mi13", 13, 2},
    /* 21*/ {"13",  "ma13", 13, 2},
    /* 22*/ {"#13", "mi14", 14, 2},
    /* 23*/ {"bI",  "ma14", 14, 2},
};

// ===================================================================
// 4. SCALE MASKS — 10 scale types x 24-bit bitmask
// ===================================================================
//
// Each uint32_t has bit N set if semitone position N is active.
// Bit 0 = P1, Bit 1 = 2m, ..., Bit 11 = 7M, Bit 12 = 8J, ...

static const uint32_t SCALE_MASKS[10] PROGMEM = {
    // Major:          P1 . 2M .  3M 4J .  5J .  M6 .  7M | .  .  9  .  .  11 .  .  .  13 .  .
    0b00000000001000100010101010110101UL,  // 0 Major
    // NatMinor:       P1 . 2M 3m .  4J .  5J #5 .  7m .  | .  .  9  .  .  11 .  .  b13 .  .  .
    0b00000000000101000010101011010101UL,  // 1 Natural minor
    // HarmMinor:      P1 . 2M 3m .  4J .  5J #5 .  .  7M | .  .  9  .  .  11 .  .  .  13 .  .
    0b00000000001000100010100011010101UL,  // 2 Harmonic minor
    // MelodicMinor:   P1 . 2M 3m .  4J .  5J .  M6 .  7M | .  .  9  .  .  11 .  .  .  13 .  .
    0b00000000001000100010101010010101UL,  // 3 Melodic minor
    // Diminished:     P1 . 2M 3m .  4J d5 .  #5 .  7m 7M | .  .  9  .  .  11 .  .  .  13 .  .
    0b00000000001000100010110001110101UL,  // 4 Diminished
    // HarmonicMajor:  P1 . 2M .  3M 4J .  5J #5 .  .  7M | .  .  9  .  .  11 .  .  .  13 .  .
    0b00000000001000100010100110110101UL,  // 5 Harmonic major
    // WholeTone:      P1 . 2M .  3M .  d5 .  #5 .  7m .  | .  .  9  .  .  .  #11 .  .  13 .  .
    0b00000000001001000000010101010101UL,  // 6 Whole tone
    // Augmented:      P1 .  .  3m 3M .  .  5J #5 .  .  7M | .  .  .  #9 .  .  .  5  .  .  #13 .
    0b00000000010010000010000110011001UL,  // 7 Augmented
    // Blues:          P1 .  .  3m .  4J d5 5J .  .  7m .  | .  .  .  #9 .  11 #11 .  .  .  #13 .
    0b00000000010011000010000011101001UL,  // 8 Blues
    // Chromatic:      all bits set
    0b00000000111111111111111111111111UL,  // 9 Chromatic
};

// Modality masks (diatonic filter and pentatonic filter)
// Applied as AND with scale mask to select active positions
static const uint32_t MODALITY_DIATONIC   PROGMEM = 0b00000000001101100010111111111111UL;
static const uint32_t MODALITY_PENTATONIC PROGMEM = 0b00000000001101100010110111101111UL;

// Scale size (number of notes in each parent scale)
static const uint8_t SCALE_SIZES[10] PROGMEM = {
    7, 7, 7, 7, 8, 7, 6, 6, 6, 12
};

// Scale type name strings
static const char STN_0[]  PROGMEM = "major";
static const char STN_1[]  PROGMEM = "natural minor";
static const char STN_2[]  PROGMEM = "harmonic minor";
static const char STN_3[]  PROGMEM = "melodic minor";
static const char STN_4[]  PROGMEM = "diminished";
static const char STN_5[]  PROGMEM = "harmonic major";
static const char STN_6[]  PROGMEM = "whole tone";
static const char STN_7[]  PROGMEM = "augmented";
static const char STN_8[]  PROGMEM = "blues";
static const char STN_9[]  PROGMEM = "chromatic";

static const char* const SCALE_TYPE_NAMES[10] PROGMEM = {
    STN_0, STN_1, STN_2, STN_3, STN_4,
    STN_5, STN_6, STN_7, STN_8, STN_9
};

// ===================================================================
// 5. CHORD FORMULAS — 42 types
// ===================================================================
//
// Each formula stores interval indices (into INTERVAL_TABLE).
// Since interval index = semitone count, these are also semitone offsets.

struct ChordFormula {
    uint8_t intervals[7];  // interval table indices (= semitone offsets)
    uint8_t count;         // how many intervals
};

static const ChordFormula CHORD_FORMULAS[42] PROGMEM = {
    /*  0 M       */ {{0, 4, 7, 0, 0, 0, 0}, 3},
    /*  1 7M      */ {{0, 4, 7, 11, 0, 0, 0}, 4},
    /*  2 6       */ {{0, 4, 7, 9, 0, 0, 0}, 4},
    /*  3 6(9)    */ {{0, 4, 7, 9, 14, 0, 0}, 5},
    /*  4 M9      */ {{0, 4, 7, 11, 14, 0, 0}, 5},
    /*  5 m       */ {{0, 3, 7, 0, 0, 0, 0}, 3},
    /*  6 m7      */ {{0, 3, 7, 10, 0, 0, 0}, 4},
    /*  7 m6      */ {{0, 3, 7, 9, 0, 0, 0}, 4},
    /*  8 m11     */ {{0, 3, 7, 10, 17, 0, 0}, 5},
    /*  9 mM7     */ {{0, 3, 7, 11, 0, 0, 0}, 4},
    /* 10 7       */ {{0, 4, 7, 10, 0, 0, 0}, 4},
    /* 11 9       */ {{0, 4, 7, 10, 14, 0, 0}, 5},
    /* 12 11      */ {{0, 4, 7, 10, 14, 17, 0}, 6},
    /* 13 dim     */ {{0, 3, 6, 0, 0, 0, 0}, 3},
    /* 14 dim7    */ {{0, 3, 6, 9, 0, 0, 0}, 4},
    /* 15 m7(b5)  */ {{0, 3, 6, 10, 0, 0, 0}, 4},
    /* 16 aug     */ {{0, 4, 8, 0, 0, 0, 0}, 3},
    /* 17 7#5     */ {{0, 4, 8, 10, 0, 0, 0}, 4},
    /* 18 7(b5)   */ {{0, 4, 6, 10, 0, 0, 0}, 4},
    /* 19 13      */ {{0, 4, 7, 10, 14, 17, 21}, 7},
    /* 20 13(#11) */ {{0, 4, 7, 10, 14, 18, 21}, 7},
    /* 21 7+5     */ {{0, 4, 8, 10, 0, 0, 0}, 4},
    /* 22 7+9     */ {{0, 4, 7, 10, 15, 0, 0}, 5},
    /* 23 7(b9)   */ {{0, 4, 7, 10, 13, 0, 0}, 5},
    /* 24 7(#11)  */ {{0, 4, 7, 10, 18, 0, 0}, 5},
    /* 25 5       */ {{0, 7, 0, 0, 0, 0, 0}, 2},
    /* 26 add9    */ {{0, 4, 7, 14, 0, 0, 0}, 4},
    /* 27 add2    */ {{0, 2, 4, 7, 0, 0, 0}, 4},
    /* 28 add11   */ {{0, 4, 7, 17, 0, 0, 0}, 4},
    /* 29 add4    */ {{0, 4, 5, 7, 0, 0, 0}, 4},
    /* 30 sus2    */ {{0, 2, 7, 0, 0, 0, 0}, 3},
    /* 31 sus4    */ {{0, 5, 7, 0, 0, 0, 0}, 3},
    /* 32 sus7    */ {{0, 5, 7, 10, 0, 0, 0}, 4},
    /* 33 sus9    */ {{0, 5, 7, 14, 0, 0, 0}, 4},
    /* 34 m13     */ {{0, 3, 7, 10, 14, 17, 21}, 7},
    /* 35 maj13   */ {{0, 4, 7, 11, 14, 18, 21}, 7},
    /* 36 sus     */ {{0, 5, 7, 0, 0, 0, 0}, 3},
    /* 37 m9      */ {{0, 3, 7, 10, 14, 0, 0}, 5},
    /* 38 M7#5    */ {{0, 4, 8, 11, 0, 0, 0}, 4},
    /* 39 m7(11)  */ {{0, 3, 7, 10, 17, 0, 0}, 5},
    /* 40 (b9)    */ {{0, 4, 7, 13, 0, 0, 0}, 4},
    /* 41 (b13)   */ {{0, 4, 7, 20, 0, 0, 0}, 4},
};

// ===================================================================
// 5b. CHORD TYPE ALIASES — sorted for binary search
// ===================================================================

struct ChordTypeAlias {
    char    name[10];      // chord type string
    uint8_t formulaIdx;    // index into CHORD_FORMULAS
};

// Sorted by strcmp order for binary search
static const ChordTypeAlias CHORD_TYPE_MAP[] PROGMEM = {
    {"(9)",     26},
    {"(b13)",   41},
    {"(b9)",    40},
    {"+",       16},
    {"+M7",     38},
    {"11",      12},
    {"13",      19},
    {"13(#11)", 20},
    {"5",       25},
    {"6",        2},
    {"6(9)",     3},
    {"7",       10},
    {"7#5",     17},
    {"7(#11)",  24},
    {"7(9)",    11},
    {"7(b5)",   18},
    {"7(b9)",   23},
    {"7+5",     21},
    {"7+9",     22},
    {"7/9",     11},
    {"7M",       1},
    {"7M(#5)",  38},
    {"M",        0},
    {"M13",     35},
    {"M6",       2},
    {"M7#5",    38},
    {"M9",       4},
    {"add11",   28},
    {"add2",    27},
    {"add4",    29},
    {"add9",    26},
    {"aug",     16},
    {"dim",     13},
    {"dim7",    14},
    {"dom7",    10},
    {"m",        5},
    {"m11",      8},
    {"m13",     34},
    {"m6",       7},
    {"m7",       6},
    {"m7(11)",  39},
    {"m7(b5)",  15},
    {"m7M",      9},
    {"m9",      37},
    {"mM7",      9},
    {"maj",      0},
    {"maj13",   35},
    {"maj7",     1},
    {"maj9",     4},
    {"mi",       5},
    {"min",      5},
    {"min7",     6},
    {"sus",     36},
    {"sus2",    30},
    {"sus4",    31},
    {"sus7",    32},
    {"sus9",    33},
};

static const uint8_t CHORD_TYPE_MAP_SIZE = sizeof(CHORD_TYPE_MAP) / sizeof(CHORD_TYPE_MAP[0]);

// ===================================================================
// 6. TEMPO MARKINGS
// ===================================================================

#if GINGODUINO_HAS_TEMPO

struct TempoMarking {
    char    name[14];
    uint8_t bpm_low;
    uint8_t bpm_high;
    uint8_t bpm_mid;  // typical BPM
};

static const TempoMarking TEMPO_MARKINGS[] PROGMEM = {
    {"Grave",        25,  45,  35},
    {"Largo",        40,  60,  50},
    {"Adagio",       55,  75,  60},
    {"Andante",      73,  108, 80},
    {"Moderato",     108, 120, 114},
    {"Allegretto",   112, 140, 120},
    {"Allegro",      120, 168, 140},
    {"Vivace",       140, 180, 160},
    {"Presto",       168, 200, 184},
    {"Prestissimo",  200, 240, 220},
};

static const uint8_t TEMPO_MARKINGS_SIZE = sizeof(TEMPO_MARKINGS) / sizeof(TEMPO_MARKINGS[0]);

#endif // GINGODUINO_HAS_TEMPO

// ===================================================================
// 7. DURATION NAMES
// ===================================================================

#if GINGODUINO_HAS_DURATION

struct DurationDef {
    char    name[16];
    uint8_t numerator;
    uint8_t denominator;
};

static const DurationDef DURATION_NAMES[] PROGMEM = {
    {"whole",          1,   1},
    {"half",           1,   2},
    {"quarter",        1,   4},
    {"eighth",         1,   8},
    {"sixteenth",      1,  16},
    {"thirty_second",  1,  32},
    {"sixty_fourth",   1,  64},
};

static const uint8_t DURATION_NAMES_SIZE = sizeof(DURATION_NAMES) / sizeof(DURATION_NAMES[0]);

#endif // GINGODUINO_HAS_DURATION

// ===================================================================
// 8. HARMONIC FUNCTION TABLE — function per degree (7 degrees)
// ===================================================================

#if GINGODUINO_HAS_FIELD

// Harmonic functions for Major scale degrees 1-7: T S D T S T D
static const uint8_t HARMONIC_FUNCTIONS_MAJOR[7] PROGMEM = {
    0, 1, 2, 0, 2, 0, 2  // T, S, D, T, D, T, D
};

// Role names (index by degree-1 for major)
static const char ROLE_0[] PROGMEM = "primary";
static const char ROLE_1[] PROGMEM = "primary";
static const char ROLE_2[] PROGMEM = "transitive";
static const char ROLE_3[] PROGMEM = "primary";
static const char ROLE_4[] PROGMEM = "primary";
static const char ROLE_5[] PROGMEM = "relative of I";
static const char ROLE_6[] PROGMEM = "transitive";

static const char* const ROLE_NAMES_MAJOR[7] PROGMEM = {
    ROLE_0, ROLE_1, ROLE_2, ROLE_3, ROLE_4, ROLE_5, ROLE_6
};

#endif // GINGODUINO_HAS_FIELD

// ===================================================================
// 9. MODE DATA (for Scale)
// ===================================================================

#if GINGODUINO_HAS_SCALE

// Mode names for the Major family (7 modes)
static const char MODE_MAJ_1[] PROGMEM = "Ionian";
static const char MODE_MAJ_2[] PROGMEM = "Dorian";
static const char MODE_MAJ_3[] PROGMEM = "Phrygian";
static const char MODE_MAJ_4[] PROGMEM = "Lydian";
static const char MODE_MAJ_5[] PROGMEM = "Mixolydian";
static const char MODE_MAJ_6[] PROGMEM = "Aeolian";
static const char MODE_MAJ_7[] PROGMEM = "Locrian";

static const char* const MODE_NAMES_MAJOR[7] PROGMEM = {
    MODE_MAJ_1, MODE_MAJ_2, MODE_MAJ_3, MODE_MAJ_4,
    MODE_MAJ_5, MODE_MAJ_6, MODE_MAJ_7
};

// Brightness values for Major modes (1=Locrian, 7=Lydian)
static const uint8_t MODE_BRIGHTNESS_MAJOR[7] PROGMEM = {
    5, 3, 1, 7, 6, 2, 0  // Ionian=5, Dorian=3, Phrygian=1, Lydian=7, ...
};

// Mode names for Harmonic Minor (7 modes)
static const char MODE_HM_1[] PROGMEM = "Harmonic Minor";
static const char MODE_HM_2[] PROGMEM = "Locrian nat6";
static const char MODE_HM_3[] PROGMEM = "Ionian #5";
static const char MODE_HM_4[] PROGMEM = "Dorian #4";
static const char MODE_HM_5[] PROGMEM = "Phrygian Dominant";
static const char MODE_HM_6[] PROGMEM = "Lydian #2";
static const char MODE_HM_7[] PROGMEM = "Superlocrian bb7";

static const char* const MODE_NAMES_HARMONIC_MINOR[7] PROGMEM = {
    MODE_HM_1, MODE_HM_2, MODE_HM_3, MODE_HM_4,
    MODE_HM_5, MODE_HM_6, MODE_HM_7
};

// Mode names for Melodic Minor (7 modes)
static const char MODE_MM_1[] PROGMEM = "Melodic Minor";
static const char MODE_MM_2[] PROGMEM = "Dorian b2";
static const char MODE_MM_3[] PROGMEM = "Lydian Augmented";
static const char MODE_MM_4[] PROGMEM = "Lydian Dominant";
static const char MODE_MM_5[] PROGMEM = "Mixolydian b6";
static const char MODE_MM_6[] PROGMEM = "Locrian nat2";
static const char MODE_MM_7[] PROGMEM = "Altered";

static const char* const MODE_NAMES_MELODIC_MINOR[7] PROGMEM = {
    MODE_MM_1, MODE_MM_2, MODE_MM_3, MODE_MM_4,
    MODE_MM_5, MODE_MM_6, MODE_MM_7
};

#endif // GINGODUINO_HAS_SCALE

// ===================================================================
// 10. INTERVAL CONSONANCE & FULL NAMES
// ===================================================================

// Consonance classification by simple semitone (0-11):
// 0=perfect, 1=imperfect, 2=dissonant
static const uint8_t INTERVAL_CONSONANCE[12] PROGMEM = {
    0, // 0  P1  perfect
    2, // 1  2m  dissonant
    2, // 2  2M  dissonant
    1, // 3  3m  imperfect
    1, // 4  3M  imperfect
    0, // 5  4J  perfect
    2, // 6  d5  dissonant
    0, // 7  5J  perfect
    1, // 8  #5  imperfect
    1, // 9  M6  imperfect
    2, // 10 7m  dissonant
    2, // 11 7M  dissonant
};

static const char CONS_0[] PROGMEM = "perfect";
static const char CONS_1[] PROGMEM = "imperfect";
static const char CONS_2[] PROGMEM = "dissonant";

static const char* const CONSONANCE_NAMES[3] PROGMEM = {
    CONS_0, CONS_1, CONS_2
};

// Full interval names in English (24 entries, indexed by semitone 0-23)
static const char IFN_EN_0[]  PROGMEM = "Perfect Unison";
static const char IFN_EN_1[]  PROGMEM = "Minor Second";
static const char IFN_EN_2[]  PROGMEM = "Major Second";
static const char IFN_EN_3[]  PROGMEM = "Minor Third";
static const char IFN_EN_4[]  PROGMEM = "Major Third";
static const char IFN_EN_5[]  PROGMEM = "Perfect Fourth";
static const char IFN_EN_6[]  PROGMEM = "Diminished Fifth";
static const char IFN_EN_7[]  PROGMEM = "Perfect Fifth";
static const char IFN_EN_8[]  PROGMEM = "Minor Sixth";
static const char IFN_EN_9[]  PROGMEM = "Major Sixth";
static const char IFN_EN_10[] PROGMEM = "Minor Seventh";
static const char IFN_EN_11[] PROGMEM = "Major Seventh";
static const char IFN_EN_12[] PROGMEM = "Perfect Octave";
static const char IFN_EN_13[] PROGMEM = "Minor Ninth";
static const char IFN_EN_14[] PROGMEM = "Major Ninth";
static const char IFN_EN_15[] PROGMEM = "Augmented Ninth";
static const char IFN_EN_16[] PROGMEM = "Minor Tenth";
static const char IFN_EN_17[] PROGMEM = "Perfect Eleventh";
static const char IFN_EN_18[] PROGMEM = "Augmented Eleventh";
static const char IFN_EN_19[] PROGMEM = "Perfect Twelfth";
static const char IFN_EN_20[] PROGMEM = "Minor Thirteenth";
static const char IFN_EN_21[] PROGMEM = "Major Thirteenth";
static const char IFN_EN_22[] PROGMEM = "Augmented Thirteenth";
static const char IFN_EN_23[] PROGMEM = "Major Fourteenth";

static const char* const INTERVAL_FULL_NAMES_EN[24] PROGMEM = {
    IFN_EN_0,  IFN_EN_1,  IFN_EN_2,  IFN_EN_3,  IFN_EN_4,  IFN_EN_5,
    IFN_EN_6,  IFN_EN_7,  IFN_EN_8,  IFN_EN_9,  IFN_EN_10, IFN_EN_11,
    IFN_EN_12, IFN_EN_13, IFN_EN_14, IFN_EN_15, IFN_EN_16, IFN_EN_17,
    IFN_EN_18, IFN_EN_19, IFN_EN_20, IFN_EN_21, IFN_EN_22, IFN_EN_23
};

// Full interval names in Portuguese (24 entries, indexed by semitone 0-23)
static const char IFN_PT_0[]  PROGMEM = "Unissono Justo";
static const char IFN_PT_1[]  PROGMEM = "Segunda Menor";
static const char IFN_PT_2[]  PROGMEM = "Segunda Maior";
static const char IFN_PT_3[]  PROGMEM = "Terca Menor";
static const char IFN_PT_4[]  PROGMEM = "Terca Maior";
static const char IFN_PT_5[]  PROGMEM = "Quarta Justa";
static const char IFN_PT_6[]  PROGMEM = "Quinta Diminuta";
static const char IFN_PT_7[]  PROGMEM = "Quinta Justa";
static const char IFN_PT_8[]  PROGMEM = "Sexta Menor";
static const char IFN_PT_9[]  PROGMEM = "Sexta Maior";
static const char IFN_PT_10[] PROGMEM = "Setima Menor";
static const char IFN_PT_11[] PROGMEM = "Setima Maior";
static const char IFN_PT_12[] PROGMEM = "Oitava Justa";
static const char IFN_PT_13[] PROGMEM = "Nona Menor";
static const char IFN_PT_14[] PROGMEM = "Nona Maior";
static const char IFN_PT_15[] PROGMEM = "Nona Aumentada";
static const char IFN_PT_16[] PROGMEM = "Decima Menor";
static const char IFN_PT_17[] PROGMEM = "Decima Primeira Justa";
static const char IFN_PT_18[] PROGMEM = "Decima Primeira Aumentada";
static const char IFN_PT_19[] PROGMEM = "Decima Segunda Justa";
static const char IFN_PT_20[] PROGMEM = "Decima Terceira Menor";
static const char IFN_PT_21[] PROGMEM = "Decima Terceira Maior";
static const char IFN_PT_22[] PROGMEM = "Decima Terceira Aumentada";
static const char IFN_PT_23[] PROGMEM = "Decima Quarta Maior";

static const char* const INTERVAL_FULL_NAMES_PT[24] PROGMEM = {
    IFN_PT_0,  IFN_PT_1,  IFN_PT_2,  IFN_PT_3,  IFN_PT_4,  IFN_PT_5,
    IFN_PT_6,  IFN_PT_7,  IFN_PT_8,  IFN_PT_9,  IFN_PT_10, IFN_PT_11,
    IFN_PT_12, IFN_PT_13, IFN_PT_14, IFN_PT_15, IFN_PT_16, IFN_PT_17,
    IFN_PT_18, IFN_PT_19, IFN_PT_20, IFN_PT_21, IFN_PT_22, IFN_PT_23
};

// ===================================================================
// 11. FRETBOARD TUNING DATA
// ===================================================================

#if GINGODUINO_HAS_FRETBOARD

// Standard tunings stored as MIDI note numbers for each open string.
// Strings ordered from lowest pitch to highest pitch.

// Violao (guitar) — E2 A2 D3 G3 B3 E4
static const uint8_t TUNING_VIOLAO[6] PROGMEM = { 40, 45, 50, 55, 59, 64 };

// Cavaquinho — D4 G4 B4 D5
static const uint8_t TUNING_CAVAQUINHO[4] PROGMEM = { 62, 67, 71, 74 };

// Bandolim (mandolin) — G3 D4 A4 E5
static const uint8_t TUNING_BANDOLIM[4] PROGMEM = { 55, 62, 69, 76 };

// Ukulele — G4 C4 E4 A4
static const uint8_t TUNING_UKULELE[4] PROGMEM = { 67, 60, 64, 69 };

#endif // GINGODUINO_HAS_FRETBOARD

// ===================================================================
// 12. PROGRESSION / TREE DATA — harmonic graph traditions
// ===================================================================

#if GINGODUINO_HAS_TREE

// --------------- Tradition IDs ---------------
// 0 = harmonic_tree (José de Alencar Soares model)
// 1 = jazz

#define PROG_TRADITION_COUNT 2
#define PROG_TRADITION_HARMONIC_TREE 0
#define PROG_TRADITION_JAZZ          1

// --------------- Scale context IDs ---------------
// 0 = major, 1 = minor
// (harmonic minor and melodic minor share the minor context)

#define PROG_CTX_MAJOR 0
#define PROG_CTX_MINOR 1
#define PROG_CTX_COUNT 2

// --------------- Branch name string pool ---------------
// All unique branch names across both traditions.
// Index is the branch ID used in edges and schemas.

static const char BR_00[] PROGMEM = "I";
static const char BR_01[] PROGMEM = "IIm / IV";
static const char BR_02[] PROGMEM = "IIm7(b5) / IIm";
static const char BR_03[] PROGMEM = "IIm7(11) / IV";
static const char BR_04[] PROGMEM = "SUBV7 / IV";
static const char BR_05[] PROGMEM = "V7 / IV";
static const char BR_06[] PROGMEM = "VIm";
static const char BR_07[] PROGMEM = "V7 / IIm";
static const char BR_08[] PROGMEM = "Idim";
static const char BR_09[] PROGMEM = "#Idim";
static const char BR_10[] PROGMEM = "bIIIdim";
static const char BR_11[] PROGMEM = "IV#dim";
static const char BR_12[] PROGMEM = "IV";
static const char BR_13[] PROGMEM = "V7 / V";
static const char BR_14[] PROGMEM = "IIm";
static const char BR_15[] PROGMEM = "IVm";
static const char BR_16[] PROGMEM = "bVI";
static const char BR_17[] PROGMEM = "bVII";
static const char BR_18[] PROGMEM = "IIm7(b5)";
static const char BR_19[] PROGMEM = "II#dim";
static const char BR_20[] PROGMEM = "SUBV7";
static const char BR_21[] PROGMEM = "V7";
static const char BR_22[] PROGMEM = "V7 / VI";
static const char BR_23[] PROGMEM = "V7 / Im";
static const char BR_24[] PROGMEM = "V7 / III";
static const char BR_25[] PROGMEM = "V7 / bIII";
static const char BR_26[] PROGMEM = "V7 / bVI";
// Minor-specific branches
static const char BR_27[] PROGMEM = "Im";
static const char BR_28[] PROGMEM = "IIm7(b5) / Ivm";
static const char BR_29[] PROGMEM = "IVm7 / IVm";
static const char BR_30[] PROGMEM = "V / IVm";
static const char BR_31[] PROGMEM = "V / V";
static const char BR_32[] PROGMEM = "bVI / Im";
static const char BR_33[] PROGMEM = "II / IIm";
static const char BR_34[] PROGMEM = "bV / V";
static const char BR_35[] PROGMEM = "V7 / I";
static const char BR_36[] PROGMEM = "bIII";
static const char BR_37[] PROGMEM = "Vm";
// Jazz-specific
static const char BR_38[] PROGMEM = "IIIm";
static const char BR_39[] PROGMEM = "VIIdim";
static const char BR_40[] PROGMEM = "#IIdim";
static const char BR_41[] PROGMEM = "V7 / V";  // (reuse ID 13 in edges)

#define PROG_BRANCH_COUNT 41

static const char* const PROG_BRANCH_NAMES[PROG_BRANCH_COUNT] PROGMEM = {
    BR_00, BR_01, BR_02, BR_03, BR_04, BR_05, BR_06, BR_07, BR_08, BR_09,
    BR_10, BR_11, BR_12, BR_13, BR_14, BR_15, BR_16, BR_17, BR_18, BR_19,
    BR_20, BR_21, BR_22, BR_23, BR_24, BR_25, BR_26, BR_27, BR_28, BR_29,
    BR_30, BR_31, BR_32, BR_33, BR_34, BR_35, BR_36, BR_37, BR_38, BR_39,
    BR_40
};

// --------------- Tradition names ---------------
static const char TRAD_NAME_0[] PROGMEM = "harmonic_tree";
static const char TRAD_NAME_1[] PROGMEM = "jazz";

static const char* const PROG_TRADITION_NAMES[PROG_TRADITION_COUNT] PROGMEM = {
    TRAD_NAME_0, TRAD_NAME_1
};

// --------------- Directed edges (paths) ---------------
// Each edge: {origin_branch_id, target_branch_id}
// Grouped by [tradition][context]

struct ProgEdge {
    uint8_t origin;
    uint8_t target;
};

// ---- harmonic_tree, major (57 edges) ----
static const ProgEdge HT_MAJOR_EDGES[] PROGMEM = {
    {0, 0},     // I → I
    {0, 1},     // I → IIm / IV
    {0, 2},     // I → IIm7(b5) / IIm
    {0, 3},     // I → IIm7(11) / IV
    {0, 6},     // I → VIm
    {0, 9},     // I → #Idim
    {0, 10},    // I → bIIIdim
    {0, 21},    // I → V7
    {0, 22},    // I → V7 / VI
    {0, 8},     // I → Idim
    {1, 5},     // IIm / IV → V7 / IV
    {2, 7},     // IIm7(b5) / IIm → V7 / IIm
    {3, 4},     // IIm7(11) / IV → SUBV7 / IV
    {4, 12},    // SUBV7 / IV → IV
    {5, 12},    // V7 / IV → IV
    {6, 12},    // VIm → IV
    {7, 14},    // V7 / IIm → IIm
    {7, 13},    // V7 / IIm → V7 / V
    {8, 14},    // Idim → IIm
    {8, 13},    // Idim → V7 / V
    {9, 14},    // #Idim → IIm
    {9, 13},    // #Idim → V7 / V
    {10, 14},   // bIIIdim → IIm
    {10, 13},   // bIIIdim → V7 / V
    {12, 11},   // IV → IV#dim
    {12, 15},   // IV → IVm
    {12, 16},   // IV → bVI
    {12, 17},   // IV → bVII
    {12, 21},   // IV → V7
    {12, 18},   // IV → IIm7(b5)
    {11, 21},   // IV#dim → V7
    {11, 0},    // IV#dim → I
    {13, 14},   // V7 / V → IIm
    {13, 21},   // V7 / V → V7
    {13, 20},   // V7 / V → SUBV7
    {13, 15},   // V7 / V → IVm
    {13, 16},   // V7 / V → bVI
    {13, 17},   // V7 / V → bVII
    {13, 18},   // V7 / V → IIm7(b5)
    {14, 21},   // IIm → V7
    {14, 13},   // IIm → V7 / V
    {14, 15},   // IIm → IVm
    {14, 16},   // IIm → bVI
    {14, 17},   // IIm → bVII
    {14, 18},   // IIm → IIm7(b5)
    {14, 20},   // IIm → SUBV7
    {14, 19},   // IIm → II#dim
    {15, 21},   // IVm → V7
    {15, 0},    // IVm → I
    {16, 0},    // bVI → I
    {16, 21},   // bVI → V7
    {17, 0},    // bVII → I
    {17, 21},   // bVII → V7
    {18, 0},    // IIm7(b5) → I
    {18, 21},   // IIm7(b5) → V7
    {20, 0},    // SUBV7 → I
    {21, 0},    // V7 → I
    {19, 0},    // II#dim → I
};
#define HT_MAJOR_EDGE_COUNT (sizeof(HT_MAJOR_EDGES) / sizeof(HT_MAJOR_EDGES[0]))

// ---- harmonic_tree, minor (25 edges) ----
static const ProgEdge HT_MINOR_EDGES[] PROGMEM = {
    {27, 27},   // Im → Im
    {27, 28},   // Im → IIm7(b5) / Ivm
    {27, 33},   // Im → II / IIm
    {27, 31},   // Im → V / V
    {27, 32},   // Im → bVI / Im
    {27, 35},   // Im → V7 / I
    {27, 17},   // Im → bVII
    {28, 29},   // IIm7(b5) / Ivm → IVm7 / IVm
    {29, 30},   // IVm7 / IVm → V / IVm
    {30, 15},   // V / IVm → IVm
    {31, 35},   // V / V → V7 / I
    {32, 27},   // bVI / Im → Im
    {33, 15},   // II / IIm → IVm
    {15, 35},   // IVm → V7 / I
    {15, 11},   // IVm → IV#dim
    {11, 35},   // IV#dim → V7 / I
    {11, 27},   // IV#dim → Im
    {35, 27},   // V7 / I → Im
    {24, 27},   // V7 / III → Im
    {13, 35},   // V7 / V → V7 / I
    {17, 36},   // bVII → bIII
    {17, 15},   // bVII → IVm
    {36, 15},   // bIII → IVm
    {36, 35},   // bIII → V7 / I
    {37, 27},   // Vm → Im
};
#define HT_MINOR_EDGE_COUNT (sizeof(HT_MINOR_EDGES) / sizeof(HT_MINOR_EDGES[0]))

// ---- jazz, major (32 edges) ----
static const ProgEdge JZ_MAJOR_EDGES[] PROGMEM = {
    {0, 0},     // I → I
    {0, 14},    // I → IIm
    {0, 12},    // I → IV
    {0, 6},     // I → VIm
    {0, 38},    // I → IIIm
    {0, 7},     // I → V7 / IIm
    {0, 5},     // I → V7 / IV
    {0, 22},    // I → V7 / VI
    {0, 21},    // I → V7
    {14, 21},   // IIm → V7
    {21, 0},    // V7 → I
    {6, 14},    // VIm → IIm
    {14, 13},   // IIm → V7 / V
    {38, 6},    // IIIm → VIm
    {12, 21},   // IV → V7
    {12, 15},   // IV → IVm
    {12, 14},   // IV → IIm
    {15, 17},   // IVm → bVII
    {17, 0},    // bVII → I
    {15, 21},   // IVm → V7
    {14, 20},   // IIm → SUBV7
    {20, 0},    // SUBV7 → I
    {7, 14},    // V7 / IIm → IIm
    {5, 12},    // V7 / IV → IV
    {13, 21},   // V7 / V → V7
    {22, 6},    // V7 / VI → VIm
    {9, 14},    // #Idim → IIm
    {40, 38},   // #IIdim → IIIm
    {16, 0},    // bVI → I
    {16, 21},   // bVI → V7
    {18, 21},   // IIm7(b5) → V7
    {39, 0},    // VIIdim → I
};
#define JZ_MAJOR_EDGE_COUNT (sizeof(JZ_MAJOR_EDGES) / sizeof(JZ_MAJOR_EDGES[0]))

// ---- jazz, minor (15 edges) ----
static const ProgEdge JZ_MINOR_EDGES[] PROGMEM = {
    {27, 27},   // Im → Im
    {27, 18},   // Im → IIm7(b5)
    {27, 15},   // Im → IVm
    {27, 16},   // Im → bVI
    {27, 17},   // Im → bVII
    {27, 13},   // Im → V7 / V
    {27, 12},   // Im → IV
    {18, 21},   // IIm7(b5) → V7
    {21, 27},   // V7 → Im
    {15, 21},   // IVm → V7
    {16, 17},   // bVI → bVII
    {17, 27},   // bVII → Im
    {36, 15},   // bIII → IVm
    {13, 21},   // V7 / V → V7
    {12, 21},   // IV → V7
};
#define JZ_MINOR_EDGE_COUNT (sizeof(JZ_MINOR_EDGES) / sizeof(JZ_MINOR_EDGES[0]))

// Edge table index: [tradition][context]
struct ProgEdgeTable {
    const ProgEdge* edges;
    uint8_t         count;
};

static const ProgEdgeTable PROG_EDGE_TABLES[PROG_TRADITION_COUNT][PROG_CTX_COUNT] PROGMEM = {
    // harmonic_tree
    {{ HT_MAJOR_EDGES, (uint8_t)HT_MAJOR_EDGE_COUNT },
     { HT_MINOR_EDGES, (uint8_t)HT_MINOR_EDGE_COUNT }},
    // jazz
    {{ JZ_MAJOR_EDGES, (uint8_t)JZ_MAJOR_EDGE_COUNT },
     { JZ_MINOR_EDGES, (uint8_t)JZ_MINOR_EDGE_COUNT }},
};

// --------------- Schemas ---------------

struct ProgSchema {
    char    name[24];       // schema name
    uint8_t branches[8];    // branch IDs
    uint8_t count;          // number of branches
    uint8_t ctx;            // context: 0=major, 1=minor
};

// harmonic_tree schemas (10)
static const ProgSchema HT_SCHEMAS[] PROGMEM = {
    // Major schemas
    {"descending",          {0, 7, 14, 21, 0, 0, 0, 0},       5, 0},  // I→V7/IIm→IIm→V7→I
    {"ascending",           {0, 5, 12, 21, 0, 0, 0, 0},       5, 0},  // I→V7/IV→IV→V7→I
    {"direct",              {0, 21, 0, 0, 0, 0, 0, 0},        3, 0},  // I→V7→I
    {"extended_descending", {0, 2, 7, 14, 21, 0, 0, 0},       6, 0},  // I→IIm7(b5)/IIm→V7/IIm→IIm→V7→I
    {"subdominant_prep",    {0, 1, 5, 12, 21, 0, 0, 0},       6, 0},  // I→IIm/IV→V7/IV→IV→V7→I
    {"tritone_sub",         {0, 14, 20, 0, 0, 0, 0, 0},       4, 0},  // I→IIm→SUBV7→I
    {"diminished_passing",  {0, 9, 14, 21, 0, 0, 0, 0},       5, 0},  // I→#Idim→IIm→V7→I
    {"modal_borrowing",     {0, 14, 15, 21, 0, 0, 0, 0},      5, 0},  // I→IIm→IVm→V7→I
    // Minor schemas
    {"minor_descending",    {27, 35, 27, 0, 0, 0, 0, 0},      3, 1},  // Im→V7/I→Im
    {"minor_subdominant",   {27, 28, 29, 30, 15, 35, 27, 0},  7, 1},  // Im→IIm7(b5)/Ivm→IVm7/IVm→V/IVm→IVm→V7/I→Im
};
#define HT_SCHEMA_COUNT (sizeof(HT_SCHEMAS) / sizeof(HT_SCHEMAS[0]))

// jazz schemas (7)
static const ProgSchema JZ_SCHEMAS[] PROGMEM = {
    // Major schemas
    {"ii-V-I",              {14, 21, 0, 0, 0, 0, 0, 0},       3, 0},  // IIm→V7→I
    {"turnaround",          {0, 6, 14, 21, 0, 0, 0, 0},       4, 0},  // I→VIm→IIm→V7
    {"backdoor",            {15, 17, 0, 0, 0, 0, 0, 0},       3, 0},  // IVm→bVII→I
    {"tritone_sub",         {14, 20, 0, 0, 0, 0, 0, 0},       3, 0},  // IIm→SUBV7→I
    {"extended_turnaround", {38, 6, 14, 21, 0, 0, 0, 0},      4, 0},  // IIIm→VIm→IIm→V7
    // Minor schemas
    {"minor_ii-V-i",        {18, 21, 27, 0, 0, 0, 0, 0},      3, 1},  // IIm7(b5)→V7→Im
    {"minor_backdoor",      {16, 17, 27, 0, 0, 0, 0, 0},      3, 1},  // bVI→bVII→Im
};
#define JZ_SCHEMA_COUNT (sizeof(JZ_SCHEMAS) / sizeof(JZ_SCHEMAS[0]))

// Schema table index: [tradition]
struct ProgSchemaTable {
    const ProgSchema* schemas;
    uint8_t           count;
};

static const ProgSchemaTable PROG_SCHEMA_TABLES[PROG_TRADITION_COUNT] PROGMEM = {
    { HT_SCHEMAS, (uint8_t)HT_SCHEMA_COUNT },
    { JZ_SCHEMAS, (uint8_t)JZ_SCHEMA_COUNT },
};

// --------------- Branch resolution helpers ---------------

// Branch types for resolve():
// 0 = diatonic (degree from roman numeral)
// 1 = secondary dominant (V7 / target)
// 2 = tritone sub (SUBV7)
// 3 = diminished passing (#Idim, bIIIdim, etc.)
// 4 = borrowed/modal (IVm, bVI, bVII)
// 5 = compound (IIm / IV, etc. — resolved via degree)

// Degree mapping for branches that map directly to field degrees
// 0xFF = requires special handling
struct BranchDegreeInfo {
    uint8_t degree;     // 1-7 scale degree (0xFF = special)
    uint8_t isSeventh;  // 1 = use seventh chord, 0 = triad
};

// Per-branch degree info (for branches that have a direct degree mapping)
static const BranchDegreeInfo BRANCH_DEGREE[PROG_BRANCH_COUNT] PROGMEM = {
    /*  0 I            */ {1, 0},
    /*  1 IIm / IV     */ {0xFF, 0},  // special
    /*  2 IIm7(b5)/IIm */ {0xFF, 0},  // special
    /*  3 IIm7(11)/IV  */ {0xFF, 0},  // special
    /*  4 SUBV7 / IV   */ {0xFF, 0},  // special
    /*  5 V7 / IV      */ {0xFF, 0},  // secondary dom
    /*  6 VIm          */ {6, 0},
    /*  7 V7 / IIm     */ {0xFF, 0},  // secondary dom
    /*  8 Idim         */ {0xFF, 0},  // diminished
    /*  9 #Idim        */ {0xFF, 0},  // diminished
    /* 10 bIIIdim      */ {0xFF, 0},  // diminished
    /* 11 IV#dim       */ {0xFF, 0},  // diminished
    /* 12 IV           */ {4, 0},
    /* 13 V7 / V       */ {0xFF, 0},  // secondary dom
    /* 14 IIm          */ {2, 0},
    /* 15 IVm          */ {0xFF, 0},  // borrowed
    /* 16 bVI          */ {0xFF, 0},  // borrowed
    /* 17 bVII         */ {0xFF, 0},  // borrowed
    /* 18 IIm7(b5)     */ {2, 1},     // ii half-dim (use seventh)
    /* 19 II#dim       */ {0xFF, 0},  // diminished
    /* 20 SUBV7        */ {0xFF, 0},  // tritone sub
    /* 21 V7           */ {5, 1},
    /* 22 V7 / VI      */ {0xFF, 0},  // secondary dom
    /* 23 V7 / Im      */ {0xFF, 0},  // secondary dom
    /* 24 V7 / III     */ {0xFF, 0},  // secondary dom
    /* 25 V7 / bIII    */ {0xFF, 0},  // secondary dom
    /* 26 V7 / bVI     */ {0xFF, 0},  // secondary dom
    /* 27 Im           */ {1, 0},
    /* 28 IIm7(b5)/Ivm */ {0xFF, 0},  // special
    /* 29 IVm7 / IVm   */ {0xFF, 0},  // special
    /* 30 V / IVm       */ {0xFF, 0},  // secondary dom
    /* 31 V / V         */ {0xFF, 0},  // secondary dom
    /* 32 bVI / Im      */ {0xFF, 0},  // special
    /* 33 II / IIm      */ {0xFF, 0},  // secondary dom
    /* 34 bV / V        */ {0xFF, 0},  // special
    /* 35 V7 / I        */ {5, 1},     // dominant (same as V7 in minor)
    /* 36 bIII          */ {0xFF, 0},  // borrowed
    /* 37 Vm            */ {5, 0},     // v minor
    /* 38 IIIm          */ {3, 0},
    /* 39 VIIdim        */ {7, 0},
    /* 40 #IIdim        */ {0xFF, 0},  // diminished
};

#endif // GINGODUINO_HAS_TREE

// ===================================================================
// PROGMEM read helpers
// ===================================================================

/// Read a PROGMEM string into a buffer
inline void readPgmStr(char* dest, const char* pgmSrc, uint8_t maxLen) {
    uint8_t i = 0;
    char c;
    while (i < maxLen - 1 && (c = (char)pgm_read_byte(pgmSrc + i)) != '\0') {
        dest[i++] = c;
    }
    dest[i] = '\0';
}

/// Binary search in sorted PROGMEM EnharmonicEntry array
inline int8_t findEnharmonic(const char* input) {
    int8_t lo = 0;
    int8_t hi = (int8_t)(ENHARMONIC_MAP_SIZE - 1);
    while (lo <= hi) {
        int8_t mid = (lo + hi) / 2;
        char buf[5];
        readPgmStr(buf, ENHARMONIC_MAP[mid].input, sizeof(buf));
        int cmp = strcmp(input, buf);
        if (cmp == 0) return mid;
        if (cmp < 0) hi = mid - 1;
        else lo = mid + 1;
    }
    return -1;
}

/// Binary search in sorted PROGMEM ChordTypeAlias array
inline int8_t findChordType(const char* typeName) {
    int8_t lo = 0;
    int8_t hi = (int8_t)(CHORD_TYPE_MAP_SIZE - 1);
    while (lo <= hi) {
        int8_t mid = (lo + hi) / 2;
        char buf[10];
        readPgmStr(buf, CHORD_TYPE_MAP[mid].name, sizeof(buf));
        int cmp = strcmp(typeName, buf);
        if (cmp == 0) return mid;
        if (cmp < 0) hi = mid - 1;
        else lo = mid + 1;
    }
    return -1;
}

/// Read a ChordFormula from PROGMEM
inline void readChordFormula(uint8_t idx, uint8_t* intervals, uint8_t* count) {
    *count = pgm_read_byte(&CHORD_FORMULAS[idx].count);
    for (uint8_t i = 0; i < *count; i++) {
        intervals[i] = pgm_read_byte(&CHORD_FORMULAS[idx].intervals[i]);
    }
}

/// Read chromatic note name from PROGMEM
inline void readChromaticName(uint8_t semitone, char* dest, uint8_t maxLen) {
    readPgmStr(dest, CHROMATIC_NAMES[semitone % 12], maxLen);
}

} // namespace data
} // namespace gingoduino

#endif // GINGODUINO_PROGMEM_H

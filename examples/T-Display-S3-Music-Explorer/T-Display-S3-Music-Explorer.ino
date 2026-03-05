// Gingoduino — T-Display-S3 Music Theory Explorer + Audio Synthesis
//
// SPDX-License-Identifier: MIT
//
// Interactive demo for LilyGo T-Display-S3 (ESP32-S3 + ST7789 170x320).
// Features real-time audio synthesis via I2S → PCM5102 DAC.
//
// Navigate with the 2 built-in buttons:
//   LEFT  (GPIO 0 / BOOT): switch page (stops audio)
//   RIGHT (GPIO 14):       cycle items within page (plays audio)
//                          OR play/stop sequence (on Sequence page)
//
// Pages (each plays audio when navigating):
//   1. Note Explorer       — 12 chromatic notes with frequency/MIDI
//   2. Interval Explorer   — 12 simple intervals (plays as 2-voice)
//   3. Chord Explorer      — common chords with notes and intervals
//   4. Scale Explorer      — scales/modes (plays as arpeggio)
//   5. Harmonic Field      — triads + sevenths with T/S/D (plays progression)
//   6. Fretboard           — guitar diagram with chord/scale overlay
//   7. Sequence            — timeline visualization with PLAY/STOP button
//
// Audio Hardware: PCM5102A I2S DAC (3.5mm stereo output)
//   GPIO 11: I2S BCK  (bit clock)   — via R15 470ohm → PCM5102 pin 13
//   GPIO 12: I2S DIN  (data out)    — via R14 470ohm → PCM5102 pin 14
//   GPIO 13: I2S LRCK (word select) — via R13 470ohm → PCM5102 pin 15
//   MCLK: not used (PCM5102 SCK pin tied to GND = 3-wire mode)
//
// REQUIRES:
//   - TFT_eSPI library configured for T-Display-S3
//     In TFT_eSPI/User_Setup_Select.h:
//       - Comment out:   #include <User_Setup.h>
//       - Uncomment:     #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>
//   - T-Display-S3 MIDI Shield v1.1 (with PCM5102 DAC module)

#include <TFT_eSPI.h>
#include <Gingoduino.h>
#include <driver/i2s.h>
#include "mapping.h"

using namespace gingoduino;

// ---------------------------------------------------------------------------
// Audio Engine — polyphonic voices with individual ADSR envelopes
// ---------------------------------------------------------------------------
#define MAX_VOICES    4
#define MAX_SCHEDULED 32

enum EnvStage : uint8_t { ENV_OFF, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE };

struct Voice {
    float freq;          // Hz (0 = inactive)
    float phase;         // oscillator phase
    float envLevel;      // current envelope level (0..1)
    EnvStage envStage;   // ADSR stage
    uint32_t durSamples; // auto note-off after N samples (0 = manual)
    uint32_t elapsed;    // samples since note-on
};

struct ADSRParams {
    float attackRate;    // envelope increment per sample
    float decayRate;     // envelope decrement per sample
    float sustainLevel;  // sustain level (0..1)
    float releaseRate;   // envelope decrement per sample
};

struct ScheduledEvent {
    float freq;              // frequency for this voice
    uint8_t voiceSlot;       // which voice slot to use (0..3)
    uint32_t triggerAt;      // sample clock when to start
    uint32_t durSamples;     // duration in samples
};

struct AudioEngine {
    Voice voices[MAX_VOICES];
    ADSRParams adsr;

    // Non-blocking scheduler (accessed from both cores — use schedCount as gate)
    ScheduledEvent schedule[MAX_SCHEDULED];
    volatile uint8_t schedCount;
    uint32_t sampleClock;

    // Control
    volatile bool clearAll;   // signal to stop everything
};

static AudioEngine engine;
static TaskHandle_t audioTaskHandle = NULL;

// Pre-allocated global sequence (avoids stack overflow in drawSequencePage)
static GingoSequence g_seq(GingoTempo(120), GingoTimeSig(4, 4));

// ADSR presets (converted to per-sample rates in initADSR)
static void setADSR(float attackMs, float decayMs, float sustainLvl, float releaseMs) {
    engine.adsr.attackRate  = 1.0f / (attackMs  * SAMPLE_RATE / 1000.0f);
    engine.adsr.decayRate   = (1.0f - sustainLvl) / (decayMs * SAMPLE_RATE / 1000.0f);
    engine.adsr.sustainLevel = sustainLvl;
    engine.adsr.releaseRate = sustainLvl / (releaseMs * SAMPLE_RATE / 1000.0f);
}

static uint32_t msToSamples(uint32_t ms) {
    return (uint32_t)((uint64_t)ms * SAMPLE_RATE / 1000);
}

/// Add an event to the schedule (thread-safe: writes data first, then bumps count).
static bool schedAdd(const ScheduledEvent& ev) {
    uint8_t idx = engine.schedCount;
    if (idx >= MAX_SCHEDULED) return false;
    engine.schedule[idx] = ev;
    engine.schedCount = idx + 1;
    return true;
}

/// Remove event at index s by swapping with last (called only from audioTask).
static void schedRemove(uint8_t s) {
    uint8_t last = engine.schedCount - 1;
    if (s != last) {
        engine.schedule[s] = engine.schedule[last];
    }
    engine.schedCount = last;
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------
TFT_eSPI tft = TFT_eSPI();

// Colors (RGB565)
#define C_BG        0x1082  // dark grey-blue
#define C_HEADER    0x2945  // slightly lighter
#define C_ACCENT    0x07FF  // cyan
#define C_NOTE      0xFFE0  // yellow
#define C_INTERVAL  0x07FF  // cyan
#define C_CHORD     0xFBE0  // orange
#define C_SCALE     0x87F0  // green
#define C_FIELD     0xF81F  // magenta
#define C_FRETBOARD 0xFD20  // warm orange
#define C_SEQUENCE  0xB71C  // purple
#define C_TEXT      0xFFFF  // white
#define C_DIM       0x8410  // grey
#define C_TONIC     0x07E0  // green
#define C_SUBDOM    0xFFE0  // yellow
#define C_DOMINANT  0xF800  // red
#define C_PERFECT   0x07E0  // green  — perfect consonance
#define C_IMPERFECT 0x04DF  // blue   — imperfect consonance
#define C_DISSONANT 0xF800  // red    — dissonance
#define C_STRING    0xBDF7  // light grey — fretboard strings
#define C_FRET_LINE 0x6B4D  // medium grey — frets
#define C_NUT       0xFFFF  // white — nut (fret 0)
#define C_DOT       0x07E0  // green — fretted positions
#define C_OPEN_STR  0x07FF  // cyan — open string
#define C_MUTED_STR 0xF800  // red — muted string
#define C_EVT_NOTE  0xFFE0  // yellow
#define C_EVT_CHORD 0xFBE0  // orange
#define C_EVT_REST  0x4208  // dark grey
#define C_BEAT_LINE 0x6B4D  // grey

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
enum Page {
    PAGE_NOTE, PAGE_INTERVAL, PAGE_CHORD, PAGE_SCALE,
    PAGE_FIELD, PAGE_FRETBOARD, PAGE_SEQUENCE, PAGE_COUNT
};

static Page    currentPage = PAGE_NOTE;
static uint8_t itemIdx     = 0;
static bool    needRedraw  = true;
static uint8_t lastItemIdx = 0;
static Page    lastPage = PAGE_NOTE;

// Debounce
static unsigned long lastBtnLeft  = 0;
static unsigned long lastBtnRight = 0;
#define DEBOUNCE_MS 250

// Audio control flags
static bool audioEnabled = true;
static bool seqPlaying = false;

// ---------------------------------------------------------------------------
// Data for cycling
// ---------------------------------------------------------------------------
static const char* NOTE_NAMES[12] = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
};

static const char* CHORD_LIST[] = {
    "CM", "Cm", "C7", "C7M", "Cm7", "Cdim", "Caug",
    "DM", "Dm7", "D7", "EM", "Em", "FM", "F7M",
    "GM", "G7", "Am", "Am7", "Bdim", "Bm7(b5)"
};
static const uint8_t CHORD_LIST_SIZE = sizeof(CHORD_LIST) / sizeof(CHORD_LIST[0]);

struct ScaleEntry {
    const char* tonic;
    ScaleType   type;
    const char* label;
};
static const ScaleEntry SCALE_LIST[] = {
    {"C",  SCALE_MAJOR,          "C Major"},
    {"A",  SCALE_NATURAL_MINOR,  "A Natural Minor"},
    {"A",  SCALE_HARMONIC_MINOR, "A Harmonic Minor"},
    {"A",  SCALE_MELODIC_MINOR,  "A Melodic Minor"},
    {"C",  SCALE_DIMINISHED,     "C Diminished"},
    {"C",  SCALE_WHOLE_TONE,     "C Whole Tone"},
    {"A",  SCALE_BLUES,          "A Blues"},
};
static const uint8_t SCALE_LIST_SIZE = sizeof(SCALE_LIST) / sizeof(SCALE_LIST[0]);

struct FieldEntry {
    const char* tonic;
    ScaleType   type;
    const char* label;
};
static const FieldEntry FIELD_LIST[] = {
    {"C",  SCALE_MAJOR,          "C Major"},
    {"G",  SCALE_MAJOR,          "G Major"},
    {"D",  SCALE_MAJOR,          "D Major"},
    {"A",  SCALE_NATURAL_MINOR,  "A Minor"},
    {"E",  SCALE_NATURAL_MINOR,  "E Minor"},
    {"D",  SCALE_NATURAL_MINOR,  "D Minor"},
};
static const uint8_t FIELD_LIST_SIZE = sizeof(FIELD_LIST) / sizeof(FIELD_LIST[0]);

// Fretboard cycling: chords then scales
static const char* FRET_CHORD_LIST[] = {
    "CM", "Dm", "Em", "FM", "GM", "Am", "Bdim",
    "C7", "D7", "E7", "G7", "A7", "Cm", "Dm7", "Em7", "Am7"
};
static const uint8_t FRET_CHORD_SIZE = sizeof(FRET_CHORD_LIST) / sizeof(FRET_CHORD_LIST[0]);

struct FretScaleEntry {
    const char* tonic;
    ScaleType   type;
    const char* label;
};
static const FretScaleEntry FRET_SCALE_LIST[] = {
    {"C", SCALE_MAJOR,          "C Major"},
    {"A", SCALE_NATURAL_MINOR,  "A Minor"},
    {"G", SCALE_MAJOR,          "G Major"},
    {"E", SCALE_NATURAL_MINOR,  "E Minor"},
    {"A", SCALE_BLUES,          "A Blues"},
};
static const uint8_t FRET_SCALE_SIZE = sizeof(FRET_SCALE_LIST) / sizeof(FRET_SCALE_LIST[0]);
static const uint8_t FRET_TOTAL_SIZE = FRET_CHORD_SIZE + FRET_SCALE_SIZE;

// Sequence presets
struct SeqPreset {
    const char* name;
    uint16_t    bpm;
    uint8_t     beatsPerBar;
    uint8_t     beatUnit;
};
static const SeqPreset SEQ_PRESETS[] = {
    {"I-IV-V-I in C",   120, 4, 4},
    {"ii-V-I Jazz",     110, 4, 4},
    {"Simple Melody",   100, 4, 4},
    {"Rests & Notes",   120, 4, 4},
    {"Bossa Pattern",   140, 4, 4},
};
static const uint8_t SEQ_PRESETS_SIZE = sizeof(SEQ_PRESETS) / sizeof(SEQ_PRESETS[0]);

// ---------------------------------------------------------------------------
// Drawing helpers
// ---------------------------------------------------------------------------

void drawHeader(const char* title, uint16_t color) {
    tft.fillRect(0, 0, SCR_W, 28, C_HEADER);
    tft.setTextColor(color, C_HEADER);
    tft.setTextSize(2);
    tft.setCursor(8, 6);
    tft.print(title);

    // Page indicator dots
    for (uint8_t i = 0; i < PAGE_COUNT; i++) {
        int x = SCR_W - 12 - (PAGE_COUNT - 1 - i) * 14;
        if (i == (uint8_t)currentPage) {
            tft.fillCircle(x, 14, 4, color);
        } else {
            tft.drawCircle(x, 14, 4, C_DIM);
        }
    }

    tft.drawFastHLine(0, 28, SCR_W, color);
}

void drawFooter() {
    tft.drawFastHLine(0, SCR_H - 18, SCR_W, C_DIM);
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(6, SCR_H - 13);
    tft.print("[BOOT] page   [KEY] next");
}

void clearContent() {
    tft.fillRect(0, 29, SCR_W, SCR_H - 29 - 18, C_BG);
}

/// Draw key signature string (e.g. "2#", "3b", "0").
void drawSignature(int x, int y, int8_t sig) {
    tft.setCursor(x, y);
    if (sig > 0) {
        tft.print(sig);
        tft.print("#");
    } else if (sig < 0) {
        tft.print(-sig);
        tft.print("b");
    } else {
        tft.print("0");
    }
}

/// Return RGB565 color for an interval's consonance class.
uint16_t consonanceColor(const GingoInterval& iv) {
    char buf[12];
    iv.consonance(buf, sizeof(buf));
    if (buf[0] == 'p') return C_PERFECT;
    if (buf[0] == 'i') return C_IMPERFECT;
    return C_DISSONANT;
}

// ---------------------------------------------------------------------------
// Page: Note Explorer
// ---------------------------------------------------------------------------

void drawNotePage() {
    drawHeader("Note Explorer", C_NOTE);
    clearContent();
    drawFooter();

    GingoNote note(NOTE_NAMES[itemIdx % 12]);

    // Big note name
    tft.setTextColor(C_NOTE, C_BG);
    tft.setTextSize(5);
    tft.setCursor(20, 42);
    tft.print(note.name());

    // Natural form
    tft.setTextSize(2);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(20, 96);
    tft.print("nat: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(note.natural());

    // Info column on the right
    int rx = 150;
    tft.setTextSize(2);

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 40);
    tft.print("Semi: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(note.semitone());

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 62);
    tft.print("MIDI: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(note.midiNumber(4));

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 84);
    tft.print("Freq: ");
    tft.setTextColor(C_ACCENT, C_BG);
    tft.print(note.frequency(4), 1);

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 106);
    tft.print("Oct4  Hz");

    // Chromatic bar at bottom (highlight current)
    int barY = 126;
    tft.setTextSize(1);
    for (uint8_t i = 0; i < 12; i++) {
        int bx = 8 + i * 25;
        if (i == itemIdx % 12) {
            tft.fillRoundRect(bx, barY, 23, 16, 3, C_NOTE);
            tft.setTextColor(0x0000, C_NOTE);
        } else {
            tft.drawRoundRect(bx, barY, 23, 16, 3, C_DIM);
            tft.setTextColor(C_DIM, C_BG);
        }
        tft.setCursor(bx + (strlen(NOTE_NAMES[i]) == 1 ? 8 : 4), barY + 4);
        tft.print(NOTE_NAMES[i]);
    }
}

// ---------------------------------------------------------------------------
// Page: Interval Explorer
// ---------------------------------------------------------------------------

void drawIntervalPage() {
    drawHeader("Interval Explorer", C_INTERVAL);
    clearContent();
    drawFooter();

    uint8_t semi = itemIdx % 12;
    GingoInterval iv((uint8_t)semi);

    // Label (large)
    char labelBuf[8];
    iv.label(labelBuf, sizeof(labelBuf));
    tft.setTextColor(C_INTERVAL, C_BG);
    tft.setTextSize(3);
    tft.setCursor(12, 36);
    tft.print(labelBuf);

    // Full name EN
    char nameBuf[32];
    iv.fullName(nameBuf, sizeof(nameBuf));
    tft.setTextSize(2);
    tft.setTextColor(C_TEXT, C_BG);
    tft.setCursor(12, 64);
    tft.print(nameBuf);

    // Full name PT
    char namePtBuf[32];
    iv.fullNamePt(namePtBuf, sizeof(namePtBuf));
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 86);
    tft.print(namePtBuf);

    // Right column: consonance, semitones, degree
    int rx = 180;

    // Consonance box
    char consBuf[12];
    iv.consonance(consBuf, sizeof(consBuf));
    uint16_t consColor = consonanceColor(iv);
    tft.fillRoundRect(rx, 36, 130, 18, 3, consColor);
    tft.setTextColor(0x0000, consColor);
    tft.setTextSize(1);
    int consLen = strlen(consBuf);
    tft.setCursor(rx + 65 - (consLen * 3), 41);
    tft.print(consBuf);

    // Semitones
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 60);
    tft.print("Semitones: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(iv.semitones());

    // Degree
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 74);
    tft.print("Degree: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(iv.degree());

    // Anglo-Saxon
    char angloBuf[8];
    iv.angloSaxon(angloBuf, sizeof(angloBuf));
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(rx, 88);
    tft.print("Anglo: ");
    tft.setTextColor(C_ACCENT, C_BG);
    tft.print(angloBuf);

    // Chromatic bar at bottom (highlight current with consonance color)
    static const char* IV_LABELS[12] = {
        "P1", "m2", "M2", "m3", "M3", "P4",
        "TT", "P5", "m6", "M6", "m7", "M7"
    };
    int barY = 114;
    tft.setTextSize(1);
    for (uint8_t i = 0; i < 12; i++) {
        int bx = 8 + i * 25;
        if (i == semi) {
            tft.fillRoundRect(bx, barY, 23, 16, 3, consColor);
            tft.setTextColor(0x0000, consColor);
        } else {
            tft.drawRoundRect(bx, barY, 23, 16, 3, C_DIM);
            tft.setTextColor(C_DIM, C_BG);
        }
        int lblLen = strlen(IV_LABELS[i]);
        tft.setCursor(bx + 12 - (lblLen * 3), barY + 4);
        tft.print(IV_LABELS[i]);
    }
}

// ---------------------------------------------------------------------------
// Page: Chord Explorer
// ---------------------------------------------------------------------------

void drawChordPage() {
    drawHeader("Chord Explorer", C_CHORD);
    clearContent();
    drawFooter();

    uint8_t idx = itemIdx % CHORD_LIST_SIZE;
    GingoChord chord(CHORD_LIST[idx]);

    // Chord name
    tft.setTextSize(3);
    tft.setTextColor(C_CHORD, C_BG);
    tft.setCursor(12, 36);
    tft.print(chord.name());

    // Root + type
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 64);
    tft.print("root: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(chord.root().name());
    tft.setTextColor(C_DIM, C_BG);
    tft.print("  type: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(chord.type());

    // Notes as boxes
    GingoNote notes[7];
    uint8_t n = chord.notes(notes, 7);

    tft.setTextSize(2);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 80);
    tft.print("Notes:");

    for (uint8_t i = 0; i < n; i++) {
        int bx = 12 + i * 42;
        int by = 98;
        tft.fillRoundRect(bx, by, 38, 22, 4, C_CHORD);
        tft.setTextColor(0x0000, C_CHORD);
        tft.setTextSize(2);
        int nameLen = strlen(notes[i].name());
        tft.setCursor(bx + (nameLen == 1 ? 12 : 5), by + 3);
        tft.print(notes[i].name());
    }

    // Intervals (GingoInterval objects with full names)
    GingoInterval ivs[7];
    uint8_t ni = chord.intervals(ivs, 7);

    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 126);
    tft.print("Intervals: ");

    for (uint8_t i = 0; i < ni; i++) {
        char lbl[8];
        ivs[i].label(lbl, sizeof(lbl));
        uint16_t ivColor = consonanceColor(ivs[i]);
        tft.setTextColor(ivColor, C_BG);
        if (i > 0) tft.print("  ");
        tft.print(lbl);
    }
}

// ---------------------------------------------------------------------------
// Page: Scale Explorer
// ---------------------------------------------------------------------------

void drawScalePage() {
    drawHeader("Scale Explorer", C_SCALE);
    clearContent();
    drawFooter();

    uint8_t idx = itemIdx % SCALE_LIST_SIZE;
    const ScaleEntry& se = SCALE_LIST[idx];
    GingoScale scale(se.tonic, se.type);

    // Scale label + signature
    tft.setTextSize(2);
    tft.setTextColor(C_SCALE, C_BG);
    tft.setCursor(12, 36);
    tft.print(se.label);

    // Signature badge
    int8_t sig = scale.signature();
    tft.setTextSize(1);
    tft.setTextColor(C_ACCENT, C_BG);
    tft.setCursor(12 + strlen(se.label) * 12 + 8, 40);
    drawSignature(tft.getCursorX(), tft.getCursorY(), sig);

    // Mode name + quality
    char modeBuf[22];
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 54);
    tft.print("mode: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(scale.modeName(modeBuf, sizeof(modeBuf)));
    tft.setTextColor(C_DIM, C_BG);
    tft.print("  quality: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(scale.quality());

    // Notes as a row
    GingoNote notes[12];
    uint8_t n = scale.notes(notes, 12);

    tft.setTextSize(2);
    int startX = 12;
    int boxW = (n <= 7) ? 38 : 30;
    int gap = (n <= 7) ? 4 : 3;

    for (uint8_t i = 0; i < n; i++) {
        int bx = startX + i * (boxW + gap);
        int by = 68;

        uint16_t boxColor = (i == 0) ? C_SCALE : C_HEADER;
        uint16_t textColor = (i == 0) ? 0x0000 : C_TEXT;

        tft.fillRoundRect(bx, by, boxW, 26, 4, boxColor);
        tft.setTextColor(textColor, boxColor);
        int nameLen = strlen(notes[i].name());
        tft.setCursor(bx + (boxW / 2) - (nameLen * 6), by + 5);
        tft.print(notes[i].name());

        // Degree number below
        tft.setTextSize(1);
        tft.setTextColor(C_DIM, C_BG);
        tft.setCursor(bx + (boxW / 2) - 3, by + 30);
        tft.print(i + 1);
        tft.setTextSize(2);
    }

    // Brightness bar (7 segments)
    uint8_t bright = scale.brightness();
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 108);
    tft.print("Brightness: ");

    for (uint8_t i = 0; i < 7; i++) {
        int bx = 84 + i * 14;
        if (i < bright) {
            tft.fillRect(bx, 106, 12, 10, C_SCALE);
        } else {
            tft.drawRect(bx, 106, 12, 10, C_DIM);
        }
    }

    tft.setTextColor(C_TEXT, C_BG);
    tft.setCursor(84 + 7 * 14 + 6, 108);
    tft.print(bright);

    // Relative + parallel
    GingoScale rel = scale.relative();
    GingoScale par = scale.parallel();
    char relMode[22], parMode[22];

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 122);
    tft.print("Relative: ");
    tft.setTextColor(C_ACCENT, C_BG);
    tft.print(rel.tonic().name());
    tft.print(" ");
    tft.print(rel.modeName(relMode, sizeof(relMode)));

    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 134);
    tft.print("Parallel: ");
    tft.setTextColor(C_ACCENT, C_BG);
    tft.print(par.tonic().name());
    tft.print(" ");
    tft.print(par.modeName(parMode, sizeof(parMode)));
}

// ---------------------------------------------------------------------------
// Page: Harmonic Field
// ---------------------------------------------------------------------------

void drawFieldPage() {
    drawHeader("Harmonic Field", C_FIELD);
    clearContent();
    drawFooter();

    uint8_t idx = itemIdx % FIELD_LIST_SIZE;
    const FieldEntry& fe = FIELD_LIST[idx];
    GingoField field(fe.tonic, fe.type);

    // Field label + signature
    tft.setTextSize(2);
    tft.setTextColor(C_FIELD, C_BG);
    tft.setCursor(12, 36);
    tft.print(fe.label);

    int8_t sig = field.signature();
    tft.setTextSize(1);
    tft.setTextColor(C_ACCENT, C_BG);
    tft.setCursor(12 + strlen(fe.label) * 12 + 8, 40);
    drawSignature(tft.getCursorX(), tft.getCursorY(), sig);

    // Triads row
    GingoChord triads[7];
    uint8_t nt = field.chords(triads, 7);

    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 56);
    tft.print("Triads:");

    for (uint8_t i = 0; i < nt && i < 7; i++) {
        int bx = 8 + i * 44;
        int by = 68;

        // Color by function
        uint8_t func = field.function(i + 1);
        uint16_t funcColor;
        switch (func) {
            case FUNC_TONIC:       funcColor = C_TONIC;    break;
            case FUNC_SUBDOMINANT: funcColor = C_SUBDOM;   break;
            case FUNC_DOMINANT:    funcColor = C_DOMINANT;  break;
            default:               funcColor = C_DIM;       break;
        }

        // Chord box
        tft.fillRoundRect(bx, by, 42, 20, 3, C_HEADER);
        tft.setTextSize(1);
        tft.setTextColor(C_TEXT, C_HEADER);
        int nameLen = strlen(triads[i].name());
        tft.setCursor(bx + 21 - (nameLen * 3), by + 6);
        tft.print(triads[i].name());

        // Role text below chord
        char roleBuf[12];
        field.role(i + 1, roleBuf, sizeof(roleBuf));
        tft.setTextColor(funcColor, C_BG);
        int roleLen = strlen(roleBuf);
        tft.setCursor(bx + 21 - (roleLen * 3), by + 23);
        tft.print(roleBuf);

        // Function dot
        tft.fillCircle(bx + 21, by - 4, 3, funcColor);
    }

    // Sevenths row
    GingoChord sevs[7];
    uint8_t ns = field.sevenths(sevs, 7);

    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 100);
    tft.print("Sevenths:");

    for (uint8_t i = 0; i < ns && i < 7; i++) {
        int bx = 8 + i * 44;
        int by = 112;
        tft.fillRoundRect(bx, by, 42, 20, 3, C_HEADER);
        tft.setTextSize(1);
        tft.setTextColor(C_ACCENT, C_HEADER);
        int nameLen = strlen(sevs[i].name());
        int cx = bx + 21 - (nameLen * 3);
        if (cx < bx + 1) cx = bx + 1;
        tft.setCursor(cx, by + 6);
        tft.print(sevs[i].name());
    }

    // Legend
    int ly = SCR_H - 32;
    tft.setTextSize(1);
    tft.fillCircle(12, ly + 3, 3, C_TONIC);
    tft.setTextColor(C_TEXT, C_BG);
    tft.setCursor(20, ly);
    tft.print("T");
    tft.fillCircle(42, ly + 3, 3, C_SUBDOM);
    tft.setCursor(50, ly);
    tft.print("S");
    tft.fillCircle(72, ly + 3, 3, C_DOMINANT);
    tft.setCursor(80, ly);
    tft.print("D");
}

// ---------------------------------------------------------------------------
// Page: Fretboard Explorer
// ---------------------------------------------------------------------------

// Fretboard geometry constants
#define FB_X        36
#define FB_Y        64
#define FB_FRETS    5
#define FRET_W      52
#define STRING_GAP  10
#define FB_STRINGS  6

/// Draw the fretboard grid (strings, frets, nut, labels).
void drawFretGrid(uint8_t baseFret) {
    uint8_t totalStrings = FB_STRINGS;
    int fbW = FB_FRETS * FRET_W;
    int fbH = (totalStrings - 1) * STRING_GAP;

    // Strings (horizontal) — top to bottom: E2 A2 D3 G3 B3 E4
    static const char* STRING_NAMES[6] = {"E", "A", "D", "G", "B", "E"};
    for (uint8_t s = 0; s < totalStrings; s++) {
        int y = FB_Y + s * STRING_GAP;
        tft.drawFastHLine(FB_X, y, fbW, C_STRING);

        // String name label
        tft.setTextSize(1);
        tft.setTextColor(C_DIM, C_BG);
        tft.setCursor(FB_X - 14, y - 3);
        tft.print(STRING_NAMES[s]);
    }

    // Nut (thick line at fret 0)
    if (baseFret == 0) {
        tft.fillRect(FB_X - 2, FB_Y, 4, fbH, C_NUT);
    }

    // Frets (vertical)
    for (uint8_t f = 1; f <= FB_FRETS; f++) {
        int x = FB_X + f * FRET_W;
        tft.drawFastVLine(x, FB_Y, fbH, C_FRET_LINE);
    }

    // Fret numbers below
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    for (uint8_t f = 0; f < FB_FRETS; f++) {
        int x = FB_X + f * FRET_W + FRET_W / 2;
        int num = baseFret + f + (baseFret == 0 ? 1 : 1);
        tft.setCursor(x - 3, FB_Y + fbH + 6);
        tft.print(num);
    }
}

void drawFretboardPage() {
    drawHeader("Fretboard", C_FRETBOARD);
    clearContent();
    drawFooter();

    uint8_t idx = itemIdx % FRET_TOTAL_SIZE;
    bool isScale = (idx >= FRET_CHORD_SIZE);

    GingoFretboard fb = GingoFretboard::violao();

    if (!isScale) {
        // Chord mode
        GingoChord chord(FRET_CHORD_LIST[idx]);
        GingoFingering fg;
        bool ok = fb.fingering(chord, 0, fg);

        // Title
        tft.setTextSize(2);
        tft.setTextColor(C_FRETBOARD, C_BG);
        tft.setCursor(12, 36);
        tft.print(chord.name());

        tft.setTextSize(1);
        tft.setTextColor(C_DIM, C_BG);
        tft.setCursor(12, 54);
        if (ok) {
            tft.print("score: ");
            tft.setTextColor(C_TEXT, C_BG);
            tft.print(fg.score);
            tft.setTextColor(C_DIM, C_BG);
            tft.print("  notes: ");
            tft.setTextColor(C_TEXT, C_BG);
            tft.print(fg.numNotes);
        } else {
            tft.print("(no fingering found)");
        }

        // Draw grid
        drawFretGrid(0);

        // Draw fingering positions
        if (ok) {
            for (uint8_t s = 0; s < fg.numStrings; s++) {
                int y = FB_Y + s * STRING_GAP;

                if (fg.strings[s].action == STRING_OPEN) {
                    // "O" left of nut
                    tft.setTextSize(1);
                    tft.setTextColor(C_OPEN_STR, C_BG);
                    tft.setCursor(FB_X - 8, y - 3);
                    tft.print("O");
                } else if (fg.strings[s].action == STRING_MUTED) {
                    // "X" left of nut
                    tft.setTextSize(1);
                    tft.setTextColor(C_MUTED_STR, C_BG);
                    tft.setCursor(FB_X - 8, y - 3);
                    tft.print("X");
                } else if (fg.strings[s].action == STRING_FRETTED) {
                    uint8_t f = fg.strings[s].fret;
                    if (f >= 1 && f <= FB_FRETS) {
                        int x = FB_X + (f - 1) * FRET_W + FRET_W / 2;
                        tft.fillCircle(x, y, 4, C_DOT);
                    }
                }
            }
        }

        // Mode indicator
        tft.setTextSize(1);
        tft.setTextColor(C_FRETBOARD, C_BG);
        tft.setCursor(SCR_W - 48, 36);
        tft.print("CHORD");
    } else {
        // Scale mode
        uint8_t scaleIdx = idx - FRET_CHORD_SIZE;
        const FretScaleEntry& se = FRET_SCALE_LIST[scaleIdx];
        GingoScale scale(se.tonic, se.type);

        // Title
        tft.setTextSize(2);
        tft.setTextColor(C_FRETBOARD, C_BG);
        tft.setCursor(12, 36);
        tft.print(se.label);

        // Positions count
        GingoFretPos positions[48];
        uint8_t npos = fb.scalePositions(scale, positions, 48, 0, FB_FRETS);

        tft.setTextSize(1);
        tft.setTextColor(C_DIM, C_BG);
        tft.setCursor(12, 54);
        tft.print(npos);
        tft.print(" positions");

        // Draw grid
        drawFretGrid(0);

        // Draw scale positions
        uint8_t tonicSemi = GingoNote(se.tonic).semitone();
        for (uint8_t i = 0; i < npos; i++) {
            uint8_t s = positions[i].string;
            uint8_t f = positions[i].fret;
            int y = FB_Y + s * STRING_GAP;

            bool isTonic = (positions[i].midi % 12 == tonicSemi);

            if (f == 0) {
                // Open string: small circle left of nut
                tft.setTextSize(1);
                tft.setTextColor(isTonic ? C_FRETBOARD : C_OPEN_STR, C_BG);
                tft.setCursor(FB_X - 8, y - 3);
                tft.print("O");
            } else if (f <= FB_FRETS) {
                int x = FB_X + (f - 1) * FRET_W + FRET_W / 2;
                uint16_t dotColor = isTonic ? C_FRETBOARD : C_DOT;
                tft.fillCircle(x, y, isTonic ? 5 : 3, dotColor);
            }
        }

        // Mode indicator
        tft.setTextSize(1);
        tft.setTextColor(C_SCALE, C_BG);
        tft.setCursor(SCR_W - 48, 36);
        tft.print("SCALE");
    }
}

// ---------------------------------------------------------------------------
// Page: Sequence Explorer
// ---------------------------------------------------------------------------

/// Build a preset sequence.
void buildPresetSequence(uint8_t presetIdx, GingoSequence& seq) {
    seq.clear();

    GingoDuration quarter("quarter");
    GingoDuration half("half");
    GingoDuration eighth("eighth");

    switch (presetIdx) {
        case 0:  // I-IV-V-I in C
            seq.add(GingoEvent::chordEvent(GingoChord("CM"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("FM"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("GM"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("CM"), quarter));
            break;
        case 1:  // ii-V-I Jazz
            seq.add(GingoEvent::chordEvent(GingoChord("Dm7"), half));
            seq.add(GingoEvent::chordEvent(GingoChord("G7"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("C7M"), quarter));
            break;
        case 2:  // Simple Melody
            seq.add(GingoEvent::noteEvent(GingoNote("C"), quarter, 4));
            seq.add(GingoEvent::noteEvent(GingoNote("D"), quarter, 4));
            seq.add(GingoEvent::noteEvent(GingoNote("E"), quarter, 4));
            seq.add(GingoEvent::noteEvent(GingoNote("F"), quarter, 4));
            seq.add(GingoEvent::noteEvent(GingoNote("G"), quarter, 4));
            break;
        case 3:  // Rests & Notes
            seq.add(GingoEvent::noteEvent(GingoNote("C"), eighth, 4));
            seq.add(GingoEvent::rest(eighth));
            seq.add(GingoEvent::noteEvent(GingoNote("E"), eighth, 4));
            seq.add(GingoEvent::rest(eighth));
            seq.add(GingoEvent::noteEvent(GingoNote("G"), eighth, 4));
            break;
        case 4:  // Bossa Pattern
            seq.add(GingoEvent::chordEvent(GingoChord("CM"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("Dm7"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("G7"), quarter));
            seq.add(GingoEvent::chordEvent(GingoChord("CM"), quarter));
            break;
    }
}

void drawSequencePage() {
    drawHeader("Sequence", C_SEQUENCE);
    clearContent();
    drawFooter();

    uint8_t idx = itemIdx % SEQ_PRESETS_SIZE;
    const SeqPreset& sp = SEQ_PRESETS[idx];

    g_seq.setTempo(GingoTempo(sp.bpm));
    g_seq.setTimeSignature(GingoTimeSig(sp.beatsPerBar, sp.beatUnit));
    buildPresetSequence(idx, g_seq);

    // Preset name
    tft.setTextSize(2);
    tft.setTextColor(C_SEQUENCE, C_BG);
    tft.setCursor(12, 36);
    tft.print(sp.name);

    // Metadata line
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, 56);
    tft.print(sp.bpm);
    tft.print(" BPM  ");
    tft.print(sp.beatsPerBar);
    tft.print("/");
    tft.print(sp.beatUnit);

    char classBuf[12];
    g_seq.timeSignature().classification(classBuf, sizeof(classBuf));
    tft.print(" ");
    tft.print(classBuf);

    // Stats
    float totalB = g_seq.totalBeats();
    float totalS = g_seq.totalSeconds();
    float bars = g_seq.barCount();

    tft.setTextColor(C_TEXT, C_BG);
    tft.setCursor(180, 56);
    tft.print(totalB, 1);
    tft.print(" beats  ");
    tft.print(bars, 1);
    tft.print(" bars");

    // Timeline visualization
    int tlY = 76;
    int tlH = 36;
    int tlX = 12;
    int tlW = SCR_W - 24;

    // Beat grid lines
    if (totalB > 0) {
        float pixelsPerBeat = (float)tlW / totalB;
        for (uint8_t b = 0; b <= (uint8_t)totalB; b++) {
            int x = tlX + (int)(b * pixelsPerBeat);
            tft.drawFastVLine(x, tlY, tlH + 14, C_BEAT_LINE);

            // Beat number below
            if (b < (uint8_t)totalB) {
                tft.setTextSize(1);
                tft.setTextColor(C_DIM, C_BG);
                tft.setCursor(x + 2, tlY + tlH + 16);
                tft.print(b + 1);
            }
        }
    }

    // Event blocks
    float xOffset = 0;
    for (uint8_t i = 0; i < g_seq.size(); i++) {
        const GingoEvent& evt = g_seq.at(i);
        float beats = evt.duration().beats();
        int blockX = tlX + (int)(xOffset / totalB * tlW);
        int blockW = (int)(beats / totalB * tlW) - 2;
        if (blockW < 6) blockW = 6;

        uint16_t blockColor;
        switch (evt.type()) {
            case EVENT_NOTE:  blockColor = C_EVT_NOTE;  break;
            case EVENT_CHORD: blockColor = C_EVT_CHORD; break;
            default:          blockColor = C_EVT_REST;   break;
        }

        tft.fillRoundRect(blockX, tlY + 2, blockW, tlH - 4, 4, blockColor);

        // Label inside block
        tft.setTextSize(1);
        tft.setTextColor(0x0000, blockColor);
        int labelX = blockX + 3;
        int labelY = tlY + tlH / 2 - 3;

        if (evt.type() == EVENT_NOTE) {
            tft.setCursor(labelX, labelY);
            tft.print(evt.note().name());
        } else if (evt.type() == EVENT_CHORD) {
            tft.setCursor(labelX, labelY);
            tft.print(evt.chord().name());
        } else {
            tft.setCursor(labelX, labelY);
            tft.print("rest");
        }

        xOffset += beats;
    }

    // Bottom stats
    tft.setTextSize(1);
    tft.setTextColor(C_DIM, C_BG);
    tft.setCursor(12, SCR_H - 34);
    tft.print("Events: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(g_seq.size());
    tft.setTextColor(C_DIM, C_BG);
    tft.print("  Duration: ");
    tft.setTextColor(C_TEXT, C_BG);
    tft.print(totalB, 1);
    tft.print(" beats  ");
    tft.print(totalS, 1);
    tft.print("s");

    // PLAY button indicator
    tft.setTextSize(2);
    int playBtnX = SCR_W - 70;
    int playBtnY = SCR_H - 40;
    uint16_t btnColor = seqPlaying ? C_ACCENT : C_SEQUENCE;
    tft.fillRoundRect(playBtnX, playBtnY, 60, 24, 4, btnColor);
    tft.setTextColor(0x0000, btnColor);
    tft.setCursor(playBtnX + 10, playBtnY + 4);
    tft.print(seqPlaying ? "STOP" : "PLAY");
}

// ---------------------------------------------------------------------------
// Audio Engine — polyphonic synthesizer with per-voice ADSR
// ---------------------------------------------------------------------------

/// Process ADSR envelope for one voice, returns current level.
static inline float processADSR(Voice& v, const ADSRParams& adsr) {
    switch (v.envStage) {
        case ENV_ATTACK:
            v.envLevel += adsr.attackRate;
            if (v.envLevel >= 1.0f) {
                v.envLevel = 1.0f;
                v.envStage = ENV_DECAY;
            }
            break;
        case ENV_DECAY:
            v.envLevel -= adsr.decayRate;
            if (v.envLevel <= adsr.sustainLevel) {
                v.envLevel = adsr.sustainLevel;
                v.envStage = ENV_SUSTAIN;
            }
            break;
        case ENV_SUSTAIN:
            v.envLevel = adsr.sustainLevel;
            break;
        case ENV_RELEASE:
            v.envLevel -= adsr.releaseRate;
            if (v.envLevel <= 0.0f) {
                v.envLevel = 0.0f;
                v.envStage = ENV_OFF;
                v.freq = 0.0f;
            }
            break;
        case ENV_OFF:
        default:
            v.envLevel = 0.0f;
            break;
    }
    return v.envLevel;
}

/// Start a voice with given frequency.
static void voiceNoteOn(uint8_t slot, float freq, uint32_t durSamples = 0) {
    if (slot >= MAX_VOICES) return;
    Voice& v = engine.voices[slot];
    v.freq = freq;
    v.phase = 0.0f;
    v.envLevel = 0.0f;
    v.envStage = ENV_ATTACK;
    v.durSamples = durSamples;
    v.elapsed = 0;
}

/// Trigger release on a voice.
static void voiceNoteOff(uint8_t slot) {
    if (slot >= MAX_VOICES) return;
    Voice& v = engine.voices[slot];
    if (v.envStage != ENV_OFF) {
        v.envStage = ENV_RELEASE;
    }
}

/// Release all voices.
static void releaseAllVoices() {
    for (uint8_t i = 0; i < MAX_VOICES; i++) {
        voiceNoteOff(i);
    }
}

/// Kill all voices and clear schedule immediately.
static void killAllAudio() {
    engine.clearAll = true;
}

/// FreeRTOS task: audio synthesis on core 1.
void audioTask(void* pvParameters) {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = I2S_BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0,
    };

    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = I2S_BCK,
        .ws_io_num = I2S_LRCK,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE,
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    Serial.printf("I2S init: %s (BCK=%d WS=%d DOUT=%d)\n",
                  err == ESP_OK ? "OK" : "FAIL", I2S_BCK, I2S_LRCK, I2S_DOUT);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);

    const int FRAMES = I2S_BUFFER_SIZE;
    int16_t* buffer = (int16_t*)malloc(FRAMES * 2 * sizeof(int16_t));

    while (true) {
        // Handle clearAll signal
        if (engine.clearAll) {
            for (uint8_t v = 0; v < MAX_VOICES; v++) {
                engine.voices[v].envStage = ENV_OFF;
                engine.voices[v].envLevel = 0.0f;
                engine.voices[v].freq = 0.0f;
            }
            engine.schedCount = 0;
            engine.clearAll = false;
        }

        // Process scheduled events
        for (uint8_t s = 0; s < engine.schedCount; ) {
            ScheduledEvent& ev = engine.schedule[s];
            if (engine.sampleClock >= ev.triggerAt) {
                voiceNoteOn(ev.voiceSlot, ev.freq, ev.durSamples);
                schedRemove(s);
            } else {
                s++;
            }
        }

        // Generate audio samples
        for (int i = 0; i < FRAMES; i++) {
            float mixed = 0.0f;

            for (uint8_t v = 0; v < MAX_VOICES; v++) {
                Voice& voice = engine.voices[v];
                if (voice.envStage == ENV_OFF) continue;

                // Oscillator (sine wave)
                voice.phase += (2.0f * M_PI * voice.freq) / SAMPLE_RATE;
                if (voice.phase > 2.0f * M_PI) voice.phase -= 2.0f * M_PI;
                float osc = sinf(voice.phase);

                // ADSR envelope
                float env = processADSR(voice, engine.adsr);
                mixed += osc * env;

                // Auto note-off (timed duration)
                voice.elapsed++;
                if (voice.durSamples > 0 && voice.elapsed >= voice.durSamples) {
                    if (voice.envStage != ENV_RELEASE && voice.envStage != ENV_OFF) {
                        voice.envStage = ENV_RELEASE;
                    }
                }
            }

            // Soft-clip to avoid harsh distortion on chords
            float limited = tanhf(mixed * 0.7f);
            int16_t sample16 = (int16_t)(limited * 30000.0f);
            buffer[i * 2]     = sample16;
            buffer[i * 2 + 1] = sample16;

            engine.sampleClock++;
        }

        size_t bytes_written = 0;
        i2s_write(I2S_NUM_0, buffer, FRAMES * 2 * sizeof(int16_t),
                  &bytes_written, portMAX_DELAY);
    }

    free(buffer);
    vTaskDelete(NULL);
}

// ---------------------------------------------------------------------------
// Audio API — all non-blocking, returns immediately
// ---------------------------------------------------------------------------

/// Stop all audio smoothly (release all voices, clear schedule).
void stopAudio() {
    engine.schedCount = 0;
    releaseAllVoices();
}

/// Play a single note (sustains until stopAudio or new note).
void playNote(const GingoNote& note, uint8_t octave = 4, uint32_t durationMs = 0) {
    releaseAllVoices();
    uint32_t dur = durationMs > 0 ? msToSamples(durationMs) : 0;
    voiceNoteOn(0, note.frequency(octave), dur);
}

/// Play a chord with strum effect (small delay between voices).
void playChord(const GingoChord& chord, uint8_t octave = 4,
               uint32_t durationMs = 0, uint32_t strumMs = 20) {
    releaseAllVoices();
    GingoNote notes[7];
    uint8_t count = chord.notes(notes, 7);
    if (count > MAX_VOICES) count = MAX_VOICES;

    uint32_t dur = durationMs > 0 ? msToSamples(durationMs) : 0;
    uint32_t strumSamples = msToSamples(strumMs);

    // First voice plays immediately
    voiceNoteOn(0, notes[0].frequency(octave), dur);

    // Remaining voices scheduled with strum delay on separate slots
    for (uint8_t i = 1; i < count; i++) {
        if (engine.schedCount < MAX_SCHEDULED) {
            ScheduledEvent ev = {};
            ev.freq = notes[i].frequency(octave);
            ev.voiceSlot = i;
            ev.triggerAt = engine.sampleClock + strumSamples * i;
            ev.durSamples = dur;
            schedAdd(ev);
        }
    }
}

/// Schedule an arpeggio (non-blocking: notes play over time).
void scheduleArpeggio(const GingoNote* notes, uint8_t count,
                      uint8_t octave, uint32_t noteMs, uint32_t gapMs) {
    killAllAudio();
    delay(5); // let clearAll propagate

    uint32_t noteSamples = msToSamples(noteMs);
    uint32_t stepSamples = msToSamples(noteMs + gapMs);

    for (uint8_t i = 0; i < count && engine.schedCount < MAX_SCHEDULED; i++) {
        ScheduledEvent ev = {};
        ev.freq = notes[i].frequency(octave);
        ev.voiceSlot = 0;  // arpeggio: always slot 0 (one at a time)
        ev.triggerAt = engine.sampleClock + stepSamples * i;
        ev.durSamples = noteSamples;
        schedAdd(ev);
    }
}

/// Schedule a chord progression (non-blocking).
void scheduleProgression(const GingoChord* chords, uint8_t count,
                         uint8_t octave, uint32_t chordMs, uint32_t strumMs) {
    killAllAudio();
    delay(5);

    uint32_t chordSamples = msToSamples(chordMs);
    uint32_t strumSamples = msToSamples(strumMs);

    for (uint8_t c = 0; c < count; c++) {
        GingoNote notes[7];
        uint8_t ncount = chords[c].notes(notes, 7);
        if (ncount > MAX_VOICES) ncount = MAX_VOICES;

        uint32_t baseTime = engine.sampleClock + chordSamples * c;

        for (uint8_t v = 0; v < ncount && engine.schedCount < MAX_SCHEDULED; v++) {
            ScheduledEvent ev = {};
            ev.freq = notes[v].frequency(octave);
            ev.voiceSlot = v;
            ev.triggerAt = baseTime + strumSamples * v;
            ev.durSamples = chordSamples - strumSamples * v;
            schedAdd(ev);
        }
    }
}

/// Schedule a sequence playback (non-blocking).
void scheduleSequence(const GingoSequence& seq) {
    killAllAudio();
    delay(5);

    const GingoTempo& tempo = seq.tempo();
    uint32_t offset = 0;

    for (uint8_t i = 0; i < seq.size(); i++) {
        const GingoEvent& evt = seq.at(i);
        float beats = evt.duration().beats();
        uint32_t durMs = (uint32_t)(tempo.msPerBeat() * beats);
        uint32_t durSamples = msToSamples(durMs);

        if (evt.type() == EVENT_NOTE) {
            if (engine.schedCount < MAX_SCHEDULED) {
                ScheduledEvent ev = {};
                ev.freq = evt.note().frequency(evt.octave());
                ev.voiceSlot = 0;
                ev.triggerAt = engine.sampleClock + offset;
                ev.durSamples = durSamples;
                schedAdd(ev);
            }
        } else if (evt.type() == EVENT_CHORD) {
            GingoNote notes[7];
            uint8_t ncount = evt.chord().notes(notes, 7);
            if (ncount > MAX_VOICES) ncount = MAX_VOICES;

            uint32_t strumSamples = msToSamples(15);
            for (uint8_t v = 0; v < ncount && engine.schedCount < MAX_SCHEDULED; v++) {
                ScheduledEvent ev = {};
                ev.freq = notes[v].frequency(evt.octave());
                ev.voiceSlot = v;
                ev.triggerAt = engine.sampleClock + offset + strumSamples * v;
                ev.durSamples = durSamples - strumSamples * v;
                schedAdd(ev);
            }
        }
        // Rests: just advance offset, no event scheduled

        offset += durSamples;
    }

    seqPlaying = true;
}

/// Trigger audio for the current page/item (ALL non-blocking).
void triggerAudioForCurrentPage() {
    if (!audioEnabled) return;

    switch (currentPage) {
        case PAGE_NOTE: {
            setADSR(5, 50, 0.7, 100);
            GingoNote note(NOTE_NAMES[itemIdx % 12]);
            playNote(note, 4);
            break;
        }
        case PAGE_INTERVAL: {
            setADSR(5, 50, 0.7, 100);
            stopAudio();
            GingoNote c("C");
            GingoNote other(c.transpose(itemIdx % 12));
            // Strum: root immediately, interval note after 30ms
            voiceNoteOn(0, c.frequency(4), 0);
            if (engine.schedCount < MAX_SCHEDULED) {
                ScheduledEvent ev = {};
                ev.freq = other.frequency(4);
                ev.voiceSlot = 1;
                ev.triggerAt = engine.sampleClock + msToSamples(30);
                ev.durSamples = 0;
                schedAdd(ev);
            }
            break;
        }
        case PAGE_CHORD: {
            setADSR(10, 150, 0.6, 300);
            uint8_t idx = itemIdx % CHORD_LIST_SIZE;
            GingoChord chord(CHORD_LIST[idx]);
            playChord(chord, 3, 1500, 30);
            break;
        }
        case PAGE_SCALE: {
            setADSR(5, 60, 0.6, 100);
            uint8_t idx = itemIdx % SCALE_LIST_SIZE;
            const ScaleEntry& se = SCALE_LIST[idx];
            GingoScale scale(se.tonic, se.type);
            GingoNote notes[12];
            uint8_t count = scale.notes(notes, 12);
            scheduleArpeggio(notes, count, 4, 400, 30);
            break;
        }
        case PAGE_FIELD: {
            setADSR(10, 100, 0.6, 200);
            uint8_t idx = itemIdx % FIELD_LIST_SIZE;
            const FieldEntry& fe = FIELD_LIST[idx];
            GingoField field(fe.tonic, fe.type);
            GingoChord chords[7];
            uint8_t count = field.chords(chords, 7);
            scheduleProgression(chords, count, 4, 900, 25);
            break;
        }
        case PAGE_FRETBOARD: {
            uint8_t idx = itemIdx % FRET_TOTAL_SIZE;
            bool isScale = (idx >= FRET_CHORD_SIZE);
            if (!isScale) {
                // Chord mode
                setADSR(10, 150, 0.6, 300);
                GingoChord chord(FRET_CHORD_LIST[idx]);
                playChord(chord, 3, 1500, 30);
            } else {
                // Scale mode
                setADSR(5, 40, 0.5, 80);
                uint8_t scaleIdx = idx - FRET_CHORD_SIZE;
                const FretScaleEntry& se = FRET_SCALE_LIST[scaleIdx];
                GingoScale scale(se.tonic, se.type);
                GingoNote notes[12];
                uint8_t count = scale.notes(notes, 12);
                scheduleArpeggio(notes, count, 4, 350, 30);
            }
            break;
        }
        case PAGE_SEQUENCE:
            // Handled separately via RIGHT button
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

void setup() {
    // Buttons
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);

    // Backlight
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    // Battery power support
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

    // Display init
    tft.init();
    tft.setRotation(1);  // landscape
    tft.fillScreen(C_BG);

    Serial.begin(115200);
    Serial.println("Gingoduino T-Display-S3 Explorer + Audio Synthesis");

    // Initialize ADSR defaults
    setADSR(5, 50, 0.7, 100);
    memset((void*)&engine, 0, sizeof(engine));

    // Start audio synthesis task on core 1
    xTaskCreatePinnedToCore(
        audioTask,           // function
        "AudioTask",         // name
        8192,                // stack size (bytes) — larger for float math
        NULL,                // parameters
        2,                   // priority (higher than loop)
        &audioTaskHandle,    // task handle
        1                    // core 1
    );
}

void loop() {
    unsigned long now = millis();
    bool itemChanged = false;

    // LEFT button — switch page, stop any playing audio
    if (digitalRead(BTN_LEFT) == LOW && (now - lastBtnLeft) > DEBOUNCE_MS) {
        lastBtnLeft = now;
        killAllAudio();
        seqPlaying = false;
        currentPage = (Page)(((uint8_t)currentPage + 1) % PAGE_COUNT);
        itemIdx = 0;
        lastItemIdx = 0;
        lastPage = currentPage;
        needRedraw = true;
    }

    // RIGHT button behavior depends on current page
    if (digitalRead(BTN_RIGHT) == LOW && (now - lastBtnRight) > DEBOUNCE_MS) {
        lastBtnRight = now;

        if (currentPage == PAGE_SEQUENCE) {
            if (seqPlaying) {
                killAllAudio();
                seqPlaying = false;
            } else {
                uint8_t idx = itemIdx % SEQ_PRESETS_SIZE;
                const SeqPreset& sp = SEQ_PRESETS[idx];
                g_seq.setTempo(GingoTempo(sp.bpm));
                g_seq.setTimeSignature(GingoTimeSig(sp.beatsPerBar, sp.beatUnit));
                buildPresetSequence(idx, g_seq);
                setADSR(5, 50, 0.7, 100);
                scheduleSequence(g_seq);
            }
            needRedraw = true;
        } else {
            itemIdx++;
            needRedraw = true;
        }
    }

    // Detect item change for audio trigger
    if (needRedraw && currentPage != PAGE_SEQUENCE) {
        if (itemIdx != lastItemIdx || currentPage != lastPage) {
            lastItemIdx = itemIdx;
            lastPage = currentPage;
            itemChanged = true;
        }
    }

    // Auto-detect sequence end
    if (seqPlaying && engine.schedCount == 0) {
        bool anyActive = false;
        for (uint8_t v = 0; v < MAX_VOICES; v++) {
            if (engine.voices[v].envStage != ENV_OFF) { anyActive = true; break; }
        }
        if (!anyActive) {
            seqPlaying = false;
            needRedraw = true;
        }
    }

    // REDRAW FIRST (fast, ~20ms) — then trigger audio
    if (needRedraw) {
        needRedraw = false;
        switch (currentPage) {
            case PAGE_NOTE:      drawNotePage();       break;
            case PAGE_INTERVAL:  drawIntervalPage();   break;
            case PAGE_CHORD:     drawChordPage();      break;
            case PAGE_SCALE:     drawScalePage();      break;
            case PAGE_FIELD:     drawFieldPage();      break;
            case PAGE_FRETBOARD: drawFretboardPage();  break;
            case PAGE_SEQUENCE:  drawSequencePage();    break;
            default: break;
        }

        // Trigger audio AFTER display update
        if (itemChanged) {
            triggerAudioForCurrentPage();
        }
    }

    delay(10);
}

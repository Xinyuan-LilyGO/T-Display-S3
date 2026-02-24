// Example: Piano Visualizer + Music Theory + Synthesizer
//
// Designed for MIDI-25 keyboards (e.g. Arturia Minilab 25).
// Shows all 25 keys (C to C, 2 octaves) on the ST7789 display of the T-Display S3.
// Highlights pressed keys in real time with music theory analysis (Gingoduino).
// Plays notes via the onboard PCM5102A DAC.
//
// Anti-tearing: full-screen sprite (double-buffered in PSRAM).
//
// Dependencies:
//   - ESP32_Host_MIDI (this library)
//   - Gingoduino v0.2.2+  (https://github.com/sauloverissimo/gingoduino)
//   - LovyanGFX

#include <Arduino.h>
#include <ESP32_Host_MIDI.h>
#include <GingoAdapter.h>
#include "PianoDisplay.h"
#include "SynthEngine.h"
#include "mapping.h"

using namespace gingoduino;

// ── Global instances ──────────────────────────────────────────────────────────
SynthEngine synth;

// ── Active notes state ────────────────────────────────────────────────────────
static bool activeNotes[128]     = {};
static bool prevActiveNotes[128] = {};

// ── Persistent music analysis ─────────────────────────────────────────────────
static PianoInfo info = {};

// ── Chord type → full name ────────────────────────────────────────────────────
static const char* chordTypeName(const char* chordName) {
    if (!chordName || !*chordName) return "";
    const char* t = chordName + 1;
    if (*t == '#' || *t == 'b') t++;
    struct { const char* code; const char* en; } MAP[] = {
        {"M",       "Major"},       {"m",       "Minor"},
        {"7",       "Dominant 7"},  {"7M",      "Major 7"},
        {"m7",      "Minor 7"},     {"dim",     "Diminished"},
        {"dim7",    "Dim. 7"},      {"m7(b5)",  "Half-dim."},
        {"+",       "Augmented"},   {"sus2",    "Sus 2"},
        {"sus4",    "Sus 4"},       {"M9",      "Major 9"},
        {"m9",      "Minor 9"},     {nullptr,   nullptr}
    };
    for (int i = 0; MAP[i].code; i++) if (strcmp(t, MAP[i].code) == 0) return MAP[i].en;
    return t;
}

// ── Gingoduino analysis ───────────────────────────────────────────────────────
static void analyzeNotes() {
    uint8_t midiNotes[GingoAdapter::MAX_CHORD_NOTES];
    uint8_t count = 0;
    for (int n = 0; n < 128 && count < GingoAdapter::MAX_CHORD_NOTES; n++) {
        if (activeNotes[n]) midiNotes[count++] = (uint8_t)n;
    }

    memset(&info, 0, sizeof(info));
    info.noteCount = count;
    if (count == 0) return;

    GingoNote gingoNotes[GingoAdapter::MAX_CHORD_NOTES];
    uint8_t n = GingoAdapter::midiToGingoNotes(midiNotes, count, gingoNotes);

    info.rootMidi  = midiNotes[0];
    int8_t rootOctave = GingoNote::octaveFromMIDI(midiNotes[0]);
    info.rootFreq  = gingoNotes[0].frequency(rootOctave);

    char noteStr[64] = "";
    for (uint8_t i = 0; i < n; i++) {
        if (i) strcat(noteStr, "  ");
        char buf[8];
        int8_t oct = GingoNote::octaveFromMIDI(midiNotes[i]);
        snprintf(buf, sizeof(buf), "%s%d", gingoNotes[i].name(), (int)oct);
        strcat(noteStr, buf);
    }
    strncpy(info.noteStr, noteStr, sizeof(info.noteStr) - 1);

    if (n == 1) return;

    if (n == 2) {
        GingoInterval iv(gingoNotes[0], gingoNotes[1]);
        iv.label(info.intervalLabel, sizeof(info.intervalLabel));
        iv.fullName(info.intervalName, sizeof(info.intervalName));
        return;
    }

    if (GingoChord::identify(gingoNotes, n, info.chordName, sizeof(info.chordName))) {
        const char* typeEn = chordTypeName(info.chordName);
        snprintf(info.chordFullName, sizeof(info.chordFullName),
                 "%s %s", gingoNotes[0].name(), typeEn);
    } else {
        strncpy(info.chordName, "?", sizeof(info.chordName));
    }

    char formula[64] = "";
    for (uint8_t i = 0; i < n; i++) {
        GingoInterval iv(gingoNotes[0], gingoNotes[i]);
        char lbl[8];
        iv.label(lbl, sizeof(lbl));
        if (i) strcat(formula, "  ");
        strcat(formula, lbl);
    }
    strncpy(info.formula, formula, sizeof(info.formula) - 1);
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    piano.init();
    piano.render(activeNotes, info);

    MIDIHandlerConfig cfg;
    cfg.maxEvents       = 64;
    cfg.chordTimeWindow = 0;
    cfg.bleName         = "ESP32 Piano";
    midiHandler.begin(cfg);

    synth.begin();
}

// ── Loop ──────────────────────────────────────────────────────────────────────
static uint32_t lastFrameUs = 0;
static const uint32_t FRAME_US = 33000;  // ~30 fps

void loop() {
    midiHandler.task();

    midiHandler.fillActiveNotes(activeNotes);

    if (memcmp(activeNotes, prevActiveNotes, sizeof(activeNotes)) != 0) {
        // Feed synth from state diff (not from queue)
        for (int n = 0; n < 128; n++) {
            if (activeNotes[n] && !prevActiveNotes[n]) synth.noteOn(n, 100);
            if (!activeNotes[n] && prevActiveNotes[n]) synth.noteOff(n);
        }
        memcpy(prevActiveNotes, activeNotes, sizeof(activeNotes));
        analyzeNotes();
    }

    uint32_t now = micros();
    if (now - lastFrameUs >= FRAME_US) {
        lastFrameUs = now;
        piano.render(activeNotes, info);
    }

}

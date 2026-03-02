#ifndef MUSIC_SEQUENCES_H
#define MUSIC_SEQUENCES_H

// ── Pre-programmed MIDI sequences for BLE MIDI Sender demo ───────────────────
// Each sequence demonstrates a different musical concept:
//   - Scales, chords, arpeggios, melodies
//   - Educationally interesting for MIDI demonstrations
//
// Portable: this file has no hardware dependencies.
// Can be reused on any Arduino-compatible board.

#include <Arduino.h>

// A single step in a sequence: one note or a chord (up to 6 simultaneous notes).
struct NoteStep {
    uint8_t notes[6];     // MIDI note numbers (0 = unused slot)
    uint8_t count;        // how many notes in this step (1 = single note, 2+ = chord)
    uint8_t velocity;     // MIDI velocity (1-127)
    uint16_t durationMs;  // how long to hold the note(s) before NoteOff
    uint16_t pauseMs;     // silence after NoteOff before the next step
};

// A named sequence of NoteSteps.
struct Sequence {
    const char* name;           // display name
    const NoteStep* steps;
    uint8_t stepCount;
    bool loop;                  // repeat when finished
};

// ── 1. C Major Scale (ascending + descending) ────────────────────────────────
// The most fundamental scale — visually shows keys "walking" across the piano.
static const NoteStep scaleSteps[] = {
    {{60,0,0,0,0,0}, 1, 100, 250, 50},   // C4
    {{62,0,0,0,0,0}, 1, 100, 250, 50},   // D4
    {{64,0,0,0,0,0}, 1, 100, 250, 50},   // E4
    {{65,0,0,0,0,0}, 1, 100, 250, 50},   // F4
    {{67,0,0,0,0,0}, 1, 100, 250, 50},   // G4
    {{69,0,0,0,0,0}, 1, 100, 250, 50},   // A4
    {{71,0,0,0,0,0}, 1, 100, 250, 50},   // B4
    {{72,0,0,0,0,0}, 1, 100, 400, 100},  // C5 (longer pause at top)
    {{71,0,0,0,0,0}, 1, 100, 250, 50},   // B4
    {{69,0,0,0,0,0}, 1, 100, 250, 50},   // A4
    {{67,0,0,0,0,0}, 1, 100, 250, 50},   // G4
    {{65,0,0,0,0,0}, 1, 100, 250, 50},   // F4
    {{64,0,0,0,0,0}, 1, 100, 250, 50},   // E4
    {{62,0,0,0,0,0}, 1, 100, 250, 50},   // D4
    {{60,0,0,0,0,0}, 1, 100, 400, 300},  // C4 (longer pause at end)
};

// ── 2. Pop Chord Progression I-V-vi-IV ───────────────────────────────────────
// The most popular chord progression in modern music (C → G → Am → F).
// Demonstrates chord sending — multiple notes simultaneously.
static const NoteStep chordProgSteps[] = {
    {{60, 64, 67, 0, 0, 0}, 3, 100, 700, 100},  // C  (C4-E4-G4)
    {{55, 59, 62, 0, 0, 0}, 3, 100, 700, 100},  // G  (G3-B3-D4)
    {{57, 60, 64, 0, 0, 0}, 3, 100, 700, 100},  // Am (A3-C4-E4)
    {{53, 57, 60, 0, 0, 0}, 3, 100, 700, 200},  // F  (F3-A3-C4)
};

// ── 3. Arpeggio Cmaj7 ───────────────────────────────────────────────────────
// Broken chord pattern — notes played one at a time, ascending then descending.
// Visually fluid on the piano display.
static const NoteStep arpSteps[] = {
    {{60,0,0,0,0,0}, 1, 90, 150, 20},   // C4
    {{64,0,0,0,0,0}, 1, 90, 150, 20},   // E4
    {{67,0,0,0,0,0}, 1, 90, 150, 20},   // G4
    {{71,0,0,0,0,0}, 1, 90, 150, 20},   // B4
    {{72,0,0,0,0,0}, 1, 90, 200, 20},   // C5
    {{71,0,0,0,0,0}, 1, 80, 150, 20},   // B4
    {{67,0,0,0,0,0}, 1, 80, 150, 20},   // G4
    {{64,0,0,0,0,0}, 1, 80, 150, 20},   // E4
    {{60,0,0,0,0,0}, 1, 80, 300, 200},  // C4
};

// ── 4. Für Elise (simplified theme) ──────────────────────────────────────────
// Beethoven's iconic melody — instantly recognizable.
static const NoteStep eliseSteps[] = {
    {{76,0,0,0,0,0}, 1, 90, 200, 30},   // E5
    {{75,0,0,0,0,0}, 1, 90, 200, 30},   // D#5
    {{76,0,0,0,0,0}, 1, 90, 200, 30},   // E5
    {{75,0,0,0,0,0}, 1, 90, 200, 30},   // D#5
    {{76,0,0,0,0,0}, 1, 90, 200, 30},   // E5
    {{71,0,0,0,0,0}, 1, 90, 200, 30},   // B4
    {{74,0,0,0,0,0}, 1, 90, 200, 30},   // D5
    {{72,0,0,0,0,0}, 1, 90, 200, 30},   // C5
    {{69,0,0,0,0,0}, 1, 90, 400, 50},   // A4 (resolution)
    {{60,0,0,0,0,0}, 1, 70, 200, 30},   // C4
    {{64,0,0,0,0,0}, 1, 70, 200, 30},   // E4
    {{69,0,0,0,0,0}, 1, 70, 400, 50},   // A4
    {{71,0,0,0,0,0}, 1, 90, 200, 30},   // B4
    {{64,0,0,0,0,0}, 1, 70, 200, 30},   // E4
    {{68,0,0,0,0,0}, 1, 70, 200, 30},   // G#4
    {{71,0,0,0,0,0}, 1, 70, 400, 50},   // B4
    {{72,0,0,0,0,0}, 1, 90, 400, 300},  // C5
};

// ── 5. Jazz ii-V-I ──────────────────────────────────────────────────────────
// The most important chord progression in jazz: Dm7 → G7 → Cmaj7.
// Demonstrates 4-note chords.
static const NoteStep jazzSteps[] = {
    {{62, 65, 69, 72, 0, 0}, 4, 95, 800, 100},  // Dm7  (D4-F4-A4-C5)
    {{55, 59, 62, 65, 0, 0}, 4, 95, 800, 100},  // G7   (G3-B3-D4-F4)
    {{60, 64, 67, 71, 0, 0}, 4, 95, 1000, 300},  // Cmaj7(C4-E4-G4-B4)
};

// ── 6. Chromatic Run ─────────────────────────────────────────────────────────
// All 12 semitones from C4 to C5 — demonstrates both black and white keys.
static const NoteStep chromaticSteps[] = {
    {{60,0,0,0,0,0}, 1, 85, 100, 20},  // C4
    {{61,0,0,0,0,0}, 1, 85, 100, 20},  // C#4
    {{62,0,0,0,0,0}, 1, 85, 100, 20},  // D4
    {{63,0,0,0,0,0}, 1, 85, 100, 20},  // D#4
    {{64,0,0,0,0,0}, 1, 85, 100, 20},  // E4
    {{65,0,0,0,0,0}, 1, 85, 100, 20},  // F4
    {{66,0,0,0,0,0}, 1, 85, 100, 20},  // F#4
    {{67,0,0,0,0,0}, 1, 85, 100, 20},  // G4
    {{68,0,0,0,0,0}, 1, 85, 100, 20},  // G#4
    {{69,0,0,0,0,0}, 1, 85, 100, 20},  // A4
    {{70,0,0,0,0,0}, 1, 85, 100, 20},  // A#4
    {{71,0,0,0,0,0}, 1, 85, 100, 20},  // B4
    {{72,0,0,0,0,0}, 1, 85, 200, 300}, // C5
};

// ── Sequence index ──────────────────────────────────────────────────────────
static const Sequence ALL_SEQUENCES[] = {
    {"C Major Scale",   scaleSteps,     15, true},
    {"Pop I-V-vi-IV",   chordProgSteps,  4, true},
    {"Arpeggio Cmaj7",  arpSteps,        9, true},
    {"Fur Elise",       eliseSteps,     17, true},
    {"Jazz ii-V-I",     jazzSteps,       3, true},
    {"Chromatic Run",   chromaticSteps, 13, true},
};
static const int NUM_SEQUENCES = sizeof(ALL_SEQUENCES) / sizeof(ALL_SEQUENCES[0]);

// ── Note name helper (portable, no library dependency) ───────────────────────
static const char* const SEQ_NOTE_NAMES[12] = {
    "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
};

static inline const char* midiNoteName(uint8_t note) {
    return SEQ_NOTE_NAMES[note % 12];
}

static inline int midiNoteOctave(uint8_t note) {
    return (note / 12) - 1;
}

#endif // MUSIC_SEQUENCES_H

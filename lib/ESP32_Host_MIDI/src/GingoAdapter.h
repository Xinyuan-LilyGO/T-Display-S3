// GingoAdapter.h — Optional bridge between ESP32_Host_MIDI and Gingoduino.
//
// Include this header ONLY if you have the Gingoduino library installed.
// It provides utility functions to convert MIDIHandler chord data
// into Gingoduino objects for chord identification, harmonic field
// deduction, and progression analysis.
//
// Usage:
//   #include <ESP32_Host_MIDI.h>
//   #include <GingoAdapter.h>
//
// Requirements:
//   - Gingoduino library v0.2.2+ (https://github.com/sauloverissimo/gingoduino)

#ifndef GINGO_ADAPTER_H
#define GINGO_ADAPTER_H

#include <Gingoduino.h>
#include "MIDIHandler.h"
#include <algorithm>

namespace GingoAdapter {

// Maximum notes in a chord (matches GingoChord formula limit)
static constexpr uint8_t MAX_CHORD_NOTES = 7;

// Maximum chord names tracked for field deduction / progression analysis
static constexpr uint8_t MAX_CHORD_HISTORY = 16;

// =========================================================================
// Note Conversion
// =========================================================================

// Converts an array of MIDI note numbers into GingoNote objects.
// Notes are sorted by pitch (lowest first) so the root is correctly placed.
// Returns the number of notes written to the output array.
inline uint8_t midiToGingoNotes(const uint8_t* midiNotes, uint8_t count,
                                gingoduino::GingoNote* output) {
    if (!midiNotes || !output || count == 0) return 0;

    uint8_t n = (count > MAX_CHORD_NOTES) ? MAX_CHORD_NOTES : count;

    // Sort MIDI notes by pitch
    uint8_t sorted[MAX_CHORD_NOTES];
    for (uint8_t i = 0; i < n; i++) sorted[i] = midiNotes[i];
    std::sort(sorted, sorted + n);

    for (uint8_t i = 0; i < n; i++) {
        output[i] = gingoduino::GingoNote::fromMIDI(sorted[i]);
    }
    return n;
}

// Converts active notes from MIDIHandler into GingoNote objects.
// Returns the number of notes written.
inline uint8_t activeNotesToGingo(const MIDIHandler& handler,
                                  gingoduino::GingoNote* output) {
    if (!output) return 0;

    std::vector<std::string> noteNames = handler.getActiveNotesVector();
    uint8_t count = 0;
    for (const auto& name : noteNames) {
        if (count >= MAX_CHORD_NOTES) break;
        output[count++] = gingoduino::GingoNote(name.c_str());
    }
    return count;
}

// =========================================================================
// Chord Identification
// =========================================================================

// Identifies the chord from the last chord in the MIDIHandler queue.
// Writes the chord name (e.g., "CM", "Am7", "Gdim") into the output buffer.
// Returns true if a chord was identified, false otherwise.
inline bool identifyLastChord(const MIDIHandler& handler,
                              char* output, uint8_t maxLen) {
    if (!output || maxLen < 2) return false;

    const auto& queue = handler.getQueue();
    if (queue.empty()) return false;

    int chordIdx = handler.lastChord(queue);
    if (chordIdx <= 0) return false;

    std::vector<std::string> noteStrs = handler.getChord(
        chordIdx, queue, {"note"}
    );

    if (noteStrs.empty()) return false;

    uint8_t midiNotes[MAX_CHORD_NOTES];
    uint8_t count = 0;
    for (const auto& s : noteStrs) {
        if (count >= MAX_CHORD_NOTES) break;
        midiNotes[count++] = (uint8_t)atoi(s.c_str());
    }

    gingoduino::GingoNote notes[MAX_CHORD_NOTES];
    uint8_t n = midiToGingoNotes(midiNotes, count, notes);

    return gingoduino::GingoChord::identify(notes, n, output, maxLen);
}

// Identifies a chord from a specific chordIndex in the queue.
// Returns true if a chord was identified, false otherwise.
inline bool identifyChord(const MIDIHandler& handler, int chordIndex,
                          char* output, uint8_t maxLen) {
    if (!output || maxLen < 2) return false;

    const auto& queue = handler.getQueue();
    if (queue.empty()) return false;

    std::vector<std::string> noteStrs = handler.getChord(
        chordIndex, queue, {"note"}
    );

    if (noteStrs.empty()) return false;

    uint8_t midiNotes[MAX_CHORD_NOTES];
    uint8_t count = 0;
    for (const auto& s : noteStrs) {
        if (count >= MAX_CHORD_NOTES) break;
        midiNotes[count++] = (uint8_t)atoi(s.c_str());
    }

    gingoduino::GingoNote notes[MAX_CHORD_NOTES];
    uint8_t n = midiToGingoNotes(midiNotes, count, notes);

    return gingoduino::GingoChord::identify(notes, n, output, maxLen);
}

// =========================================================================
// Harmonic Field Deduction (Tier 2+)
// =========================================================================

#if defined(GINGODUINO_HAS_FIELD)

// Deduces probable harmonic fields from a list of chord names.
// chordNames: array of C strings (e.g., {"CM", "FM", "G7"})
// output: array of FieldMatch to receive results
// maxResults: max entries to write
// Returns number of results written (sorted by relevance).
inline uint8_t deduceField(const char* const* chordNames, uint8_t chordCount,
                           gingoduino::FieldMatch* output, uint8_t maxResults) {
    if (!chordNames || !output || chordCount == 0) return 0;
    return gingoduino::GingoField::deduce(chordNames, chordCount, output, maxResults);
}

// Deduces harmonic field from all identified chords in the MIDIHandler queue.
// Scans the queue, identifies each chord, then passes the chord names to Field::deduce.
// Returns number of results written.
inline uint8_t deduceFieldFromQueue(const MIDIHandler& handler,
                                    gingoduino::FieldMatch* output, uint8_t maxResults) {
    if (!output) return 0;

    const auto& queue = handler.getQueue();
    if (queue.empty()) return 0;

    // Collect unique chord names from the queue
    char chordBufs[MAX_CHORD_HISTORY][16];
    const char* chordPtrs[MAX_CHORD_HISTORY];
    uint8_t chordCount = 0;

    // Find all unique chordIndex values in the queue
    int seenChords[MAX_CHORD_HISTORY];
    uint8_t seenCount = 0;

    for (const auto& event : queue) {
        if (event.chordIndex <= 0 || event.status != "NoteOn") continue;

        // Check if we already processed this chordIndex
        bool seen = false;
        for (uint8_t i = 0; i < seenCount; i++) {
            if (seenChords[i] == event.chordIndex) { seen = true; break; }
        }
        if (seen) continue;
        if (seenCount >= MAX_CHORD_HISTORY) break;
        seenChords[seenCount++] = event.chordIndex;

        // Try to identify this chord
        char name[16];
        if (identifyChord(handler, event.chordIndex, name, sizeof(name))) {
            // Check for duplicate chord names
            bool dup = false;
            for (uint8_t i = 0; i < chordCount; i++) {
                if (strcmp(chordBufs[i], name) == 0) { dup = true; break; }
            }
            if (!dup && chordCount < MAX_CHORD_HISTORY) {
                strncpy(chordBufs[chordCount], name, 15);
                chordBufs[chordCount][15] = '\0';
                chordPtrs[chordCount] = chordBufs[chordCount];
                chordCount++;
            }
        }
    }

    if (chordCount == 0) return 0;

    return gingoduino::GingoField::deduce(chordPtrs, chordCount, output, maxResults);
}

#endif // GINGODUINO_HAS_FIELD

// =========================================================================
// Progression Analysis (Tier 3)
// =========================================================================

#if defined(GINGODUINO_HAS_PROGRESSION)

// Identifies the best progression match for a sequence of branch names.
// tonic: tonic note (e.g., "C")
// scaleType: scale type (e.g., gingoduino::SCALE_MAJOR)
// branches: array of branch names (e.g., {"IIm", "V7", "I"})
// Returns true if a match was found.
inline bool identifyProgression(const char* tonic, gingoduino::ScaleType scaleType,
                                const char* const* branches, uint8_t count,
                                gingoduino::ProgressionMatch* result) {
    if (!tonic || !branches || !result || count == 0) return false;
    gingoduino::GingoProgression prog(tonic, scaleType);
    return prog.identify(branches, count, result);
}

// Predicts the next branches from a partial sequence.
// Returns number of predictions written.
inline uint8_t predictNext(const char* tonic, gingoduino::ScaleType scaleType,
                           const char* const* branches, uint8_t count,
                           gingoduino::ProgressionRoute* output, uint8_t maxResults) {
    if (!tonic || !branches || !output || count == 0) return 0;
    gingoduino::GingoProgression prog(tonic, scaleType);
    return prog.predict(branches, count, output, maxResults);
}

#endif // GINGODUINO_HAS_PROGRESSION

} // namespace GingoAdapter

#endif // GINGO_ADAPTER_H

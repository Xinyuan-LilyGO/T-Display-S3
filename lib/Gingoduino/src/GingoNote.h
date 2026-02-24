// Gingoduino — Music Theory Library for Embedded Systems
// GingoNote: the atomic unit of Western music.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_NOTE_H
#define GINGO_NOTE_H

#include "gingoduino_config.h"
#include "gingoduino_types.h"

namespace gingoduino {

/// Represents a single musical pitch class (e.g. "C", "Bb", "F#").
///
/// Notes are stored in their original form and can be converted to a
/// canonical sharp-based representation (natural form).
///
/// ~12 bytes per instance. No heap allocation.
///
/// Examples:
///   GingoNote n("Bb");
///   n.name();          // "Bb"
///   n.natural();       // "A#"
///   n.sound();         // 'B'
///   n.semitone();      // 10
///   n.frequency(4);    // 466.16 Hz
class GingoNote {
public:
    /// Default constructor (creates C natural).
    GingoNote();

    /// Construct from any accepted notation: "C", "Bb", "F#", "Gb", etc.
    explicit GingoNote(const char* name);

    /// The name as given at construction time.
    const char* name() const { return name_.c_str(); }

    /// Canonical sharp-based form: "Bb" -> "A#", "Gb" -> "F#", "C" -> "C".
    const char* natural() const { return natural_.c_str(); }

    /// The base letter only (no accidentals): "Bb" -> 'B', "F#" -> 'F'.
    char sound() const { return sound_; }

    /// Position in the chromatic scale (C=0, C#=1, ... B=11).
    uint8_t semitone() const { return semitone_; }

    /// Concert pitch frequency in Hz.
    /// @param octave  Octave number (default 4, where A4 = tuning reference).
    /// @param tuning  Reference frequency for A4 in Hz (default 440.0).
    float frequency(int8_t octave = 4, float tuning = 440.0f) const;

    /// MIDI note number (C4 = 60).
    uint8_t midiNumber(int8_t octave = 4) const {
        return (uint8_t)(12 * (octave + 1) + semitone_);
    }

    /// Enharmonic equivalence: GingoNote("Bb").isEnharmonic(GingoNote("A#")) == true.
    bool isEnharmonic(const GingoNote& other) const { return semitone_ == other.semitone_; }

    /// Transpose by a number of semitones (positive = up, negative = down).
    GingoNote transpose(int8_t semitones) const;

    /// Shortest distance on the circle of fifths (0-6).
    uint8_t distance(const GingoNote& other) const;

    bool operator==(const GingoNote& other) const { return natural_ == other.natural_; }
    bool operator!=(const GingoNote& other) const { return !(natural_ == other.natural_); }

    // -- MIDI conversion --

    /// Create a note from a MIDI number (C4 = 60).
    /// Uses sharp-based notation (C#, D#, F#, G#, A#).
    static GingoNote fromMIDI(uint8_t midiNote);

    /// Extract octave from MIDI number (C4 = 60 → octave 4).
    static int8_t octaveFromMIDI(uint8_t midiNote);

    // -- Static utilities --

    /// Resolve any enharmonic spelling to sharp-based canonical form.
    /// Writes result to output buffer.
    static void toNatural(const char* noteName, char* output, uint8_t maxLen);

    /// Map any note name to its chromatic index 0-11.
    static uint8_t toSemitone(const char* noteName);

    /// Extract the root note from a chord name: "C#m7" -> "C#".
    static void extractRoot(const char* chordName, char* output, uint8_t maxLen);

    /// Extract just the base letter: "C#m7" -> 'C'.
    static char extractSound(const char* name);

    /// Extract the chord-type suffix: "C#m7" -> "m7".
    static void extractType(const char* chordName, char* output, uint8_t maxLen);

private:
    NoteStr name_;       // original input (5 bytes)
    NoteStr natural_;    // sharp-based canonical (5 bytes)
    char    sound_;      // base letter (1 byte)
    uint8_t semitone_;   // 0-11 (1 byte)
};

} // namespace gingoduino

#endif // GINGO_NOTE_H

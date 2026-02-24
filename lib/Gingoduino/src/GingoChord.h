// Gingoduino — Music Theory Library for Embedded Systems
// GingoChord: a combination of notes with harmonic identity.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_CHORD_H
#define GINGO_CHORD_H

#include "gingoduino_config.h"
#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoInterval.h"

namespace gingoduino {

/// Represents a musical chord — a root note plus a set of intervals.
///
/// Constructed from a name string (e.g. "Cm7", "Db7M", "A#m") and
/// resolves notes/intervals from the PROGMEM chord formula database.
///
/// ~35 bytes per instance. No heap allocation.
///
/// Examples:
///   GingoChord c("Cm7");
///   c.root().name();     // "C"
///   c.type();            // "m7"
///   c.size();            // 4
///
///   GingoNote notes[7];
///   uint8_t n = c.notes(notes, 7);  // n=4, notes filled
class GingoChord {
public:
    /// Default constructor (C Major).
    GingoChord();

    /// Construct from a chord name: "CM", "Dm7", "Bb7M(#5)", etc.
    explicit GingoChord(const char* name);

    /// The full chord name as given.
    const char* name() const { return name_.c_str(); }

    /// Root note of the chord.
    GingoNote root() const { return GingoNote(rootStr_.c_str()); }

    /// Chord type/quality suffix: "m7", "7M", "dim", "aug", etc.
    const char* type() const { return type_.c_str(); }

    /// How many notes in the chord.
    uint8_t size() const;

    /// Fill output array with chord tones (natural/sharp-based).
    /// Returns the number of notes written.
    uint8_t notes(GingoNote* output, uint8_t maxNotes) const;

    /// Fill output array with interval labels.
    /// Returns the number of labels written.
    uint8_t intervalLabels(LabelStr* output, uint8_t maxLabels) const;

    /// Whether a given note (by semitone) is part of this chord.
    bool contains(const GingoNote& note) const;

    /// Transpose the chord by a number of semitones.
    GingoChord transpose(int8_t semitones) const;

    /// Identify a chord from a set of notes (reverse lookup).
    /// The first note is treated as the root.
    /// Writes the chord name to output. Returns true if found.
    static bool identify(const GingoNote* notes, uint8_t count,
                         char* output, uint8_t maxLen);

    /// Fill output array with GingoInterval objects for this chord.
    /// Returns the number of intervals written.
    uint8_t intervals(GingoInterval* output, uint8_t maxIntervals) const;

    /// Index of this chord's formula in CHORD_FORMULAS (255 = unknown).
    uint8_t formulaIndex() const { return formulaIdx_; }

    bool operator==(const GingoChord& other) const { return name_ == other.name_; }
    bool operator!=(const GingoChord& other) const { return name_ != other.name_; }

private:
    NameStr  name_;         // full chord name
    NoteStr  rootStr_;      // root note string
    LabelStr type_;         // type/quality suffix
    uint8_t  formulaIdx_;   // index in CHORD_FORMULAS (255 = not found)

    void resolveFormula();
};

} // namespace gingoduino

#endif // GINGO_CHORD_H

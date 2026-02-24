// Gingoduino — Music Theory Library for Embedded Systems
// GingoFretboard: fretted string instrument engine.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_FRETBOARD_H
#define GINGO_FRETBOARD_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_FRETBOARD

#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoChord.h"
#include "GingoScale.h"

namespace gingoduino {

/// Action on a string: open, fretted, or muted.
enum StringAction : uint8_t {
    STRING_OPEN,
    STRING_FRETTED,
    STRING_MUTED
};

/// A position on the fretboard.
struct GingoFretPos {
    uint8_t   string;   ///< 0-based string index (low to high)
    uint8_t   fret;     ///< fret number (0 = open)
    uint8_t   midi;     ///< MIDI note number
    GingoNote note;     ///< the note at this position
    uint8_t   octave;   ///< octave number
};

/// State of a single string in a fingering.
struct GingoStringState {
    uint8_t      string;  ///< 0-based string index
    StringAction action;  ///< open, fretted, or muted
    uint8_t      fret;    ///< fret number (0 if open or muted)
};

/// A chord fingering on the fretboard.
struct GingoFingering {
    GingoStringState strings[GINGODUINO_MAX_STRINGS];
    uint8_t  numStrings;
    NameStr  chordName;
    uint8_t  baseFret;    ///< lowest non-zero fret in this fingering
    uint8_t  capoFret;    ///< capo position (0 = no capo)
    uint8_t  midiNotes[GINGODUINO_MAX_STRINGS];
    uint8_t  numNotes;    ///< number of sounding notes
    uint16_t score;       ///< lower = better playability
};

/// Fretted string instrument engine.
///
/// Computes note positions, scale patterns, and chord fingerings
/// for any fretted string instrument.
///
/// Examples:
///   auto fb = GingoFretboard::violao();
///   fb.numStrings();   // 6
///   fb.noteAt(0, 5);   // GingoNote("A")
///
///   GingoFingering fg;
///   fb.fingering(GingoChord("CM"), 0, fg);
///   // fg contains open C major shape
class GingoFretboard {
public:
    /// Construct from explicit tuning data.
    /// @param name       instrument name
    /// @param openMidi   MIDI notes for each open string (low to high)
    /// @param numStrings number of strings
    /// @param numFrets   number of frets
    GingoFretboard(const char* name,
                   const uint8_t* openMidi,
                   uint8_t numStrings,
                   uint8_t numFrets = 19);

    /// Factory: standard 6-string guitar (violao).
    static GingoFretboard violao(uint8_t numFrets = 19);

    /// Factory: cavaquinho (4 strings).
    static GingoFretboard cavaquinho(uint8_t numFrets = 17);

    /// Factory: bandolim / mandolin (4 strings).
    static GingoFretboard bandolim(uint8_t numFrets = 17);

    /// Factory: ukulele (4 strings).
    static GingoFretboard ukulele(uint8_t numFrets = 17);

    /// Instrument name.
    const char* name() const { return name_.c_str(); }

    /// Number of strings.
    uint8_t numStrings() const { return numStrings_; }

    /// Number of frets.
    uint8_t numFrets() const { return numFrets_; }

    /// MIDI number of an open string.
    uint8_t openMidi(uint8_t string) const;

    /// Get the fret position at (string, fret).
    GingoFretPos position(uint8_t string, uint8_t fret) const;

    /// Get the note at (string, fret).
    GingoNote noteAt(uint8_t string, uint8_t fret) const;

    /// Get the MIDI number at (string, fret).
    uint8_t midiAt(uint8_t string, uint8_t fret) const;

    /// Find all positions of a given note on the fretboard.
    /// Returns the number of positions written.
    uint8_t positions(const GingoNote& note,
                      GingoFretPos* output, uint8_t maxPositions) const;

    /// Find all positions of scale notes within a fret range.
    /// Returns the number of positions written.
    uint8_t scalePositions(const GingoScale& scale,
                           GingoFretPos* output, uint8_t maxPositions,
                           uint8_t fretLo = 0, uint8_t fretHi = 255) const;

    /// Find a single fingering for a chord at a given position index.
    /// Returns true if a valid fingering was found.
    bool fingering(const GingoChord& chord, uint8_t positionIdx,
                   GingoFingering& output) const;

    /// Find multiple fingerings for a chord, sorted by playability score.
    /// Returns the number of fingerings written.
    uint8_t fingerings(const GingoChord& chord,
                       GingoFingering* output, uint8_t maxResults) const;

    /// Identify a chord from string-fret positions.
    /// @param stringFrets  array of fret numbers per string (255 = muted)
    /// @param count        number of entries
    /// @param output       buffer for chord name
    /// @param maxLen       buffer size
    /// @return true if a chord was identified
    bool identify(const uint8_t* stringFrets, uint8_t count,
                  char* output, uint8_t maxLen) const;

    /// Create a new fretboard with a capo at the given fret.
    GingoFretboard capo(uint8_t fret) const;

private:
    NameStr  name_;
    uint8_t  openMidi_[GINGODUINO_MAX_STRINGS];
    uint8_t  numStrings_;
    uint8_t  numFrets_;
    uint8_t  capoFret_;

    /// Score a fingering for playability (lower = better).
    uint16_t scoreFingering(const GingoFingering& fg) const;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_FRETBOARD
#endif // GINGO_FRETBOARD_H

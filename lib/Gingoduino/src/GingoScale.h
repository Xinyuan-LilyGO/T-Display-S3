// Gingoduino — Music Theory Library for Embedded Systems
// GingoScale: ordered sequence of notes following a tonal pattern.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_SCALE_H
#define GINGO_SCALE_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_SCALE

#include "gingoduino_types.h"
#include "GingoNote.h"

namespace gingoduino {

/// Represents a musical scale built from a tonic and a scale pattern.
///
/// Examples:
///   GingoScale s("C", SCALE_MAJOR);
///   s.size();        // 7
///   s.degree(5);     // GingoNote("G")
///
///   GingoNote notes[12];
///   uint8_t n = s.notes(notes, 12);
class GingoScale {
public:
    /// Construct from tonic and scale type enum.
    GingoScale(const char* tonic, ScaleType type, uint8_t modeNum = 1, bool penta = false);

    /// Construct from tonic and string name: "major", "dorian", "harmonic minor", etc.
    GingoScale(const char* tonic, const char* typeName);

    /// The tonic (first degree) of the scale.
    GingoNote tonic() const { return tonic_; }

    /// The parent scale type.
    ScaleType parent() const { return parent_; }

    /// Mode number within the parent (1-based).
    uint8_t modeNumber() const { return modeNumber_; }

    /// Whether the pentatonic filter is applied.
    bool isPentatonic() const { return pentatonic_; }

    /// Mode name (e.g. "Dorian", "Phrygian Dominant").
    /// Writes to buffer and returns it.
    const char* modeName(char* buf, uint8_t maxLen) const;

    /// Number of notes in the scale.
    uint8_t size() const;

    /// Fill output array with scale notes.
    /// Returns the number of notes written.
    uint8_t notes(GingoNote* output, uint8_t maxNotes) const;

    /// Get the note at a specific scale degree (1-indexed).
    GingoNote degree(uint8_t n) const;

    /// Whether a note belongs to this scale.
    bool contains(const GingoNote& note) const;

    /// Build a new scale starting from a different mode/degree.
    GingoScale mode(uint8_t degreeNumber) const;

    /// Return a pentatonic version of this scale.
    GingoScale pentatonic() const;

    /// Quality: "major" or "minor" based on the third.
    const char* quality() const;

    /// Key signature: number of sharps (+) or flats (-), range -7 to +7.
    int8_t signature() const;

    /// Relative scale (major→aeolian, minor→ionian).
    GingoScale relative() const;

    /// Parallel scale (same tonic, opposite quality).
    GingoScale parallel() const;

    /// Find the scale degree of a note (1-based). Returns 0 if not found.
    uint8_t degreeOf(const GingoNote& note) const;

    /// Build a mode by name: "dorian", "phrygian", etc.
    GingoScale modeByName(const char* modeName) const;

    /// Brightness ranking within the parent family (higher = brighter).
    uint8_t brightness() const;

    /// 12-bit pitch-class mask (bit N = semitone N from tonic is active).
    uint16_t mask() const;

private:
    GingoNote tonic_;
    ScaleType parent_;
    uint8_t   modeNumber_;
    bool      pentatonic_;

    /// Compute the 12-bit mask (one bit per semitone) for this scale+mode.
    uint16_t computeMask12() const;

    /// Parse a scale type name to ScaleType enum. Returns SCALE_MAJOR if unknown.
    static ScaleType parseTypeName(const char* name, uint8_t* outMode);
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_SCALE
#endif // GINGO_SCALE_H

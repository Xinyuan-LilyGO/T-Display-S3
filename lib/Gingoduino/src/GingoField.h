// Gingoduino — Music Theory Library for Embedded Systems
// GingoField: harmonic field — chords built from each scale degree.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_FIELD_H
#define GINGO_FIELD_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_FIELD

#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoChord.h"
#include "GingoScale.h"

namespace gingoduino {

/// Represents a harmonic field — the diatonic chords built from each
/// degree of a scale.
///
/// Examples:
///   GingoField f("C", SCALE_MAJOR);
///   GingoChord triads[7];
///   f.chords(triads, 7);
///   // triads: CM, Dm, Em, FM, GM, Am, Bdim
/// Result of GingoField::deduce() — a candidate harmonic field match.
struct FieldMatch {
    const char* tonicName;   // tonic name (points to internal static buffer)
    ScaleType   scaleType;   // scale type of the candidate field
    uint8_t     matched;     // how many input items belong to this field
    uint8_t     total;       // how many input items were given
    char        roles[7][8]; // roman numeral roles for matched items
    uint8_t     roleCount;   // how many roles filled
};

class GingoField {
public:
    GingoField();
    GingoField(const char* tonic, ScaleType type);
    GingoField(const char* tonic, const char* typeName);

    /// The tonic note.
    GingoNote tonic() const { return scale_.tonic(); }

    /// The underlying scale.
    const GingoScale& scale() const { return scale_; }

    /// Fill output with triads (3-note chords) for each degree.
    /// Returns number of chords written.
    uint8_t chords(GingoChord* output, uint8_t maxChords) const;

    /// Fill output with seventh chords for each degree.
    uint8_t sevenths(GingoChord* output, uint8_t maxChords) const;

    /// Get the triad at a specific degree (1-indexed).
    GingoChord chord(uint8_t degree) const;

    /// Get the seventh chord at a specific degree (1-indexed).
    GingoChord seventh(uint8_t degree) const;

    /// Harmonic function of a degree (1-indexed): 0=T, 1=S, 2=D.
    HarmonicFunc function(uint8_t degree) const;

    /// Role of a degree. Writes to buffer and returns it.
    const char* role(uint8_t degree, char* buf, uint8_t maxLen) const;

    /// Harmonic function of a chord in this field.
    /// Returns FUNC_TONIC if the chord root is not in the scale.
    HarmonicFunc functionOf(const GingoChord& chord) const;

    /// Harmonic function of a chord by name.
    HarmonicFunc functionOf(const char* chordName) const;

    /// Role of a chord in this field. Writes to buffer and returns it.
    const char* roleOf(const GingoChord& chord, char* buf, uint8_t maxLen) const;

    /// Role of a chord by name.
    const char* roleOf(const char* chordName, char* buf, uint8_t maxLen) const;

    /// Key signature (delegates to scale).
    int8_t signature() const { return scale_.signature(); }

    /// Number of degrees.
    uint8_t size() const { return scale_.size(); }

    /// Deduce the most probable harmonic fields from a set of notes or chords.
    /// Items can be note names ("C", "E", "G") or chord names ("CM", "Dm", "G7").
    /// Returns the number of results written to output, sorted by score desc.
    static uint8_t deduce(const char* const* items, uint8_t itemCount,
                          FieldMatch* output, uint8_t maxResults);

private:
    GingoScale scale_;

    /// Build chords by stacking intervals at given degree offsets.
    /// offsets: array of scale-degree offsets {0, 2, 4} for triads.
    uint8_t buildChords(GingoChord* output, uint8_t maxChords,
                        const uint8_t* offsets, uint8_t offsetCount) const;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_FIELD
#endif // GINGO_FIELD_H

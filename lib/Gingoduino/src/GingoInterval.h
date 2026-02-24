// Gingoduino — Music Theory Library for Embedded Systems
// GingoInterval: the distance between two notes.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_INTERVAL_H
#define GINGO_INTERVAL_H

#include "gingoduino_config.h"
#include "gingoduino_types.h"

namespace gingoduino {

class GingoNote; // forward declaration

/// Represents a musical interval — the distance between two pitches.
///
/// Only 1 byte per instance (an index into the INTERVAL_TABLE).
///
/// Examples:
///   GingoInterval iv("3m");
///   iv.semitones();    // 3
///   iv.label();        // "3m"  (reads from PROGMEM)
///   iv.angloSaxon();   // "mi3"
///   iv.degree();       // 3
class GingoInterval {
public:
    /// Default constructor (unison).
    GingoInterval() : index_(0) {}

    /// Construct from a short label: "P1", "3m", "5J", "7M", "b9", etc.
    explicit GingoInterval(const char* label);

    /// Construct from a semitone count (0-23).
    explicit GingoInterval(uint8_t semitones);

    /// Construct from two notes: ascending chromatic distance (mod 24).
    GingoInterval(const GingoNote& from, const GingoNote& to);

    /// Short label: "P1", "3m", "5J", "7M", etc.
    /// Writes to the provided buffer and returns it.
    const char* label(char* buf, uint8_t maxLen) const;

    /// Anglo-Saxon formal name: "P1", "mi3", "P5", "ma7", etc.
    const char* angloSaxon(char* buf, uint8_t maxLen) const;

    /// Number of semitones (0 = unison, 7 = perfect fifth, etc.)
    uint8_t semitones() const { return index_; }

    /// Diatonic degree number (1 = unison, 3 = third, 5 = fifth, etc.)
    uint8_t degree() const;

    /// Which octave the interval falls in (1 = first octave, 2 = second).
    uint8_t octave() const;

    /// Index in the interval table (= semitone count).
    uint8_t index() const { return index_; }

    /// Reduce to simple interval (first octave, 0-11 semitones).
    GingoInterval simple() const;

    /// Whether this interval spans more than one octave.
    bool isCompound() const { return index_ > 11; }

    /// Complement within an octave.
    GingoInterval invert() const;

    /// Consonance classification: "perfect", "imperfect", or "dissonant".
    const char* consonance(char* buf, uint8_t maxLen) const;

    /// Whether the interval is consonant (perfect or imperfect).
    bool isConsonant() const;

    /// Full English name: "Perfect Fifth", "Minor Third", etc.
    const char* fullName(char* buf, uint8_t maxLen) const;

    /// Full Portuguese name: "Quinta Justa", "Terca Menor", etc.
    const char* fullNamePt(char* buf, uint8_t maxLen) const;

    /// Sum of two intervals (capped at 23 semitones).
    GingoInterval operator+(const GingoInterval& other) const;

    /// Difference of two intervals (floored at 0 semitones).
    GingoInterval operator-(const GingoInterval& other) const;

    bool operator==(const GingoInterval& other) const { return index_ == other.index_; }
    bool operator!=(const GingoInterval& other) const { return index_ != other.index_; }

    /// Find semitone count from a label. Returns 255 if unknown.
    static uint8_t labelToSemitones(const char* label);

private:
    uint8_t index_;  // 0-23, index into INTERVAL_TABLE
};

} // namespace gingoduino

#endif // GINGO_INTERVAL_H

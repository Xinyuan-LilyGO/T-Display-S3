// Gingoduino — Music Theory Library for Embedded Systems
// GingoDuration: rhythmic durations and time values.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_DURATION_H
#define GINGO_DURATION_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_DURATION

#include "gingoduino_types.h"

namespace gingoduino {

/// Represents a rhythmic duration (whole, half, quarter, eighth, etc.).
///
/// Durations are stored as rational values (numerator/denominator) to avoid
/// floating-point precision issues.
///
/// Examples:
///   GingoDuration d("quarter");      // 1/4 note
///   d.beats();                       // 1.0
///   d.numerator();                   // 1
///   d.denominator();                 // 4
class GingoDuration {
public:
    /// Default constructor (quarter note).
    GingoDuration();

    /// Construct from name: "whole", "half", "quarter", "eighth", etc.
    explicit GingoDuration(const char* name, uint8_t dots = 0, uint8_t tuplet = 0);

    /// Construct from rational value.
    GingoDuration(int16_t numerator, int16_t denominator);

    /// Duration name.
    const char* name(char* buf, uint8_t maxLen) const;

    /// Number of dots.
    uint8_t dots() const { return dots_; }

    /// Tuplet division (0 = none, 3 = triplet).
    uint8_t tuplet() const { return tuplet_; }

    /// Duration in beats (quarter note = 1.0 by default).
    float beats(float referenceValue = 1.0f) const;

    /// Numerator of rational duration.
    int16_t numerator() const { return numerator_; }

    /// Denominator of rational duration.
    int16_t denominator() const { return denominator_; }

    /// Sum of two durations (rational addition with GCD reduction).
    GingoDuration operator+(const GingoDuration& other) const;

    /// Comparison (cross-multiplication).
    bool operator<(const GingoDuration& other) const {
        return (int32_t)numerator_ * other.denominator_ < (int32_t)other.numerator_ * denominator_;
    }
    bool operator>(const GingoDuration& other) const { return other < *this; }
    bool operator<=(const GingoDuration& other) const { return !(other < *this); }
    bool operator>=(const GingoDuration& other) const { return !(*this < other); }

    bool operator==(const GingoDuration& other) const {
        return (int32_t)numerator_ * other.denominator_ == (int32_t)other.numerator_ * denominator_;
    }
    bool operator!=(const GingoDuration& other) const { return !(*this == other); }

private:
    uint8_t nameIdx_;       // index in DURATION_NAMES
    uint8_t dots_;
    uint8_t tuplet_;
    int16_t numerator_;
    int16_t denominator_;

    void computeRational();
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_DURATION
#endif // GINGO_DURATION_H

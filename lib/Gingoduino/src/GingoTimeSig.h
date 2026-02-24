// Gingoduino — Music Theory Library for Embedded Systems
// GingoTimeSig: meter and bar structure.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_TIMESIG_H
#define GINGO_TIMESIG_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_TIMESIG

#include "gingoduino_types.h"

namespace gingoduino {

class GingoDuration;  // forward declaration

/// Represents a time signature (meter): beats per bar and beat unit.
///
/// Examples: 4/4, 3/4, 6/8, 7/8, 5/4, etc.
///
/// Examples:
///   GingoTimeSig ts(4, 4);       // common time
///   ts.beatsPerBar();            // 4
///   ts.beatUnit();               // 4 (quarter note)
///   ts.barDuration();            // GingoDuration(4, 4)
class GingoTimeSig {
public:
    /// Construct from beats per bar and beat unit.
    /// GingoTimeSig(4, 4) = 4/4, GingoTimeSig(6, 8) = 6/8.
    GingoTimeSig(uint8_t beatsPerBar = 4, uint8_t beatUnit = 4);

    /// Number of beats per bar (numerator).
    uint8_t beatsPerBar() const { return beatsPerBar_; }

    /// Beat unit (denominator): 2=half, 4=quarter, 8=eighth, etc.
    uint8_t beatUnit() const { return beatUnit_; }

    /// Total duration of one complete bar.
    GingoDuration barDuration() const;

    /// Classification: returns true if compound (6/8, 9/8, 12/8),
    /// false if simple.
    bool isCompound() const;

    /// Classification string: "simple" or "compound".
    /// Writes to buffer and returns it.
    const char* classification(char* buf, uint8_t maxLen) const;

    /// Common names: 4/4 = "common time", 2/2 = "cut time", else "X/Y".
    /// Writes to buffer and returns it.
    const char* commonName(char* buf, uint8_t maxLen) const;

    /// Write "X/Y" format to buffer.
    const char* toString(char* buf, uint8_t maxLen) const;

    bool operator==(const GingoTimeSig& other) const {
        return beatsPerBar_ == other.beatsPerBar_ && beatUnit_ == other.beatUnit_;
    }
    bool operator!=(const GingoTimeSig& other) const { return !(*this == other); }

private:
    uint8_t beatsPerBar_;
    uint8_t beatUnit_;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_TIMESIG
#endif // GINGO_TIMESIG_H

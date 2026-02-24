// Gingoduino — Music Theory Library for Embedded Systems
// GingoTempo: beats per minute and time conversions.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_TEMPO_H
#define GINGO_TEMPO_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_TEMPO

#include "gingoduino_types.h"

namespace gingoduino {

class GingoDuration;  // forward declaration

/// Represents a musical tempo in beats per minute (BPM).
///
/// Converts durations to absolute time (seconds) and provides
/// standard tempo marking names (Largo, Andante, Allegro, etc.).
///
/// Examples:
///   GingoTempo t(120);               // 120 BPM
///   t.bpm();                         // 120.0
///   char buf[14]; t.marking(buf, 14); // "Allegro"
///   t.msPerBeat();                   // 500.0
class GingoTempo {
public:
    /// Construct from BPM value.
    explicit GingoTempo(float bpm = 120.0f);

    /// Construct from a standard tempo marking name.
    /// Valid: "Grave", "Largo", "Adagio", "Andante", "Moderato",
    ///        "Allegretto", "Allegro", "Vivace", "Presto", "Prestissimo"
    explicit GingoTempo(const char* marking);

    /// Beats per minute.
    float bpm() const { return bpm_; }

    /// Standard tempo marking name (closest match).
    /// Writes to buffer and returns it.
    const char* marking(char* buf, uint8_t maxLen) const;

    /// Convert a duration to seconds.
    /// Assumes quarter note = 1 beat.
    float seconds(const GingoDuration& duration) const;

    /// Milliseconds per beat (useful for MIDI timing).
    float msPerBeat() const { return 60000.0f / bpm_; }

    /// Seconds per beat.
    float secondsPerBeat() const { return 60.0f / bpm_; }

    bool operator==(const GingoTempo& other) const { return bpm_ == other.bpm_; }
    bool operator!=(const GingoTempo& other) const { return bpm_ != other.bpm_; }

    // -- Static utilities ---------------------------------------------------

    /// Map BPM to the closest standard tempo marking.
    static void bpmToMarking(float bpm, char* buf, uint8_t maxLen);

    /// Map tempo marking to typical BPM (midpoint of range).
    /// Returns -1 if unknown.
    static float markingToBpm(const char* marking);

private:
    float bpm_;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_TEMPO
#endif // GINGO_TEMPO_H

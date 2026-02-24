// Gingoduino — Music Theory Library for Embedded Systems
// GingoSequence: a timeline of musical events with tempo and meter.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_SEQUENCE_H
#define GINGO_SEQUENCE_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_SEQUENCE

#include "gingoduino_types.h"
#include "GingoEvent.h"
#include "GingoTempo.h"
#include "GingoTimeSig.h"

namespace gingoduino {

/// A fixed-capacity timeline of musical events.
///
/// Capacity is controlled by GINGODUINO_MAX_EVENTS (default 64).
///
/// Examples:
///   GingoSequence seq(GingoTempo(120), GingoTimeSig(4, 4));
///   seq.add(GingoEvent::noteEvent(GingoNote("C"), GingoDuration("quarter"), 4));
///   seq.add(GingoEvent::rest(GingoDuration("quarter")));
///   seq.totalBeats();   // 2.0
///   seq.totalSeconds(); // ~1.0 at 120 BPM
class GingoSequence {
public:
    /// Construct with tempo and time signature.
    GingoSequence(const GingoTempo& tempo = GingoTempo(120),
                  const GingoTimeSig& timeSig = GingoTimeSig(4, 4));

    /// Add an event to the end of the sequence.
    /// Returns false if the sequence is full.
    bool add(const GingoEvent& event);

    /// Remove the event at the given index.
    /// Returns false if the index is out of range.
    bool remove(uint8_t index);

    /// Remove all events.
    void clear();

    /// Number of events in the sequence.
    uint8_t size() const { return count_; }

    /// Whether the sequence is empty.
    bool empty() const { return count_ == 0; }

    /// Access an event by index (0-based). Returns rest on out-of-range.
    const GingoEvent& at(uint8_t index) const;

    /// Total duration in beats.
    float totalBeats() const;

    /// Total duration in seconds (based on tempo).
    float totalSeconds() const;

    /// Number of full bars.
    float barCount() const;

    /// Transpose all events by a number of semitones.
    void transpose(int8_t semitones);

    /// Serialize all events to raw MIDI bytes (NoteOn/NoteOff pairs).
    /// Returns total number of bytes written.
    /// Stops early if buffer is too small for an event.
    uint16_t toMIDI(uint8_t* buf, uint16_t maxLen, uint8_t channel = 1) const;

    /// Current tempo.
    const GingoTempo& tempo() const { return tempo_; }

    /// Set a new tempo.
    void setTempo(const GingoTempo& tempo) { tempo_ = tempo; }

    /// Current time signature.
    const GingoTimeSig& timeSignature() const { return timeSig_; }

    /// Set a new time signature.
    void setTimeSignature(const GingoTimeSig& ts) { timeSig_ = ts; }

private:
    GingoEvent  events_[GINGODUINO_MAX_EVENTS];
    uint8_t     count_;
    GingoTempo  tempo_;
    GingoTimeSig timeSig_;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_SEQUENCE
#endif // GINGO_SEQUENCE_H

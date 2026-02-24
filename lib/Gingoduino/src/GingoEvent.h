// Gingoduino — Music Theory Library for Embedded Systems
// GingoEvent: a musical event (note, chord, or rest) with duration.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_EVENT_H
#define GINGO_EVENT_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_EVENT

#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoChord.h"
#include "GingoDuration.h"

namespace gingoduino {

/// Event type tag.
enum EventType : uint8_t {
    EVENT_NOTE,
    EVENT_CHORD,
    EVENT_REST
};

/// A musical event: a note, chord, or rest bound to a duration.
///
/// Uses a tagged union (no heap, no std::variant).
///
/// Examples:
///   auto e = GingoEvent::noteEvent(GingoNote("C"), GingoDuration("quarter"), 4);
///   e.type();          // EVENT_NOTE
///   e.note().name();   // "C"
///   e.midiNumber();    // 60
///   e.velocity();      // 100 (default)
///   e.midiChannel();   // 1 (default)
///
///   auto r = GingoEvent::rest(GingoDuration("half"));
///   r.type();          // EVENT_REST
class GingoEvent {
public:
    /// Default constructor (quarter-note rest, velocity 100, channel 1).
    GingoEvent();

    /// Factory: create a note event with optional velocity and channel.
    static GingoEvent noteEvent(const GingoNote& note,
                                const GingoDuration& duration,
                                uint8_t octave = 4,
                                uint8_t velocity = 100,
                                uint8_t midiChannel = 1);

    /// Factory: create a chord event with optional velocity and channel.
    static GingoEvent chordEvent(const GingoChord& chord,
                                 const GingoDuration& duration,
                                 uint8_t octave = 4,
                                 uint8_t velocity = 100,
                                 uint8_t midiChannel = 1);

    /// Factory: create a rest event.
    static GingoEvent rest(const GingoDuration& duration);

    /// Factory: create a note event from MIDI number.
    /// octave is extracted from midiNote automatically.
    /// Duration defaults to quarter note.
    static GingoEvent fromMIDI(uint8_t midiNote,
                               const GingoDuration& duration = GingoDuration("quarter"),
                               uint8_t velocity = 100,
                               uint8_t midiChannel = 1);

    /// The event type.
    EventType type() const { return type_; }

    /// The note (valid only for EVENT_NOTE).
    const GingoNote& note() const { return note_; }

    /// The chord (valid only for EVENT_CHORD).
    const GingoChord& chord() const { return chord_; }

    /// The duration of this event.
    const GingoDuration& duration() const { return duration_; }

    /// The octave (valid for EVENT_NOTE and EVENT_CHORD).
    uint8_t octave() const { return octave_; }

    /// MIDI velocity (0-127, default 100).
    uint8_t velocity() const { return velocity_; }

    /// MIDI channel (1-16, default 1).
    uint8_t midiChannel() const { return midiChannel_; }

    /// Set velocity (clamped to 0-127).
    void setVelocity(uint8_t v) { velocity_ = v & 0x7F; }

    /// Set MIDI channel (clamped to 1-16, defaults to 1 if invalid).
    void setMidiChannel(uint8_t ch) { midiChannel_ = (ch > 0 && ch <= 16) ? ch : 1; }

    /// MIDI number of the note (EVENT_NOTE) or chord root (EVENT_CHORD).
    uint8_t midiNumber() const;

    /// Frequency in Hz (EVENT_NOTE) or root frequency (EVENT_CHORD).
    float frequency() const;

    /// Transpose the event by a number of semitones.
    GingoEvent transpose(int8_t semitones) const;

    /// Serialize to raw MIDI bytes (NoteOn + NoteOff for notes, nothing for rests).
    /// Uses velocity() and midiChannel() from this event.
    /// Returns number of bytes written (6 for note event, 0 for rest).
    /// buf must have at least 6 bytes available.
    /// For chord events, encodes the root note only.
    uint8_t toMIDI(uint8_t* buf) const;

private:
    EventType    type_;
    GingoNote    note_;
    GingoChord   chord_;
    GingoDuration duration_;
    uint8_t      octave_;
    uint8_t      velocity_;      // MIDI velocity (0-127)
    uint8_t      midiChannel_;   // MIDI channel (1-16)
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_EVENT
#endif // GINGO_EVENT_H

// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoFretboard.
//
// SPDX-License-Identifier: MIT

#include "GingoFretboard.h"

#if GINGODUINO_HAS_FRETBOARD

#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GingoFretboard::GingoFretboard(const char* name,
                               const uint8_t* openMidi,
                               uint8_t numStrings,
                               uint8_t numFrets)
    : numStrings_(numStrings > GINGODUINO_MAX_STRINGS ? GINGODUINO_MAX_STRINGS : numStrings)
    , numFrets_(numFrets)
    , capoFret_(0)
{
    name_.set(name);
    for (uint8_t i = 0; i < numStrings_; i++) {
        openMidi_[i] = pgm_read_byte(&openMidi[i]);
    }
}

// ---------------------------------------------------------------------------
// Factories
// ---------------------------------------------------------------------------

GingoFretboard GingoFretboard::violao(uint8_t numFrets) {
    return GingoFretboard("Violao", data::TUNING_VIOLAO, 6, numFrets);
}

GingoFretboard GingoFretboard::cavaquinho(uint8_t numFrets) {
    return GingoFretboard("Cavaquinho", data::TUNING_CAVAQUINHO, 4, numFrets);
}

GingoFretboard GingoFretboard::bandolim(uint8_t numFrets) {
    return GingoFretboard("Bandolim", data::TUNING_BANDOLIM, 4, numFrets);
}

GingoFretboard GingoFretboard::ukulele(uint8_t numFrets) {
    return GingoFretboard("Ukulele", data::TUNING_UKULELE, 4, numFrets);
}

// ---------------------------------------------------------------------------
// Info
// ---------------------------------------------------------------------------

uint8_t GingoFretboard::openMidi(uint8_t string) const {
    if (string >= numStrings_) return 0;
    return openMidi_[string] + capoFret_;
}

// ---------------------------------------------------------------------------
// Forward lookups
// ---------------------------------------------------------------------------

GingoFretPos GingoFretboard::position(uint8_t string, uint8_t fret) const {
    GingoFretPos pos;
    pos.string = string;
    pos.fret = fret;
    if (string < numStrings_) {
        pos.midi = openMidi_[string] + capoFret_ + fret;
    } else {
        pos.midi = 0;
    }
    char name[3];
    data::readChromaticName(pos.midi % 12, name, sizeof(name));
    pos.note = GingoNote(name);
    pos.octave = (pos.midi / 12) - 1;
    return pos;
}

GingoNote GingoFretboard::noteAt(uint8_t string, uint8_t fret) const {
    uint8_t midi = 0;
    if (string < numStrings_) {
        midi = openMidi_[string] + capoFret_ + fret;
    }
    char name[3];
    data::readChromaticName(midi % 12, name, sizeof(name));
    return GingoNote(name);
}

uint8_t GingoFretboard::midiAt(uint8_t string, uint8_t fret) const {
    if (string >= numStrings_) return 0;
    return openMidi_[string] + capoFret_ + fret;
}

uint8_t GingoFretboard::positions(const GingoNote& note,
                                  GingoFretPos* output, uint8_t maxPositions) const {
    uint8_t written = 0;
    uint8_t targetPc = note.semitone();

    for (uint8_t s = 0; s < numStrings_ && written < maxPositions; s++) {
        uint8_t baseMidi = openMidi_[s] + capoFret_;
        for (uint8_t f = 0; f <= numFrets_ && written < maxPositions; f++) {
            uint8_t midi = baseMidi + f;
            if (midi % 12 == targetPc) {
                GingoFretPos& p = output[written++];
                p.string = s;
                p.fret = f;
                p.midi = midi;
                p.note = note;
                p.octave = (midi / 12) - 1;
            }
        }
    }
    return written;
}

uint8_t GingoFretboard::scalePositions(const GingoScale& scale,
                                       GingoFretPos* output, uint8_t maxPositions,
                                       uint8_t fretLo, uint8_t fretHi) const {
    if (fretHi > numFrets_) fretHi = numFrets_;
    uint8_t written = 0;

    for (uint8_t s = 0; s < numStrings_ && written < maxPositions; s++) {
        uint8_t baseMidi = openMidi_[s] + capoFret_;
        for (uint8_t f = fretLo; f <= fretHi && written < maxPositions; f++) {
            uint8_t midi = baseMidi + f;
            char name[3];
            data::readChromaticName(midi % 12, name, sizeof(name));
            GingoNote n(name);
            if (scale.contains(n)) {
                GingoFretPos& p = output[written++];
                p.string = s;
                p.fret = f;
                p.midi = midi;
                p.note = n;
                p.octave = (midi / 12) - 1;
            }
        }
    }
    return written;
}

// ---------------------------------------------------------------------------
// Fingerings
// ---------------------------------------------------------------------------

uint16_t GingoFretboard::scoreFingering(const GingoFingering& fg) const {
    uint16_t score = 0;
    uint8_t minFret = 255, maxFret = 0;
    uint8_t mutedCount = 0;

    for (uint8_t i = 0; i < fg.numStrings; i++) {
        if (fg.strings[i].action == STRING_MUTED) {
            mutedCount++;
            score += 10;
        } else if (fg.strings[i].action == STRING_FRETTED) {
            uint8_t f = fg.strings[i].fret;
            if (f < minFret) minFret = f;
            if (f > maxFret) maxFret = f;
        }
    }

    // Span penalty
    if (maxFret > minFret) {
        score += (uint16_t)(maxFret - minFret) * 5;
    }

    // Position penalty: higher frets are harder
    if (minFret != 255) {
        score += minFret;
    }

    // Prefer more sounding notes
    if (mutedCount > 0) {
        score += (uint16_t)mutedCount * 8;
    }

    return score;
}

bool GingoFretboard::fingering(const GingoChord& chord, uint8_t positionIdx,
                               GingoFingering& output) const {
    // Get chord notes
    GingoNote chordNotes[7];
    uint8_t chordSize = chord.notes(chordNotes, 7);
    if (chordSize == 0) return false;

    // Determine the fret window based on positionIdx
    uint8_t windowStart = positionIdx * 4;
    uint8_t windowEnd = windowStart + 4;
    if (windowEnd > numFrets_) windowEnd = numFrets_;
    if (windowStart > numFrets_) return false;

    // Initialize output
    output.numStrings = numStrings_;
    output.chordName.set(chord.name());
    output.baseFret = windowStart;
    output.capoFret = capoFret_;
    output.numNotes = 0;

    // For each string, find the best fret within the window
    // that matches a chord tone
    for (uint8_t s = 0; s < numStrings_; s++) {
        uint8_t baseMidi = openMidi_[s] + capoFret_;
        bool found = false;

        // Check open string first
        uint8_t openPc = baseMidi % 12;
        for (uint8_t n = 0; n < chordSize; n++) {
            if (chordNotes[n].semitone() == openPc) {
                output.strings[s].string = s;
                output.strings[s].action = STRING_OPEN;
                output.strings[s].fret = 0;
                output.midiNotes[output.numNotes++] = baseMidi;
                found = true;
                break;
            }
        }

        if (!found) {
            // Check frets in window
            for (uint8_t f = (windowStart > 0 ? windowStart : 1); f <= windowEnd; f++) {
                uint8_t midi = baseMidi + f;
                uint8_t pc = midi % 12;
                for (uint8_t n = 0; n < chordSize; n++) {
                    if (chordNotes[n].semitone() == pc) {
                        output.strings[s].string = s;
                        output.strings[s].action = STRING_FRETTED;
                        output.strings[s].fret = f;
                        output.midiNotes[output.numNotes++] = midi;
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }

        if (!found) {
            output.strings[s].string = s;
            output.strings[s].action = STRING_MUTED;
            output.strings[s].fret = 0;
        }
    }

    output.score = scoreFingering(output);
    return output.numNotes >= chordSize;
}

uint8_t GingoFretboard::fingerings(const GingoChord& chord,
                                   GingoFingering* output, uint8_t maxResults) const {
    uint8_t written = 0;
    uint8_t maxWindows = (numFrets_ / 4) + 1;

    // Generate fingerings for each position window
    GingoFingering candidates[GINGODUINO_MAX_FINGERINGS + 4];
    uint8_t candidateCount = 0;

    for (uint8_t w = 0; w < maxWindows && candidateCount < GINGODUINO_MAX_FINGERINGS + 4; w++) {
        GingoFingering fg;
        if (fingering(chord, w, fg)) {
            candidates[candidateCount++] = fg;
        }
    }

    // Sort by score (insertion sort, small array)
    for (uint8_t i = 1; i < candidateCount; i++) {
        GingoFingering key = candidates[i];
        int8_t j = (int8_t)i - 1;
        while (j >= 0 && candidates[j].score > key.score) {
            candidates[j + 1] = candidates[j];
            j--;
        }
        candidates[j + 1] = key;
    }

    // Copy best results
    for (uint8_t i = 0; i < candidateCount && written < maxResults; i++) {
        output[written++] = candidates[i];
    }

    return written;
}

// ---------------------------------------------------------------------------
// Reverse: identify chord from fret positions
// ---------------------------------------------------------------------------

bool GingoFretboard::identify(const uint8_t* stringFrets, uint8_t count,
                              char* output, uint8_t maxLen) const {
    GingoNote notes[GINGODUINO_MAX_STRINGS];
    uint8_t noteCount = 0;

    for (uint8_t i = 0; i < count && i < numStrings_; i++) {
        if (stringFrets[i] == 255) continue;  // muted
        uint8_t midi = openMidi_[i] + capoFret_ + stringFrets[i];
        char name[3];
        data::readChromaticName(midi % 12, name, sizeof(name));
        notes[noteCount++] = GingoNote(name);
    }

    if (noteCount == 0) {
        output[0] = '\0';
        return false;
    }

    return GingoChord::identify(notes, noteCount, output, maxLen);
}

// ---------------------------------------------------------------------------
// Capo
// ---------------------------------------------------------------------------

GingoFretboard GingoFretboard::capo(uint8_t fret) const {
    GingoFretboard fb = *this;
    fb.capoFret_ = fret;
    fb.numFrets_ = (fret < numFrets_) ? (numFrets_ - fret) : 0;
    return fb;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_FRETBOARD

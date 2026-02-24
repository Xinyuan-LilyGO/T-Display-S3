// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoChord.
//
// SPDX-License-Identifier: MIT

#include "GingoChord.h"
#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Constructor helpers
// ---------------------------------------------------------------------------

void GingoChord::resolveFormula() {
    if (type_.empty()) {
        // No type suffix means Major
        formulaIdx_ = 0;
        return;
    }
    int8_t idx = data::findChordType(type_.c_str());
    if (idx >= 0) {
        formulaIdx_ = pgm_read_byte(&data::CHORD_TYPE_MAP[idx].formulaIdx);
    } else {
        formulaIdx_ = 255; // unknown
    }
}

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GingoChord::GingoChord()
    : name_("CM"), rootStr_("C"), type_("M"), formulaIdx_(0)
{}

GingoChord::GingoChord(const char* name)
    : formulaIdx_(255)
{
    name_.set(name);

    // Extract root
    char rootBuf[5];
    GingoNote::extractRoot(name, rootBuf, sizeof(rootBuf));
    rootStr_.set(rootBuf);

    // Extract type
    char typeBuf[10];
    GingoNote::extractType(name, typeBuf, sizeof(typeBuf));
    type_.set(typeBuf);

    resolveFormula();
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

uint8_t GingoChord::size() const {
    if (formulaIdx_ == 255) return 0;
    return pgm_read_byte(&data::CHORD_FORMULAS[formulaIdx_].count);
}

uint8_t GingoChord::notes(GingoNote* output, uint8_t maxNotes) const {
    if (formulaIdx_ == 255 || !output) return 0;

    // Read formula from PROGMEM
    uint8_t intervals[7];
    uint8_t count;
    data::readChordFormula(formulaIdx_, intervals, &count);

    // Get root semitone
    uint8_t rootSt = GingoNote::toSemitone(rootStr_.c_str());

    uint8_t written = 0;
    for (uint8_t i = 0; i < count && written < maxNotes; i++) {
        uint8_t noteSt = (rootSt + intervals[i]) % 12;
        char noteName[3];
        data::readChromaticName(noteSt, noteName, sizeof(noteName));
        output[written++] = GingoNote(noteName);
    }
    return written;
}

uint8_t GingoChord::intervalLabels(LabelStr* output, uint8_t maxLabels) const {
    if (formulaIdx_ == 255 || !output) return 0;

    uint8_t intervals[7];
    uint8_t count;
    data::readChordFormula(formulaIdx_, intervals, &count);

    uint8_t written = 0;
    for (uint8_t i = 0; i < count && written < maxLabels; i++) {
        char labelBuf[5];
        data::readPgmStr(labelBuf, data::INTERVAL_TABLE[intervals[i]].label, sizeof(labelBuf));
        output[written++].set(labelBuf);
    }
    return written;
}

bool GingoChord::contains(const GingoNote& note) const {
    if (formulaIdx_ == 255) return false;

    uint8_t intervals[7];
    uint8_t count;
    data::readChordFormula(formulaIdx_, intervals, &count);

    uint8_t rootSt = GingoNote::toSemitone(rootStr_.c_str());
    uint8_t targetSt = note.semitone();

    for (uint8_t i = 0; i < count; i++) {
        if ((rootSt + intervals[i]) % 12 == targetSt) return true;
    }
    return false;
}

GingoChord GingoChord::transpose(int8_t semitones) const {
    GingoNote newRoot = GingoNote(rootStr_.c_str()).transpose(semitones);
    char newName[16];
    uint8_t pos = 0;
    const char* rn = newRoot.name();
    while (rn[pos] && pos < 14) {
        newName[pos] = rn[pos];
        pos++;
    }
    const char* tp = type_.c_str();
    uint8_t ti = 0;
    while (tp[ti] && pos < 15) {
        newName[pos++] = tp[ti++];
    }
    newName[pos] = '\0';
    return GingoChord(newName);
}

uint8_t GingoChord::intervals(GingoInterval* output, uint8_t maxIntervals) const {
    if (formulaIdx_ == 255 || !output) return 0;

    uint8_t rawIntervals[7];
    uint8_t count;
    data::readChordFormula(formulaIdx_, rawIntervals, &count);

    uint8_t written = 0;
    for (uint8_t i = 0; i < count && written < maxIntervals; i++) {
        output[written++] = GingoInterval(rawIntervals[i]);
    }
    return written;
}

// ---------------------------------------------------------------------------
// Identify
// ---------------------------------------------------------------------------

bool GingoChord::identify(const GingoNote* notes, uint8_t count,
                          char* output, uint8_t maxLen) {
    if (!notes || count == 0 || !output || maxLen < 2) return false;

    // Get root semitone (first note)
    uint8_t rootSt = notes[0].semitone();

    // Compute interval offsets from root
    uint8_t detected[7];
    for (uint8_t i = 0; i < count && i < 7; i++) {
        detected[i] = (notes[i].semitone() - rootSt + 12) % 12;
        // For extended chords, handle second octave
        if (i > 0 && detected[i] <= detected[i - 1]) {
            detected[i] += 12;
        }
    }

    // Compare against every formula
    for (uint8_t fi = 0; fi < 42; fi++) {
        uint8_t fIntervals[7];
        uint8_t fCount;
        data::readChordFormula(fi, fIntervals, &fCount);

        if (fCount != count) continue;

        bool match = true;
        for (uint8_t j = 0; j < fCount; j++) {
            if (detected[j] != fIntervals[j]) {
                match = false;
                break;
            }
        }

        if (match) {
            // Found! Build chord name: root + canonical type name
            const char* rootName = notes[0].name();
            uint8_t pos = 0;
            while (rootName[pos] && pos < maxLen - 1) {
                output[pos] = rootName[pos];
                pos++;
            }

            // Find canonical name for this formula index
            // (shortest name that maps to this index)
            char bestName[10] = "";
            uint8_t bestLen = 255;
            for (uint8_t a = 0; a < data::CHORD_TYPE_MAP_SIZE; a++) {
                uint8_t idx = pgm_read_byte(&data::CHORD_TYPE_MAP[a].formulaIdx);
                if (idx == fi) {
                    char aliasName[10];
                    data::readPgmStr(aliasName, data::CHORD_TYPE_MAP[a].name, sizeof(aliasName));
                    uint8_t alen = (uint8_t)strlen(aliasName);
                    // Prefer shortest alphanumeric name
                    bool hasAlpha = false;
                    for (uint8_t c = 0; c < alen; c++) {
                        if ((aliasName[c] >= 'A' && aliasName[c] <= 'Z') ||
                            (aliasName[c] >= 'a' && aliasName[c] <= 'z') ||
                            (aliasName[c] >= '0' && aliasName[c] <= '9')) {
                            hasAlpha = true;
                            break;
                        }
                    }
                    if (hasAlpha && alen < bestLen) {
                        bestLen = alen;
                        for (uint8_t c = 0; c <= alen; c++) bestName[c] = aliasName[c];
                    }
                }
            }

            // Append type name
            uint8_t ti = 0;
            while (bestName[ti] && pos < maxLen - 1) {
                output[pos++] = bestName[ti++];
            }
            output[pos] = '\0';
            return true;
        }
    }

    output[0] = '\0';
    return false;
}

} // namespace gingoduino

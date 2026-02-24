// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoInterval.
//
// SPDX-License-Identifier: MIT

#include "GingoInterval.h"
#include "GingoNote.h"
#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------

uint8_t GingoInterval::labelToSemitones(const char* label) {
    if (!label) return 255;
    for (uint8_t i = 0; i < 24; i++) {
        char buf[5];
        data::readPgmStr(buf, data::INTERVAL_TABLE[i].label, sizeof(buf));
        if (strcmp(label, buf) == 0) return i;
    }
    return 255;
}

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GingoInterval::GingoInterval(const char* label) {
    uint8_t st = labelToSemitones(label);
    index_ = (st != 255) ? st : 0;
}

GingoInterval::GingoInterval(uint8_t semitones)
    : index_(semitones < 24 ? semitones : 0)
{}

GingoInterval::GingoInterval(const GingoNote& from, const GingoNote& to) {
    index_ = (uint8_t)((to.semitone() - from.semitone() + 12) % 12);
}

// ---------------------------------------------------------------------------
// Accessors (read from PROGMEM)
// ---------------------------------------------------------------------------

const char* GingoInterval::label(char* buf, uint8_t maxLen) const {
    data::readPgmStr(buf, data::INTERVAL_TABLE[index_].label, maxLen);
    return buf;
}

const char* GingoInterval::angloSaxon(char* buf, uint8_t maxLen) const {
    data::readPgmStr(buf, data::INTERVAL_TABLE[index_].anglo, maxLen);
    return buf;
}

uint8_t GingoInterval::degree() const {
    return pgm_read_byte(&data::INTERVAL_TABLE[index_].degree);
}

uint8_t GingoInterval::octave() const {
    return pgm_read_byte(&data::INTERVAL_TABLE[index_].octave);
}

GingoInterval GingoInterval::simple() const {
    if (index_ <= 11) return *this;
    return GingoInterval((uint8_t)(index_ - 12));
}

GingoInterval GingoInterval::invert() const {
    uint8_t s = index_;
    if (s > 11) s -= 12; // reduce to simple
    return GingoInterval((uint8_t)(12 - s));
}

const char* GingoInterval::consonance(char* buf, uint8_t maxLen) const {
    uint8_t s = index_ % 12;
    uint8_t c = pgm_read_byte(&data::INTERVAL_CONSONANCE[s]);
    const char* pgmPtr = (const char*)pgm_read_ptr(&data::CONSONANCE_NAMES[c]);
    data::readPgmStr(buf, pgmPtr, maxLen);
    return buf;
}

bool GingoInterval::isConsonant() const {
    uint8_t s = index_ % 12;
    uint8_t c = pgm_read_byte(&data::INTERVAL_CONSONANCE[s]);
    return c < 2;  // perfect(0) or imperfect(1)
}

const char* GingoInterval::fullName(char* buf, uint8_t maxLen) const {
    const char* pgmPtr = (const char*)pgm_read_ptr(&data::INTERVAL_FULL_NAMES_EN[index_]);
    data::readPgmStr(buf, pgmPtr, maxLen);
    return buf;
}

const char* GingoInterval::fullNamePt(char* buf, uint8_t maxLen) const {
    const char* pgmPtr = (const char*)pgm_read_ptr(&data::INTERVAL_FULL_NAMES_PT[index_]);
    data::readPgmStr(buf, pgmPtr, maxLen);
    return buf;
}

GingoInterval GingoInterval::operator+(const GingoInterval& other) const {
    uint8_t sum = index_ + other.index_;
    if (sum > 23) sum = 23;
    return GingoInterval(sum);
}

GingoInterval GingoInterval::operator-(const GingoInterval& other) const {
    if (other.index_ >= index_) return GingoInterval((uint8_t)0);
    return GingoInterval((uint8_t)(index_ - other.index_));
}

} // namespace gingoduino

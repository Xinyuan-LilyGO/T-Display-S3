// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoDuration.
//
// SPDX-License-Identifier: MIT

#include "GingoDuration.h"

#if GINGODUINO_HAS_DURATION

#include "gingoduino_progmem.h"

namespace gingoduino {

GingoDuration::GingoDuration()
    : nameIdx_(2), dots_(0), tuplet_(0), numerator_(1), denominator_(4)
{}

GingoDuration::GingoDuration(const char* name, uint8_t dots, uint8_t tuplet)
    : nameIdx_(2), dots_(dots), tuplet_(tuplet)
{
    // Find name in DURATION_NAMES table
    if (name) {
        for (uint8_t i = 0; i < data::DURATION_NAMES_SIZE; i++) {
            char buf[16];
            data::readPgmStr(buf, data::DURATION_NAMES[i].name, sizeof(buf));
            if (strcmp(name, buf) == 0) {
                nameIdx_ = i;
                break;
            }
        }
    }
    computeRational();
}

GingoDuration::GingoDuration(int16_t num, int16_t den)
    : nameIdx_(255), dots_(0), tuplet_(0), numerator_(num), denominator_(den)
{
    // Try to match to a known duration name
    for (uint8_t i = 0; i < data::DURATION_NAMES_SIZE; i++) {
        uint8_t dn = pgm_read_byte(&data::DURATION_NAMES[i].numerator);
        uint8_t dd = pgm_read_byte(&data::DURATION_NAMES[i].denominator);
        if ((int16_t)dn * den == num * (int16_t)dd) {
            nameIdx_ = i;
            break;
        }
    }
}

void GingoDuration::computeRational() {
    // Base rational from name
    if (nameIdx_ < data::DURATION_NAMES_SIZE) {
        numerator_   = pgm_read_byte(&data::DURATION_NAMES[nameIdx_].numerator);
        denominator_ = pgm_read_byte(&data::DURATION_NAMES[nameIdx_].denominator);
    } else {
        numerator_ = 1;
        denominator_ = 4;
    }

    // Apply dots: dotted = base * (2 - 1/2^dots)
    // 1 dot:  3/2 of original → num*3, den*2
    // 2 dots: 7/4 of original → num*7, den*4
    if (dots_ == 1) {
        numerator_ = (int16_t)(numerator_ * 3);
        denominator_ = (int16_t)(denominator_ * 2);
    } else if (dots_ == 2) {
        numerator_ = (int16_t)(numerator_ * 7);
        denominator_ = (int16_t)(denominator_ * 4);
    }

    // Apply tuplet: divide by tuplet ratio
    // Triplet (3): multiply denominator by 3, numerator by 2
    if (tuplet_ == 3) {
        numerator_ = (int16_t)(numerator_ * 2);
        denominator_ = (int16_t)(denominator_ * 3);
    } else if (tuplet_ == 5) {
        numerator_ = (int16_t)(numerator_ * 4);
        denominator_ = (int16_t)(denominator_ * 5);
    } else if (tuplet_ == 7) {
        numerator_ = (int16_t)(numerator_ * 4);
        denominator_ = (int16_t)(denominator_ * 7);
    }
}

const char* GingoDuration::name(char* buf, uint8_t maxLen) const {
    if (nameIdx_ < data::DURATION_NAMES_SIZE) {
        data::readPgmStr(buf, data::DURATION_NAMES[nameIdx_].name, maxLen);
    } else {
        buf[0] = '\0';
    }
    return buf;
}

float GingoDuration::beats(float referenceValue) const {
    // beats = (numerator / denominator) / (1/4) * referenceValue
    // = 4 * numerator / denominator * referenceValue
    return 4.0f * (float)numerator_ / (float)denominator_ * referenceValue;
}

// ---------------------------------------------------------------------------
// Arithmetic
// ---------------------------------------------------------------------------

static int16_t gcd16(int16_t a, int16_t b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { int16_t t = b; b = a % b; a = t; }
    return a;
}

GingoDuration GingoDuration::operator+(const GingoDuration& other) const {
    // a/b + c/d = (a*d + c*b) / (b*d)
    int16_t num = (int16_t)(numerator_ * other.denominator_ + other.numerator_ * denominator_);
    int16_t den = (int16_t)(denominator_ * other.denominator_);
    int16_t g = gcd16(num, den);
    if (g > 1) { num /= g; den /= g; }
    return GingoDuration(num, den);
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_DURATION

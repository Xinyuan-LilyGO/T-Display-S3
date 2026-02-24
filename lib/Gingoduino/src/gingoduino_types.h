// Gingoduino — Music Theory Library for Embedded Systems
// Base types: FixedStr, FixedArray — zero-heap replacements for std::string/vector.
//
// SPDX-License-Identifier: MIT

#ifndef GINGODUINO_TYPES_H
#define GINGODUINO_TYPES_H

#include "gingoduino_config.h"

#include <string.h>

namespace gingoduino {

// ---------------------------------------------------------------------------
// FixedStr<N> — fixed-capacity string, no heap allocation
// ---------------------------------------------------------------------------

template<uint8_t N>
struct FixedStr {
    char    data[N + 1];
    uint8_t len;

    FixedStr() : len(0) { data[0] = '\0'; }

    FixedStr(const char* s) {
        set(s);
    }

    void set(const char* s) {
        if (!s) { len = 0; data[0] = '\0'; return; }
        len = 0;
        while (len < N && s[len]) {
            data[len] = s[len];
            len++;
        }
        data[len] = '\0';
    }

    void setFromPROGMEM(const char* s) {
        if (!s) { len = 0; data[0] = '\0'; return; }
        len = 0;
        char c;
        while (len < N && (c = (char)pgm_read_byte(s + len)) != '\0') {
            data[len] = c;
            len++;
        }
        data[len] = '\0';
    }

    void clear() { len = 0; data[0] = '\0'; }

    bool append(char c) {
        if (len >= N) return false;
        data[len++] = c;
        data[len] = '\0';
        return true;
    }

    bool append(const char* s) {
        if (!s) return true;
        while (*s && len < N) {
            data[len++] = *s++;
        }
        data[len] = '\0';
        return (*s == '\0');
    }

    const char* c_str() const { return data; }
    operator const char*() const { return data; }
    uint8_t size() const { return len; }
    bool empty() const { return len == 0; }
    char operator[](uint8_t i) const { return (i < len) ? data[i] : '\0'; }

    bool operator==(const FixedStr& other) const {
        return (len == other.len) && (strcmp(data, other.data) == 0);
    }
    bool operator!=(const FixedStr& other) const { return !(*this == other); }

    bool operator==(const char* s) const {
        return strcmp(data, s ? s : "") == 0;
    }
    bool operator!=(const char* s) const { return !(*this == s); }
};

// Common aliases
using NoteStr  = FixedStr<5>;    // "C##\0" — max 4 chars + null
using LabelStr = FixedStr<10>;   // "m7(b5)\0" — max 9 chars + null
using NameStr  = FixedStr<16>;   // "Bbm7(b5)\0" — max 15 chars + null

// ---------------------------------------------------------------------------
// FixedArray<T, MaxN> — fixed-capacity array, no heap allocation
// ---------------------------------------------------------------------------

template<typename T, uint8_t MaxN>
struct FixedArray {
    T       items[MaxN];
    uint8_t count;

    FixedArray() : count(0) {}

    bool push(const T& item) {
        if (count >= MaxN) return false;
        items[count++] = item;
        return true;
    }

    void clear() { count = 0; }

    T& operator[](uint8_t i) { return items[i]; }
    const T& operator[](uint8_t i) const { return items[i]; }

    uint8_t size() const { return count; }
    bool empty() const { return count == 0; }
    bool full() const { return count >= MaxN; }

    T* begin() { return items; }
    T* end()   { return items + count; }
    const T* begin() const { return items; }
    const T* end()   const { return items + count; }
};

// ---------------------------------------------------------------------------
// Scale / Chord type enums
// ---------------------------------------------------------------------------

enum ScaleType : uint8_t {
    SCALE_MAJOR          = 0,
    SCALE_NATURAL_MINOR  = 1,
    SCALE_HARMONIC_MINOR = 2,
    SCALE_MELODIC_MINOR  = 3,
    SCALE_DIMINISHED     = 4,
    SCALE_HARMONIC_MAJOR = 5,
    SCALE_WHOLE_TONE     = 6,
    SCALE_AUGMENTED      = 7,
    SCALE_BLUES          = 8,
    SCALE_CHROMATIC      = 9,
    SCALE_TYPE_COUNT     = 10
};

enum HarmonicFunc : uint8_t {
    FUNC_TONIC       = 0,
    FUNC_SUBDOMINANT = 1,
    FUNC_DOMINANT    = 2
};

} // namespace gingoduino

#endif // GINGODUINO_TYPES_H

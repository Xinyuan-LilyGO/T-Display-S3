#ifndef MIDI2_SUPPORT_H
#define MIDI2_SUPPORT_H

// MIDI2Support — Universal MIDI Packet (UMP) definitions, scaling utilities
// and parser for MIDI 2.0 on ESP32.
//
// Status: FOUNDATION / EXPERIMENTAL
//   The data layer and parsing utilities are complete. Physical transport
//   support depends on ecosystem readiness:
//   - USB MIDI 2.0 : pending arduino-esp32 TinyUSB update
//   - RTP-MIDI 2.0 : spec finalised (RFC draft), library support in progress
//   - BLE MIDI 2.0 : spec published, no Arduino library yet
//   Track progress: github.com/sauloverissimo/ESP32_Host_MIDI
//
// What you can do today:
//   1. Scale MIDI 1.0 values to MIDI 2.0 resolution (and back).
//   2. Build / parse UMP packets for use with future transports.
//   3. Receive high-resolution UMP data from another ESP32 over raw UDP/ESP-NOW.
//
// Spec references:
//   MIDI 2.0 Specification (M2-104-UM)   — midi.org
//   Universal MIDI Packet (UMP) Format   — midi.org/specifications
//   MIDI-CI Specification (M2-101-UM)    — midi.org

#include <Arduino.h>
#include <cstdint>
#include <cstring>

// ---- UMP Message Types (MT) — upper nibble of first byte ---------------
enum UMPMessageType : uint8_t {
    UMP_MT_UTILITY          = 0x0,  // Utility messages (clock, etc.)
    UMP_MT_SYSTEM           = 0x1,  // System Real-Time / System Common
    UMP_MT_MIDI1_VOICE      = 0x2,  // MIDI 1.0 Channel Voice (32-bit)
    UMP_MT_DATA_64          = 0x3,  // SysEx / Data (64-bit)
    UMP_MT_MIDI2_VOICE      = 0x4,  // MIDI 2.0 Channel Voice (64-bit)  ← main type
    UMP_MT_DATA_128         = 0x5,  // SysEx8 / Mixed Data (128-bit)
};

// ---- MIDI 2.0 Channel Voice opcodes (Type 4) ---------------------------
enum MIDI2Opcode : uint8_t {
    MIDI2_OP_REGISTERED_PER_NOTE_CTRL  = 0x0,
    MIDI2_OP_ASSIGNABLE_PER_NOTE_CTRL  = 0x1,
    MIDI2_OP_REGISTERED_CTRL           = 0x2,
    MIDI2_OP_ASSIGNABLE_CTRL           = 0x3,
    MIDI2_OP_REL_REGISTERED_CTRL       = 0x4,
    MIDI2_OP_REL_ASSIGNABLE_CTRL       = 0x5,
    MIDI2_OP_PER_NOTE_PITCH_BEND       = 0x6,
    MIDI2_OP_NOTE_OFF                  = 0x8,
    MIDI2_OP_NOTE_ON                   = 0x9,
    MIDI2_OP_POLY_PRESSURE             = 0xA,
    MIDI2_OP_CONTROL_CHANGE            = 0xB,
    MIDI2_OP_PROGRAM_CHANGE            = 0xC,
    MIDI2_OP_CHANNEL_PRESSURE          = 0xD,
    MIDI2_OP_PITCH_BEND                = 0xE,
    MIDI2_OP_PER_NOTE_MGMT             = 0xF,
};

// ---- UMP packet containers ---------------------------------------------

// Single 32-bit word (MIDI 1.0 in UMP, Utility, System)
struct UMPWord32 {
    uint32_t raw;  // named 'raw' to avoid collision with Arduino's word() macro
    UMPWord32() : raw(0) {}
    explicit UMPWord32(uint32_t w) : raw(w) {}

    uint8_t messageType() const { return (raw >> 28) & 0x0F; }
    uint8_t group()       const { return (raw >> 24) & 0x0F; }
    uint8_t statusByte()  const { return (raw >> 16) & 0xFF; }  // MIDI status (MT=2)
    uint8_t data1()       const { return (raw >>  8) & 0x7F; }
    uint8_t data2()       const { return (raw >>  0) & 0x7F; }
};

// Two 32-bit words (MIDI 2.0 Channel Voice = 64-bit)
struct UMPWord64 {
    uint32_t word0;
    uint32_t word1;
    UMPWord64() : word0(0), word1(0) {}
    UMPWord64(uint32_t w0, uint32_t w1) : word0(w0), word1(w1) {}

    uint8_t  messageType() const { return (word0 >> 28) & 0x0F; }
    uint8_t  group()       const { return (word0 >> 24) & 0x0F; }
    uint8_t  opcode()      const { return (word0 >> 20) & 0x0F; }
    uint8_t  channel()     const { return (word0 >> 16) & 0x0F; }  // 0-indexed
    uint8_t  index()       const { return (word0 >>  8) & 0xFF; }  // note / controller
    uint8_t  optionFlags() const { return (word0 >>  0) & 0xFF; }  // attribute type etc.
    uint32_t data()        const { return word1; }                  // 32-bit value
    uint16_t dataHi()      const { return (word1 >> 16) & 0xFFFF; }
    uint16_t dataLo()      const { return (word1 >>  0) & 0xFFFF; }
};

// ---- Decoded MIDI 2.0 message ------------------------------------------

struct MIDI2NoteMessage {
    uint8_t  group;
    uint8_t  channel;    // 0-15
    uint8_t  note;       // 0-127
    uint16_t velocity;   // 16-bit (0=off, 65535=max)  — MIDI 1.0 maps 7-bit here
    uint16_t attribute;  // attribute value (if attributeType != 0)
    uint8_t  attributeType;
    bool     isNoteOn;
};

struct MIDI2CCMessage {
    uint8_t  group;
    uint8_t  channel;
    uint8_t  index;      // controller number (0-127)
    uint32_t value;      // 32-bit  — MIDI 1.0 maps 7-bit × 2^25
};

struct MIDI2PitchBendMessage {
    uint8_t  group;
    uint8_t  channel;
    uint32_t value;      // 32-bit, center = 0x80000000  — MIDI 1.0 maps 14-bit
};

struct MIDI2PressureMessage {
    uint8_t  group;
    uint8_t  channel;
    uint8_t  note;       // 0 = channel pressure, 1-127 = polyphonic
    uint32_t value;      // 32-bit
};

// ---- Value scaling utilities -------------------------------------------
// Formulas from MIDI 2.0 Specification, Section 4 (Scaling)

class MIDI2Scaler {
public:
    // MIDI 1.0 → MIDI 2.0 expansion

    // 7-bit (0-127) → 16-bit (0-65535)
    static uint16_t scale7to16(uint8_t v) {
        if (v == 0)   return 0;
        if (v == 127) return 0xFFFF;
        return (uint16_t)((uint32_t)v << 9) |
               (uint16_t)((uint32_t)v << 2) |
               (uint8_t)(v >> 5);
    }

    // 7-bit (0-127) → 32-bit (0-4294967295)
    static uint32_t scale7to32(uint8_t v) {
        if (v == 0)   return 0;
        if (v == 127) return 0xFFFFFFFF;
        return ((uint32_t)v << 25) | ((uint32_t)v << 18) |
               ((uint32_t)v << 11) | ((uint32_t)v <<  4) |
               (v >> 3);
    }

    // 14-bit pitch bend (0-16383, center=8192) → 32-bit (center=0x80000000)
    static uint32_t scale14to32(uint16_t v14) {
        if (v14 == 0)     return 0;
        if (v14 == 16383) return 0xFFFFFFFF;
        return ((uint32_t)v14 << 18) | ((uint32_t)v14 << 4) | (v14 >> 10);
    }

    // MIDI 2.0 → MIDI 1.0 compression

    // 16-bit → 7-bit
    static uint8_t scale16to7(uint16_t v) { return (uint8_t)(v >> 9); }

    // 32-bit → 7-bit
    static uint8_t scale32to7(uint32_t v) { return (uint8_t)(v >> 25); }

    // 32-bit → 14-bit pitch bend
    static uint16_t scale32to14(uint32_t v) { return (uint16_t)(v >> 18); }

    // Convenience: MIDI 1.0 pitch bend (14-bit signed) → 32-bit UMP
    static uint32_t pitchBend14to32(int pb14) {
        uint16_t u14 = (uint16_t)(pb14 + 8192);   // shift to unsigned
        return scale14to32(u14);
    }

    // 32-bit UMP pitch bend → MIDI 1.0 signed (-8192 … +8191)
    static int pitchBend32to14(uint32_t v32) {
        return (int)scale32to14(v32) - 8192;
    }
};

// ---- UMP Packet builder ------------------------------------------------

class UMPBuilder {
public:
    // Build a MIDI 1.0-in-UMP (Type 2, 32-bit) from raw MIDI bytes
    static UMPWord32 fromMIDI1(uint8_t group, const uint8_t* data, uint8_t len) {
        uint32_t w = ((uint32_t)UMP_MT_MIDI1_VOICE << 28) |
                     ((uint32_t)(group & 0x0F) << 24) |
                     ((uint32_t)data[0] << 16);
        if (len >= 2) w |= ((uint32_t)data[1] << 8);
        if (len >= 3) w |= (uint32_t)data[2];
        return UMPWord32(w);
    }

    // Build a MIDI 2.0 Note On (Type 4, 64-bit)
    static UMPWord64 noteOn(uint8_t group, uint8_t channel, uint8_t note,
                            uint16_t velocity, uint8_t attrType = 0,
                            uint16_t attrValue = 0) {
        uint32_t w0 = ((uint32_t)UMP_MT_MIDI2_VOICE << 28) |
                      ((uint32_t)(group   & 0x0F) << 24) |
                      ((uint32_t)(MIDI2_OP_NOTE_ON & 0x0F) << 20) |
                      ((uint32_t)(channel & 0x0F) << 16) |
                      ((uint32_t)(note    & 0x7F) <<  8) |
                      (uint32_t)attrType;
        uint32_t w1 = ((uint32_t)velocity << 16) | (uint32_t)attrValue;
        return UMPWord64(w0, w1);
    }

    // Build a MIDI 2.0 Note Off (Type 4, 64-bit)
    static UMPWord64 noteOff(uint8_t group, uint8_t channel, uint8_t note,
                             uint16_t velocity) {
        uint32_t w0 = ((uint32_t)UMP_MT_MIDI2_VOICE << 28) |
                      ((uint32_t)(group   & 0x0F) << 24) |
                      ((uint32_t)(MIDI2_OP_NOTE_OFF & 0x0F) << 20) |
                      ((uint32_t)(channel & 0x0F) << 16) |
                      ((uint32_t)(note    & 0x7F) <<  8);
        uint32_t w1 = ((uint32_t)velocity << 16);
        return UMPWord64(w0, w1);
    }

    // Build a MIDI 2.0 Control Change (32-bit value)
    static UMPWord64 controlChange(uint8_t group, uint8_t channel,
                                   uint8_t index, uint32_t value) {
        uint32_t w0 = ((uint32_t)UMP_MT_MIDI2_VOICE << 28) |
                      ((uint32_t)(group   & 0x0F) << 24) |
                      ((uint32_t)(MIDI2_OP_CONTROL_CHANGE & 0x0F) << 20) |
                      ((uint32_t)(channel & 0x0F) << 16) |
                      ((uint32_t)(index   & 0x7F) <<  8);
        return UMPWord64(w0, value);
    }

    // Build a MIDI 2.0 Pitch Bend (32-bit value, center = 0x80000000)
    static UMPWord64 pitchBend(uint8_t group, uint8_t channel, uint32_t value) {
        uint32_t w0 = ((uint32_t)UMP_MT_MIDI2_VOICE << 28) |
                      ((uint32_t)(group   & 0x0F) << 24) |
                      ((uint32_t)(MIDI2_OP_PITCH_BEND & 0x0F) << 20) |
                      ((uint32_t)(channel & 0x0F) << 16);
        return UMPWord64(w0, value);
    }
};

// ---- UMP Parser --------------------------------------------------------

// Parsed result from a UMP stream.
struct UMPResult {
    bool        valid;
    bool        isMIDI2;        // true = Type 4 (MIDI 2.0), false = Type 2 (MIDI 1.0 in UMP)
    uint8_t     opcode;         // MIDI2Opcode or MIDI status nibble
    uint8_t     group;
    uint8_t     channel;        // 0-indexed
    uint8_t     note;           // note number or controller index
    uint32_t    value;          // 32-bit high-res value (MIDI 2.0)
    uint8_t     midi1[3];       // raw MIDI 1.0 bytes (MIDI 1.0 in UMP or scaled-down)
    uint8_t     midi1Len;
};

class UMPParser {
public:
    // Parse a 32-bit UMP word (MIDI 1.0 in UMP, Type 2).
    static UMPResult parseMIDI1(const UMPWord32& pkt) {
        UMPResult r;
        memset(&r, 0, sizeof(r));
        if (pkt.messageType() != UMP_MT_MIDI1_VOICE) { r.valid = false; return r; }
        r.valid      = true;
        r.isMIDI2    = false;
        r.group      = pkt.group();
        r.channel    = pkt.statusByte() & 0x0F;
        r.opcode     = (pkt.statusByte() >> 4) & 0x0F;
        r.midi1[0]   = pkt.statusByte();
        r.midi1[1]   = pkt.data1();
        r.midi1[2]   = pkt.data2();
        r.midi1Len   = _midi1Len(r.opcode);
        // Scale data byte(s) to 32-bit for uniform access
        r.value      = MIDI2Scaler::scale7to32(pkt.data1());
        r.note       = pkt.data1();
        return r;
    }

    // Parse a 64-bit MIDI 2.0 Channel Voice packet (Type 4).
    static UMPResult parseMIDI2(const UMPWord64& pkt) {
        UMPResult r;
        memset(&r, 0, sizeof(r));
        if (pkt.messageType() != UMP_MT_MIDI2_VOICE) { r.valid = false; return r; }
        r.valid   = true;
        r.isMIDI2 = true;
        r.group   = pkt.group();
        r.opcode  = pkt.opcode();
        r.channel = pkt.channel();
        r.note    = pkt.index();
        r.value   = pkt.data();

        // Scale down to MIDI 1.0 bytes for interoperability
        r.midi1[0] = (uint8_t)((r.opcode << 4) | (r.channel & 0x0F));
        r.midi1Len = _ump4MsgLen(r.opcode);

        switch ((MIDI2Opcode)r.opcode) {
            case MIDI2_OP_NOTE_ON:
            case MIDI2_OP_NOTE_OFF:
                r.midi1[1] = r.note;
                r.midi1[2] = MIDI2Scaler::scale16to7((uint16_t)(pkt.dataHi()));
                break;
            case MIDI2_OP_CONTROL_CHANGE:
                r.midi1[1] = r.note;
                r.midi1[2] = MIDI2Scaler::scale32to7(r.value);
                break;
            case MIDI2_OP_PITCH_BEND: {
                uint16_t pb14 = MIDI2Scaler::scale32to14(r.value);
                r.midi1[1] = pb14 & 0x7F;
                r.midi1[2] = (pb14 >> 7) & 0x7F;
                break;
            }
            case MIDI2_OP_PROGRAM_CHANGE:
                r.midi1[1] = (uint8_t)(r.value >> 24);  // program number in MSB
                r.midi1Len = 2;
                break;
            case MIDI2_OP_CHANNEL_PRESSURE:
            case MIDI2_OP_POLY_PRESSURE:
                r.midi1[1] = r.note;
                r.midi1[2] = MIDI2Scaler::scale32to7(r.value);
                break;
            default:
                break;
        }
        return r;
    }

private:
    static uint8_t _midi1Len(uint8_t statusNibble) {
        switch (statusNibble) {
            case 0x8: case 0x9: case 0xA: case 0xB: case 0xE: return 3;
            case 0xC: case 0xD: return 2;
            default: return 1;
        }
    }
    static uint8_t _ump4MsgLen(uint8_t opcode) {
        switch ((MIDI2Opcode)opcode) {
            case MIDI2_OP_NOTE_ON:
            case MIDI2_OP_NOTE_OFF:
            case MIDI2_OP_POLY_PRESSURE:
            case MIDI2_OP_CONTROL_CHANGE:
            case MIDI2_OP_PITCH_BEND:
            case MIDI2_OP_PER_NOTE_PITCH_BEND:
                return 3;
            case MIDI2_OP_PROGRAM_CHANGE:
            case MIDI2_OP_CHANNEL_PRESSURE:
                return 2;
            default:
                return 1;
        }
    }
};

#endif // MIDI2_SUPPORT_H

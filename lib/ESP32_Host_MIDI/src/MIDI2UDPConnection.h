#ifndef MIDI2_UDP_CONNECTION_H
#define MIDI2_UDP_CONNECTION_H

// MIDI2UDPConnection — UMP over UDP transport for ESP32
//
// Enables bidirectional MIDI 2.0 (Universal MIDI Packet) between two ESP32
// devices over WiFi UDP. Uses a custom 12-byte protocol specific to
// ESP32_Host_MIDI — NOT compatible with RTP-MIDI, standard MIDI, or any
// other protocol.
//
// Protocol: 12-byte UDP datagram
//   Bytes  0-3:  Magic 0x55 0x4D 0x50 0x32 ("UMP2")
//   Bytes  4-7:  UMP Word 0 (big-endian uint32)
//   Bytes 8-11:  UMP Word 1 (big-endian uint32, 0x00000000 for 32-bit packets)
//
// What this gives over MIDI 1.0:
//   NoteOn/Off velocity  :  7-bit  (128 levels)  → 16-bit (65 536 levels)
//   Control Change value :  7-bit  (128 steps)   → 32-bit (4 294 967 296 steps)
//   Pitch Bend           : 14-bit  (16 384 steps) → 32-bit (4 294 967 296 steps)
//
// The library automatically scales values in both directions so all existing
// callbacks and queue entries remain compatible with MIDI 1.0 code.
// High-resolution MIDI 2.0 values are accessible via lastResult().
//
// Signal path (send):
//   MIDI 1.0 bytes → scale up → UMP Type 4 (64-bit) → UDP → peer ESP32
//
// Signal path (receive):
//   UDP → validate magic → UMPParser::parseMIDI2() → scale down → dispatchMidiData()
//   Raw 32-bit values accessible via lastResult() immediately after task().
//
// Prerequisites:
//   1. #include "MIDI2Support.h"  must appear BEFORE this file.
//   2. WiFi must be connected before calling begin().
//   3. This transport uses WiFiUDP — no additional library required.
//
// Typical usage:
//   #include "../../src/MIDI2Support.h"
//   #include "../../src/MIDI2UDPConnection.h"
//
//   MIDI2UDPConnection midi2udp;
//
//   void setup() {
//       WiFi.begin(SSID, PASS);
//       while (WiFi.status() != WL_CONNECTED) delay(500);
//       midi2udp.begin(LOCAL_PORT, IPAddress(192,168,1,42), REMOTE_PORT);
//       midiHandler.addTransport(&midi2udp);
//       midiHandler.begin();
//   }
//
//   void loop() {
//       midiHandler.task();
//       const auto& q = midiHandler.getQueue();
//       for (const auto& ev : q) {
//           if (ev.index <= lastIndex) continue;
//           lastIndex = ev.index;
//           // Standard MIDI 1.0 values in ev.velocity, ev.note, etc.
//           // MIDI 2.0 high-res values via:
//           const UMPResult& r = midi2udp.lastResult();
//           if (r.valid && r.isMIDI2) { /* r.value = 32-bit */ }
//       }
//   }

#ifndef MIDI2_SUPPORT_H
  #error "MIDI2UDPConnection.h requires MIDI2Support.h — #include it before this file."
#endif

#include <WiFi.h>
#include <WiFiUdp.h>
#include "MIDITransport.h"

// Default ports — override in mapping.h before including this file.
#ifndef MIDI2_UDP_LOCAL_PORT
  #define MIDI2_UDP_LOCAL_PORT  5006
#endif
#ifndef MIDI2_UDP_REMOTE_PORT
  #define MIDI2_UDP_REMOTE_PORT 5006
#endif

// Packet magic: ASCII "UMP2"
static const uint8_t _MIDI2UDP_MAGIC[4] = { 0x55, 0x4D, 0x50, 0x32 };

class MIDI2UDPConnection : public MIDITransport {
public:
    inline static MIDI2UDPConnection* _instance = nullptr;

    MIDI2UDPConnection()
        : _initialized(false),
          _localPort(MIDI2_UDP_LOCAL_PORT),
          _targetPort(MIDI2_UDP_REMOTE_PORT)
    {
        _instance = this;
        memset(&_lastResult, 0, sizeof(_lastResult));
    }

    // begin() — initialise the UDP socket and set the target peer.
    //
    // localPort  : UDP port to listen on (default MIDI2_UDP_LOCAL_PORT / 5006)
    // targetIP   : Peer ESP32 IP (e.g. IPAddress(192,168,1,42)).
    //              Pass IPAddress(0,0,0,0) to receive-only (no send).
    // targetPort : UDP port of the peer (default MIDI2_UDP_REMOTE_PORT / 5006)
    //
    // Returns true on success. Call WiFi.begin() and wait for WL_CONNECTED
    // before calling begin().
    bool begin(int localPort     = MIDI2_UDP_LOCAL_PORT,
               IPAddress targetIP = IPAddress(0, 0, 0, 0),
               int targetPort    = MIDI2_UDP_REMOTE_PORT)
    {
        if (_initialized) return true;
        _localPort  = localPort;
        _targetIP   = targetIP;
        _targetPort = targetPort;
        if (_udp.begin(localPort) != 1) return false;
        _initialized = true;
        dispatchConnected();
        return true;
    }

    // task() — receive pending UDP packets and dispatch MIDI to midiHandler.
    // Called automatically by midiHandler.task().
    void task() override {
        if (!_initialized) return;

        int size = _udp.parsePacket();
        if (size < 12) return;  // discard fragments and non-MIDI2UDP traffic

        uint8_t buf[12];
        if (_udp.read(buf, sizeof(buf)) < 12) return;

        // Validate magic "UMP2"
        if (memcmp(buf, _MIDI2UDP_MAGIC, 4) != 0) return;

        // Decode UMP words (big-endian on the wire)
        uint32_t w0 = ((uint32_t)buf[4]  << 24) | ((uint32_t)buf[5]  << 16) |
                      ((uint32_t)buf[6]  <<  8) |  (uint32_t)buf[7];
        uint32_t w1 = ((uint32_t)buf[8]  << 24) | ((uint32_t)buf[9]  << 16) |
                      ((uint32_t)buf[10] <<  8) |  (uint32_t)buf[11];

        uint8_t mt = (w0 >> 28) & 0x0F;

        if (mt == UMP_MT_MIDI2_VOICE) {
            // Type 4 — MIDI 2.0 Channel Voice (64-bit)
            UMPWord64 pkt(w0, w1);
            _lastResult = UMPParser::parseMIDI2(pkt);
            if (_lastResult.valid && _lastResult.midi1Len > 0) {
                dispatchMidiData(_lastResult.midi1, _lastResult.midi1Len);
            }
        } else if (mt == UMP_MT_MIDI1_VOICE) {
            // Type 2 — MIDI 1.0 in UMP (32-bit, w1 unused)
            UMPWord32 pkt(w0);
            _lastResult = UMPParser::parseMIDI1(pkt);
            if (_lastResult.valid && _lastResult.midi1Len > 0) {
                dispatchMidiData(_lastResult.midi1, _lastResult.midi1Len);
            }
        }
        // Other UMP message types are silently ignored.
    }

    // isConnected() — true when WiFi is up and begin() has been called.
    bool isConnected() const override {
        return _initialized && (WiFi.status() == WL_CONNECTED);
    }

    // sendMidiMessage() — scales MIDI 1.0 bytes up to MIDI 2.0 resolution and
    // sends a 12-byte UMP-over-UDP datagram to the configured target peer.
    //
    // Supported message types (others are not sent):
    //   NoteOn  (0x9n) : velocity 7→16-bit
    //   NoteOff (0x8n) : velocity 7→16-bit
    //   CC      (0xBn) : value   7→32-bit
    //   PitchBend(0xEn): 14→32-bit
    //
    // Returns false if no target IP is set or WiFi is down.
    bool sendMidiMessage(const uint8_t* data, size_t length) override {
        if (!_initialized || !data || length < 1) return false;
        if (!isConnected()) return false;

        // Require a valid target IP
        if ((uint32_t)_targetIP == 0) return false;

        uint8_t status  = data[0] & 0xF0;
        uint8_t channel = data[0] & 0x0F;  // 0-indexed in UMP

        UMPWord64 ump;
        bool valid = false;

        switch (status) {
            case 0x90:  // Note On
                if (length >= 3) {
                    ump   = UMPBuilder::noteOn(0, channel, data[1],
                                               MIDI2Scaler::scale7to16(data[2]));
                    valid = true;
                }
                break;

            case 0x80:  // Note Off
                if (length >= 3) {
                    ump   = UMPBuilder::noteOff(0, channel, data[1],
                                                MIDI2Scaler::scale7to16(data[2]));
                    valid = true;
                }
                break;

            case 0xB0:  // Control Change
                if (length >= 3) {
                    ump   = UMPBuilder::controlChange(0, channel, data[1],
                                                      MIDI2Scaler::scale7to32(data[2]));
                    valid = true;
                }
                break;

            case 0xE0:  // Pitch Bend (14-bit: data[1] = LSB 7-bit, data[2] = MSB 7-bit)
                if (length >= 3) {
                    uint16_t pb14 = (uint16_t)(((uint16_t)data[2] << 7) | data[1]);
                    ump   = UMPBuilder::pitchBend(0, channel,
                                                  MIDI2Scaler::scale14to32(pb14));
                    valid = true;
                }
                break;

            default:
                break;
        }

        return valid && _sendUMP(ump.word0, ump.word1);
    }

    // lastResult() — the UMPResult from the most recently received packet.
    // Access the 32-bit MIDI 2.0 value via result.value.
    // For NoteOn/Off: 32-bit value holds velocity in the upper 16 bits
    //   → uint16_t vel16 = (uint16_t)(result.value >> 16)
    // For CC          : result.value is the full 32-bit controller value.
    // For PitchBend   : result.value is the 32-bit pitch bend (center=0x80000000).
    //
    // Note: lastResult() is overwritten on every received packet.
    // Access it immediately after the queue event that triggered it.
    const UMPResult& lastResult() const { return _lastResult; }

    // setTarget() — change the peer IP/port after begin().
    void setTarget(IPAddress ip, int port = MIDI2_UDP_REMOTE_PORT) {
        _targetIP   = ip;
        _targetPort = port;
    }

private:
    bool      _initialized;
    WiFiUDP   _udp;
    int       _localPort;
    IPAddress _targetIP;
    int       _targetPort;
    UMPResult _lastResult;

    bool _sendUMP(uint32_t w0, uint32_t w1) {
        uint8_t buf[12];

        // Magic "UMP2"
        buf[0] = 0x55;  buf[1] = 0x4D;  buf[2] = 0x50;  buf[3] = 0x32;

        // Word 0 — big-endian
        buf[4]  = (w0 >> 24) & 0xFF;
        buf[5]  = (w0 >> 16) & 0xFF;
        buf[6]  = (w0 >>  8) & 0xFF;
        buf[7]  =  w0        & 0xFF;

        // Word 1 — big-endian
        buf[8]  = (w1 >> 24) & 0xFF;
        buf[9]  = (w1 >> 16) & 0xFF;
        buf[10] = (w1 >>  8) & 0xFF;
        buf[11] =  w1        & 0xFF;

        _udp.beginPacket(_targetIP, _targetPort);
        _udp.write(buf, 12);
        return _udp.endPacket() == 1;
    }
};

#endif // MIDI2_UDP_CONNECTION_H

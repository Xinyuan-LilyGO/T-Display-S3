#ifndef OSC_CONNECTION_H
#define OSC_CONNECTION_H

// OSCConnection — bidirectional OSC ↔ MIDI bridge over WiFi UDP.
//
// Header-only implementation: include this file in ONE translation unit only
// (the sketch's .ino). Users who do not use OSC simply do not include this
// header — the OSC library is not required for them.
//
// Usage:
//   #include "OSCConnection.h"  // requires CNMAT/OSC library
//
// Requires:
//   - CNMAT/OSC library
//     Arduino IDE : Library Manager → search "OSC" by Adrian Freed / CNMAT
//     PlatformIO  : lib_deps = CNMAT/OSC
//   - WiFi (built-in on ESP32)
//
// OSC address map (prefix configurable via OSC_MIDI_PREFIX):
//   /midi/noteon      channel note velocity
//   /midi/noteoff     channel note velocity
//   /midi/cc          channel controller value
//   /midi/pc          channel program
//   /midi/pitchbend   channel bend        (-8192 … 8191)
//   /midi/aftertouch  channel pressure
//
// Compile-time overrides — define before including this header:
//   #define OSC_MIDI_PREFIX   "/midi"
//   #define OSC_LOCAL_PORT    8000
//   #define OSC_REMOTE_PORT   9000

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>
#include "MIDITransport.h"

#ifndef OSC_MIDI_PREFIX
  #define OSC_MIDI_PREFIX "/midi"
#endif

#ifndef OSC_LOCAL_PORT
  #define OSC_LOCAL_PORT 8000
#endif

#ifndef OSC_REMOTE_PORT
  #define OSC_REMOTE_PORT 9000
#endif

class OSCConnection : public MIDITransport {
public:
    inline static OSCConnection* _instance = nullptr;

    OSCConnection() : _initialized(false), _hasTarget(false),
                      _targetPort(OSC_REMOTE_PORT) {}

    // Opens the UDP socket and starts listening for incoming OSC messages.
    //   localPort : UDP port to listen on (default OSC_LOCAL_PORT = 8000).
    //   targetIP  : IP address to send OSC messages to. Pass IPAddress(0,0,0,0)
    //               (default) for receive-only mode (no outbound messages).
    //   targetPort: remote UDP port to send to (default OSC_REMOTE_PORT = 9000).
    // WiFi must already be connected before calling begin().
    // Returns false if the UDP socket could not be opened.
    bool begin(int localPort     = OSC_LOCAL_PORT,
               IPAddress targetIP  = IPAddress(0, 0, 0, 0),
               int targetPort    = OSC_REMOTE_PORT)
    {
        if (_initialized) return true;

        _instance   = this;
        _targetIP   = targetIP;
        _targetPort = targetPort;
        _hasTarget  = (targetIP != IPAddress(0, 0, 0, 0));

        if (_udp.begin(localPort) == 0) return false;

        _initialized = true;
        return true;
    }

    // Polls the UDP socket for incoming OSC messages and dispatches them
    // as MIDI events. Call from loop().
    void task() override {
        if (!_initialized) return;

        int size = _udp.parsePacket();
        if (size <= 0) return;

        OSCMessage msg;
        while (size--) msg.fill(_udp.read());

        if (msg.hasError()) return;

        msg.dispatch(OSC_MIDI_PREFIX "/noteon",     _onNoteOn);
        msg.dispatch(OSC_MIDI_PREFIX "/noteoff",    _onNoteOff);
        msg.dispatch(OSC_MIDI_PREFIX "/cc",         _onCC);
        msg.dispatch(OSC_MIDI_PREFIX "/pc",         _onPC);
        msg.dispatch(OSC_MIDI_PREFIX "/pitchbend",  _onPitchBend);
        msg.dispatch(OSC_MIDI_PREFIX "/aftertouch", _onAfterTouch);
    }

    // Returns true while the WiFi connection is up and the socket is open.
    bool isConnected() const override {
        return _initialized && (WiFi.status() == WL_CONNECTED);
    }

    // Converts raw MIDI bytes to an OSC message and sends it to targetIP:targetPort.
    // Returns false if no target was configured or WiFi is down.
    bool sendMidiMessage(const uint8_t* data, size_t length) override {
        if (!_initialized || !_hasTarget || length < 1) return false;
        if (WiFi.status() != WL_CONNECTED) return false;

        uint8_t status  = data[0] & 0xF0;
        uint8_t channel = (data[0] & 0x0F) + 1;

        const char* addr = nullptr;
        OSCMessage msg("");  // address set below

        switch (status) {
            case 0x90:
                if (length < 3) return false;
                addr = OSC_MIDI_PREFIX "/noteon";
                { OSCMessage m(addr); m.add((int32_t)channel).add((int32_t)data[1]).add((int32_t)data[2]); _send(m); }
                return true;
            case 0x80:
                if (length < 3) return false;
                { OSCMessage m(OSC_MIDI_PREFIX "/noteoff"); m.add((int32_t)channel).add((int32_t)data[1]).add((int32_t)data[2]); _send(m); }
                return true;
            case 0xB0:
                if (length < 3) return false;
                { OSCMessage m(OSC_MIDI_PREFIX "/cc"); m.add((int32_t)channel).add((int32_t)data[1]).add((int32_t)data[2]); _send(m); }
                return true;
            case 0xC0:
                if (length < 2) return false;
                { OSCMessage m(OSC_MIDI_PREFIX "/pc"); m.add((int32_t)channel).add((int32_t)data[1]); _send(m); }
                return true;
            case 0xD0:
                if (length < 2) return false;
                { OSCMessage m(OSC_MIDI_PREFIX "/aftertouch"); m.add((int32_t)channel).add((int32_t)data[1]); _send(m); }
                return true;
            case 0xE0:
                if (length < 3) return false;
                {
                    int bend = (int)(data[1] | ((uint16_t)data[2] << 7)) - 8192;
                    OSCMessage m(OSC_MIDI_PREFIX "/pitchbend");
                    m.add((int32_t)channel).add((int32_t)bend);
                    _send(m);
                }
                return true;
            default:
                return false;
        }
    }

    // Returns the IP address assigned to this device on the WiFi network.
    IPAddress localIP() const { return WiFi.localIP(); }

private:
    bool       _initialized;
    bool       _hasTarget;
    WiFiUDP    _udp;
    IPAddress  _targetIP;
    int        _targetPort;

    void _send(OSCMessage& msg) {
        _udp.beginPacket(_targetIP, _targetPort);
        msg.send(_udp);
        _udp.endPacket();
        msg.empty();
    }

    // ---- Incoming OSC → MIDI dispatch callbacks -------------------------

    static void _onNoteOn(OSCMessage& msg) {
        if (!_instance || msg.size() < 3) return;
        uint8_t ch  = (uint8_t)msg.getInt(0);
        uint8_t d[3] = { (uint8_t)(0x90 | ((ch - 1) & 0x0F)),
                         (uint8_t)msg.getInt(1),
                         (uint8_t)msg.getInt(2) };
        _instance->dispatchMidiData(d, 3);
    }
    static void _onNoteOff(OSCMessage& msg) {
        if (!_instance || msg.size() < 3) return;
        uint8_t ch  = (uint8_t)msg.getInt(0);
        uint8_t d[3] = { (uint8_t)(0x80 | ((ch - 1) & 0x0F)),
                         (uint8_t)msg.getInt(1),
                         (uint8_t)msg.getInt(2) };
        _instance->dispatchMidiData(d, 3);
    }
    static void _onCC(OSCMessage& msg) {
        if (!_instance || msg.size() < 3) return;
        uint8_t ch  = (uint8_t)msg.getInt(0);
        uint8_t d[3] = { (uint8_t)(0xB0 | ((ch - 1) & 0x0F)),
                         (uint8_t)msg.getInt(1),
                         (uint8_t)msg.getInt(2) };
        _instance->dispatchMidiData(d, 3);
    }
    static void _onPC(OSCMessage& msg) {
        if (!_instance || msg.size() < 2) return;
        uint8_t ch  = (uint8_t)msg.getInt(0);
        uint8_t d[2] = { (uint8_t)(0xC0 | ((ch - 1) & 0x0F)),
                         (uint8_t)msg.getInt(1) };
        _instance->dispatchMidiData(d, 2);
    }
    static void _onPitchBend(OSCMessage& msg) {
        if (!_instance || msg.size() < 2) return;
        uint8_t ch   = (uint8_t)msg.getInt(0);
        int     bend = msg.getInt(1);
        uint16_t val = (uint16_t)(bend + 8192);
        uint8_t d[3] = { (uint8_t)(0xE0 | ((ch - 1) & 0x0F)),
                         (uint8_t)(val & 0x7F),
                         (uint8_t)((val >> 7) & 0x7F) };
        _instance->dispatchMidiData(d, 3);
    }
    static void _onAfterTouch(OSCMessage& msg) {
        if (!_instance || msg.size() < 2) return;
        uint8_t ch  = (uint8_t)msg.getInt(0);
        uint8_t d[2] = { (uint8_t)(0xD0 | ((ch - 1) & 0x0F)),
                         (uint8_t)msg.getInt(1) };
        _instance->dispatchMidiData(d, 2);
    }
};

#endif // OSC_CONNECTION_H

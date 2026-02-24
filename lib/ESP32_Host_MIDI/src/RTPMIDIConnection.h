#ifndef RTPMIDI_CONNECTION_H
#define RTPMIDI_CONNECTION_H

// RTPMIDIConnection — Apple MIDI (RTP-MIDI, RFC 6295) over WiFi.
//
// Header-only implementation: no separate .cpp, so this file is compiled
// only when explicitly included. Users who do not use RTP-MIDI simply do
// not include this header — the AppleMIDI library is not required for them.
//
// Usage:
//   #include "RTPMIDIConnection.h"   // requires AppleMIDI to be installed
//
// Requires: lathoub/Arduino-AppleMIDI-Library v3.x
//   Arduino IDE : Library Manager → search "AppleMIDI" by lathoub
//   PlatformIO  : lib_deps = lathoub/Arduino-AppleMIDI-Library
//
// WiFi must already be connected before calling begin().
//
// Compile-time overrides — define before including this header:
//   #define RTP_MIDI_PORT        5004          // UDP port (IANA standard)
//   #define RTP_MIDI_DEVICE_NAME "My ESP32"    // name in Audio MIDI Setup

#include <Arduino.h>
#include <WiFi.h>
#include <AppleMIDI.h>   // lathoub/Arduino-AppleMIDI-Library v3.x
#include <ESPmDNS.h>
#include "MIDITransport.h"

#ifndef RTP_MIDI_PORT
  #define RTP_MIDI_PORT 5004
#endif

#ifndef RTP_MIDI_DEVICE_NAME
  #define RTP_MIDI_DEVICE_NAME "ESP32 MIDI"
#endif

// AppleMIDI session and MIDI interface — scoped to avoid symbol conflicts.
// Defined here (header-only): safe as long as this header is included in
// only ONE translation unit (the sketch's .ino). Do not include in multiple
// .cpp files of the same project.
//
// APPLEMIDI_CREATE_INSTANCE(WiFiUDP, midi, name, port) expands to:
//   AppleMIDISession<WiFiUDP>    Applemidi(name, port);   // session mgmt
//   MidiInterface<...>           midi(...);                // MIDI I/O
namespace _rtpMidiInternal {
    APPLEMIDI_CREATE_INSTANCE(WiFiUDP, midi, RTP_MIDI_DEVICE_NAME, RTP_MIDI_PORT);
}

// File-scope session callbacks — must be defined before the class uses them.
static void _rtpOnConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name);
static void _rtpOnDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc);

class RTPMIDIConnection : public MIDITransport {
public:
    // C++17 inline static — no separate .cpp definition needed.
    inline static RTPMIDIConnection* _instance = nullptr;

    RTPMIDIConnection() : _initialized(false), _connectedCount(0) {}

    // Opens the RTP-MIDI session and registers with mDNS for auto-discovery.
    // WiFi must be connected before calling this.
    //   name: label shown in macOS/iOS Audio MIDI Setup (≤24 chars).
    //         Pass nullptr to use the RTP_MIDI_DEVICE_NAME compile-time default.
    // Returns false if WiFi is not connected.
    bool begin(const char* name = nullptr) {
        if (_initialized) return true;
        if (WiFi.status() != WL_CONNECTED) return false;

        _instance = this;

        const char* deviceName = (name && name[0]) ? name : RTP_MIDI_DEVICE_NAME;
        _rtpMidiInternal::Applemidi.setName(deviceName);

        MDNS.begin(deviceName);
        MDNS.addService("apple-midi", "udp", RTP_MIDI_PORT);

        // Session-level callbacks (AppleMIDI object)
        _rtpMidiInternal::Applemidi.setHandleConnected(_rtpOnConnected);
        _rtpMidiInternal::Applemidi.setHandleDisconnected(_rtpOnDisconnected);

        // MIDI message callbacks (MidiInterface object)
        _rtpMidiInternal::midi.setHandleNoteOn(_onNoteOn);
        _rtpMidiInternal::midi.setHandleNoteOff(_onNoteOff);
        _rtpMidiInternal::midi.setHandleControlChange(_onControlChange);
        _rtpMidiInternal::midi.setHandleProgramChange(_onProgramChange);
        _rtpMidiInternal::midi.setHandleAfterTouchChannel(_onAfterTouch);
        _rtpMidiInternal::midi.setHandlePitchBend(_onPitchBend);

        _rtpMidiInternal::midi.begin(MIDI_CHANNEL_OMNI);

        _initialized = true;
        return true;
    }

    // Processes incoming RTP-MIDI packets. Call from loop().
    void task() override {
        if (!_initialized) return;
        _rtpMidiInternal::midi.read();
    }

    // Returns true while at least one macOS/iOS session is connected.
    bool isConnected() const override { return _connectedCount > 0; }

    // Sends raw MIDI bytes to all connected RTP-MIDI peers.
    bool sendMidiMessage(const uint8_t* data, size_t length) override {
        if (!_initialized || _connectedCount == 0 || length < 1) return false;

        uint8_t status  = data[0] & 0xF0;
        uint8_t channel = (data[0] & 0x0F) + 1;

        switch (status) {
            case 0x90: if (length >= 3) { _rtpMidiInternal::midi.sendNoteOn(data[1], data[2], channel); return true; } break;
            case 0x80: if (length >= 3) { _rtpMidiInternal::midi.sendNoteOff(data[1], data[2], channel); return true; } break;
            case 0xB0: if (length >= 3) { _rtpMidiInternal::midi.sendControlChange(data[1], data[2], channel); return true; } break;
            case 0xC0: if (length >= 2) { _rtpMidiInternal::midi.sendProgramChange(data[1], channel); return true; } break;
            case 0xD0: if (length >= 2) { _rtpMidiInternal::midi.sendAfterTouch(data[1], channel); return true; } break;
            case 0xE0:
                if (length >= 3) {
                    int bend = (int)(data[1] | ((uint16_t)data[2] << 7)) - 8192;
                    _rtpMidiInternal::midi.sendPitchBend(bend, channel);
                    return true;
                }
                break;
            default: break;
        }
        return false;
    }

    // Returns the number of active RTP-MIDI peer sessions.
    int connectedCount() const { return _connectedCount; }

    // --- Internal helpers called by file-scope callbacks below ---
    void _handleConnected() {
        _connectedCount++;
        if (_connectedCount == 1) dispatchConnected();
    }
    void _handleDisconnected() {
        if (_connectedCount > 0) _connectedCount--;
        if (_connectedCount == 0) dispatchDisconnected();
    }

private:
    bool _initialized;
    volatile int _connectedCount;

    static void _onNoteOn(byte channel, byte note, byte velocity) {
        if (!_instance) return;
        uint8_t msg[3] = { (uint8_t)(0x90 | (channel - 1)), note, velocity };
        _instance->dispatchMidiData(msg, 3);
    }
    static void _onNoteOff(byte channel, byte note, byte velocity) {
        if (!_instance) return;
        uint8_t msg[3] = { (uint8_t)(0x80 | (channel - 1)), note, velocity };
        _instance->dispatchMidiData(msg, 3);
    }
    static void _onControlChange(byte channel, byte controller, byte value) {
        if (!_instance) return;
        uint8_t msg[3] = { (uint8_t)(0xB0 | (channel - 1)), controller, value };
        _instance->dispatchMidiData(msg, 3);
    }
    static void _onProgramChange(byte channel, byte program) {
        if (!_instance) return;
        uint8_t msg[2] = { (uint8_t)(0xC0 | (channel - 1)), program };
        _instance->dispatchMidiData(msg, 2);
    }
    static void _onAfterTouch(byte channel, byte pressure) {
        if (!_instance) return;
        uint8_t msg[2] = { (uint8_t)(0xD0 | (channel - 1)), pressure };
        _instance->dispatchMidiData(msg, 2);
    }
    static void _onPitchBend(byte channel, int bend) {
        if (!_instance) return;
        uint16_t val = (uint16_t)(bend + 8192);
        uint8_t msg[3] = {
            (uint8_t)(0xE0 | (channel - 1)),
            (uint8_t)(val & 0x7F),
            (uint8_t)((val >> 7) & 0x7F)
        };
        _instance->dispatchMidiData(msg, 3);
    }
};

// Session-level callbacks — defined after the class so they can call its methods.
static void _rtpOnConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name) {
    if (RTPMIDIConnection::_instance)
        RTPMIDIConnection::_instance->_handleConnected();
}
static void _rtpOnDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc) {
    if (RTPMIDIConnection::_instance)
        RTPMIDIConnection::_instance->_handleDisconnected();
}

#endif // RTPMIDI_CONNECTION_H

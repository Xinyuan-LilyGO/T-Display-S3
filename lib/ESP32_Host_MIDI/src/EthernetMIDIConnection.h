#ifndef ETHERNET_MIDI_CONNECTION_H
#define ETHERNET_MIDI_CONNECTION_H

// EthernetMIDIConnection — Apple MIDI (RTP-MIDI, RFC 6295) over wired Ethernet.
//
// Header-only implementation: include this file in ONE translation unit only
// (the sketch's .ino). Users who do not use Ethernet MIDI simply do not include
// this header — the AppleMIDI and Ethernet libraries are not required for them.
//
// Usage:
//   #include "EthernetMIDIConnection.h"  // requires AppleMIDI + Ethernet
//
// Requires:
//   - lathoub/Arduino-AppleMIDI-Library v3.x
//     Arduino IDE : Library Manager → search "AppleMIDI" by lathoub
//     PlatformIO  : lib_deps = lathoub/Arduino-AppleMIDI-Library
//   - Arduino Ethernet library (W5100/W5200/W5500)
//     Arduino IDE : Library Manager → search "Ethernet" (built-in) or
//                   "Ethernet_Generic" for newer W5500 support
//     PlatformIO  : lib_deps = arduino-libraries/Ethernet
//
// Connect a W5x00 SPI module (W5500 recommended) to the ESP32 SPI bus.
// Unlike WiFi RTP-MIDI, Ethernet does NOT use mDNS auto-discovery — enter
// the device IP manually in macOS "Audio MIDI Setup → Network".
//
// Compile-time overrides — define before including this header:
//   #define ETH_MIDI_PORT        5004
//   #define ETH_MIDI_DEVICE_NAME "My ESP32"

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <AppleMIDI.h>
#include "MIDITransport.h"

#ifndef ETH_MIDI_PORT
  #define ETH_MIDI_PORT 5004
#endif

#ifndef ETH_MIDI_DEVICE_NAME
  #define ETH_MIDI_DEVICE_NAME "ESP32 MIDI"
#endif

// AppleMIDI session and MIDI interface — scoped to avoid symbol conflicts.
// Same rationale as RTPMIDIConnection: APPLEMIDI_CREATE_INSTANCE always names
// the MIDI interface 'MIDI', so we scope it inside a private namespace.
namespace _ethMidiInternal {
    APPLEMIDI_CREATE_INSTANCE(EthernetUDP, session, ETH_MIDI_DEVICE_NAME, ETH_MIDI_PORT);
}

// File-scope session callbacks — declared before the class uses them.
static void _ethOnConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name);
static void _ethOnDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc);

class EthernetMIDIConnection : public MIDITransport {
public:
    inline static EthernetMIDIConnection* _instance = nullptr;

    EthernetMIDIConnection() : _initialized(false), _connectedCount(0) {}

    // Opens an RTP-MIDI session over wired Ethernet (W5x00 SPI module).
    //   mac  : 6-byte hardware address, must be unique on your LAN.
    //   ip   : static IP. Pass IPAddress(0,0,0,0) (default) to use DHCP.
    //   csPin: SPI chip-select pin for the W5x00 module (default 5).
    //          Pass -1 if Ethernet.init() was already called in setup().
    // Returns false if the Ethernet hardware is absent or DHCP times out.
    bool begin(const uint8_t mac[6],
               IPAddress ip   = IPAddress(0, 0, 0, 0),
               int       csPin = 5)
    {
        if (_initialized) return true;

        _instance = this;

        if (csPin >= 0) Ethernet.init((uint8_t)csPin);

        const bool useDHCP = (ip == IPAddress(0, 0, 0, 0));
        if (useDHCP) {
            if (Ethernet.begin(const_cast<uint8_t*>(mac)) == 0) return false;
        } else {
            Ethernet.begin(const_cast<uint8_t*>(mac), ip);
        }

        _ethMidiInternal::session.setHandleConnected(_ethOnConnected);
        _ethMidiInternal::session.setHandleDisconnected(_ethOnDisconnected);

        _ethMidiInternal::MIDI.setHandleNoteOn(_onNoteOn);
        _ethMidiInternal::MIDI.setHandleNoteOff(_onNoteOff);
        _ethMidiInternal::MIDI.setHandleControlChange(_onControlChange);
        _ethMidiInternal::MIDI.setHandleProgramChange(_onProgramChange);
        _ethMidiInternal::MIDI.setHandleAfterTouchChannel(_onAfterTouch);
        _ethMidiInternal::MIDI.setHandlePitchBend(_onPitchBend);

        _ethMidiInternal::MIDI.begin(MIDI_CHANNEL_OMNI);

        _initialized = true;
        return true;
    }

    // Processes incoming RTP-MIDI packets. Call from loop().
    void task() override {
        if (!_initialized) return;
        _ethMidiInternal::MIDI.read();
    }

    // Returns true while at least one macOS/iOS session is connected.
    bool isConnected() const override { return _connectedCount > 0; }

    // Sends raw MIDI bytes to all connected RTP-MIDI peers over Ethernet.
    bool sendMidiMessage(const uint8_t* data, size_t length) override {
        if (!_initialized || _connectedCount == 0 || length < 1) return false;

        uint8_t status  = data[0] & 0xF0;
        uint8_t channel = (data[0] & 0x0F) + 1;

        switch (status) {
            case 0x90: if (length >= 3) { _ethMidiInternal::MIDI.sendNoteOn(data[1], data[2], channel);        return true; } break;
            case 0x80: if (length >= 3) { _ethMidiInternal::MIDI.sendNoteOff(data[1], data[2], channel);       return true; } break;
            case 0xB0: if (length >= 3) { _ethMidiInternal::MIDI.sendControlChange(data[1], data[2], channel); return true; } break;
            case 0xC0: if (length >= 2) { _ethMidiInternal::MIDI.sendProgramChange(data[1], channel);          return true; } break;
            case 0xD0: if (length >= 2) { _ethMidiInternal::MIDI.sendAfterTouch(data[1], channel);             return true; } break;
            case 0xE0:
                if (length >= 3) {
                    int bend = (int)(data[1] | ((uint16_t)data[2] << 7)) - 8192;
                    _ethMidiInternal::MIDI.sendPitchBend(bend, channel);
                    return true;
                }
                break;
            default: break;
        }
        return false;
    }

    // Returns the number of active RTP-MIDI peer sessions.
    int connectedCount() const { return _connectedCount; }

    // Returns the IP address assigned to the Ethernet interface.
    IPAddress localIP() const { return Ethernet.localIP(); }

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
static void _ethOnConnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name) {
    if (EthernetMIDIConnection::_instance)
        EthernetMIDIConnection::_instance->_handleConnected();
}
static void _ethOnDisconnected(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc) {
    if (EthernetMIDIConnection::_instance)
        EthernetMIDIConnection::_instance->_handleDisconnected();
}

#endif // ETHERNET_MIDI_CONNECTION_H

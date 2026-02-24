#ifndef USB_DEVICE_CONNECTION_H
#define USB_DEVICE_CONNECTION_H

// USBDeviceConnection — ESP32-S3/S2/P4 as a class-compliant USB MIDI Device.
//
// The ESP32 presents itself to macOS / Windows / Linux as a USB MIDI interface:
// no driver required, plug-and-play in every DAW.
//
// Typical bridge use cases:
//   BLE MIDI keyboard  →  ESP32  →  USB MIDI  →  DAW on laptop
//   RTP-MIDI (WiFi)    →  ESP32  →  USB MIDI  →  DAW on laptop
//   USB Host keyboard  →  not possible simultaneously (same OTG port)
//
// IMPORTANT — cannot coexist with USBConnection (USB Host):
//   Both use the ESP32-S3 USB OTG port (pins 19/20). Use one or the other.
//
// Arduino IDE setup (required):
//   Tools > Board        → ESP32S3 Dev Module (or T-Display-S3, etc.)
//   Tools > USB Mode     → USB-OTG (TinyUSB)          ← mandatory
//   Tools > USB CDC on Boot → Enabled (optional, for Serial over USB)
//
// PlatformIO setup:
//   board_build.arduino.usb_mode = USBMODE_USB_OTG
//
// Usage — the object MUST be declared globally (before setup()):
//   USBDeviceConnection usbMIDI("ESP32 MIDI");   // global
//   void setup() {
//     midiHandler.addTransport(&usbMIDI);
//     usbMIDI.begin();   // starts USB stack — call BEFORE midiHandler.begin()
//     midiHandler.begin(cfg);
//   }
//
// Header-only: no external library required — uses arduino-esp32's built-in
// USBMIDI class (available since arduino-esp32 2.0).

#if !defined(CONFIG_IDF_TARGET_ESP32S2) && !defined(CONFIG_IDF_TARGET_ESP32S3) && \
    !defined(CONFIG_IDF_TARGET_ESP32P4)
  #error "USBDeviceConnection requires ESP32-S2, S3 or P4 (USB OTG peripheral)."
#endif

// Prevent accidental combination with USB Host mode.
#ifdef USB_CONNECTION_H
  #error "USBDeviceConnection and USBConnection cannot be used together: both use the USB OTG port."
#endif

#include <Arduino.h>
#include <USB.h>
#include <USBMIDI.h>
#include "MIDITransport.h"

class USBDeviceConnection : public MIDITransport {
public:
    inline static USBDeviceConnection* _instance = nullptr;

    // deviceName : shown in macOS "Audio MIDI Setup" and Windows Device Manager.
    // Must be set here — TinyUSB registers the USB descriptor on construction,
    // which must happen before USB.begin() (i.e., at global scope).
    explicit USBDeviceConnection(const char* deviceName = "ESP32 MIDI")
        : _midi(deviceName), _initialized(false)
    {
        _instance = this;
    }

    // Starts the TinyUSB stack. Call once in setup().
    // Returns false if USB hardware is unavailable on this target.
    bool begin() {
        if (_initialized) return true;
        _midi.begin();  // no-op in arduino-esp32 3.x, kept for forward compatibility
        USB.begin();
        _initialized = true;
        return true;
    }

    // Reads pending USB MIDI packets and dispatches them through midiHandler.
    // Call from loop() via midiHandler.task().
    void task() override {
        if (!_initialized) return;
        midiEventPacket_t pkt;
        while (_midi.readPacket(&pkt)) {
            _processPacket(pkt);
        }
    }

    // Returns true once begin() has been called and the USB stack is running.
    // TinyUSB does not expose a per-session "port open" state for MIDI class.
    bool isConnected() const override { return _initialized; }

    // Sends raw MIDI bytes as a USB MIDI packet to the connected host (DAW).
    bool sendMidiMessage(const uint8_t* data, size_t length) override {
        if (!_initialized || length < 1) return false;

        uint8_t status  = data[0] & 0xF0;
        uint8_t channel = (data[0] & 0x0F) + 1;  // USBMIDI API is 1-indexed

        switch (status) {
            case 0x90:
                if (length >= 3) { _midi.noteOn(data[1], data[2], channel);           return true; }
                break;
            case 0x80:
                if (length >= 3) { _midi.noteOff(data[1], data[2], channel);          return true; }
                break;
            case 0xB0:
                if (length >= 3) { _midi.controlChange(data[1], data[2], channel);    return true; }
                break;
            case 0xC0:
                if (length >= 2) { _midi.programChange(data[1], channel);             return true; }
                break;
            case 0xD0:
                if (length >= 2) { _midi.channelPressure(data[1], channel);           return true; }
                break;
            case 0xE0:
                if (length >= 3) {
                    // Reconstruct 14-bit value (0–16383) from two 7-bit data bytes.
                    uint16_t val = data[1] | ((uint16_t)data[2] << 7);
                    _midi.pitchBend(val, channel);
                    return true;
                }
                break;
            default:
                break;
        }
        return false;
    }

private:
    USBMIDI _midi;
    bool    _initialized;

    void _processPacket(const midiEventPacket_t& pkt) {
        // CIN (Code Index Number) — USB MIDI spec 1.0, Table 4-1
        uint8_t cin  = pkt.header & 0x0F;
        uint8_t msg3[3] = { pkt.byte1, pkt.byte2, pkt.byte3 };
        uint8_t msg2[2] = { pkt.byte1, pkt.byte2 };

        switch (cin) {
            case 0x08:  // Note Off
            case 0x09:  // Note On
            case 0x0A:  // Poly Key Pressure
            case 0x0B:  // Control Change
            case 0x0E:  // Pitch Bend
                dispatchMidiData(msg3, 3);
                break;
            case 0x0C:  // Program Change
            case 0x0D:  // Channel Pressure
                dispatchMidiData(msg2, 2);
                break;
            default:
                break;
        }
    }
};

#endif // USB_DEVICE_CONNECTION_H

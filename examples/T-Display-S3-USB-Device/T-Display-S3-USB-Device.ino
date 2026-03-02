// T-Display-S3-USB-Device — ESP32_Host_MIDI example
//
// The ESP32-S3 acts as a USB MIDI interface for the connected computer,
// while simultaneously receiving MIDI from a BLE device (iPhone, iPad,
// BLE keyboard, etc.).
//
// Signal path:
//   iPhone / iPad  ──BLE──►  ESP32-S3  ──USB──►  Logic / Ableton / Reaper
//
// The 1.9" display shows:
//   - USB connection status (waiting / connected)
//   - BLE connection status (scanning / connected)
//   - Scrolling MIDI event log (color-coded by message type)
//   - IN / OUT event counters
//
// Arduino IDE setup (mandatory):
//   Tools > Board    → T-Display-S3 (or ESP32S3 Dev Module)
//   Tools > USB Mode → USB-OTG (TinyUSB)   ← required for USB MIDI device
//
// The device name shown in the DAW's MIDI port list is configured in
// mapping.h via USB_MIDI_DEVICE_NAME.

#include <Arduino.h>
#define ESP32_HOST_MIDI_NO_USB_HOST  // Use USB Device mode, not Host
#include <ESP32_Host_MIDI.h>
#include "../../src/USBDeviceConnection.h"
#include "mapping.h"
#include "ST7789_Handler.h"

// MUST be global — TinyUSB registers the USB descriptor on construction,
// before USB.begin() is called. The name comes from mapping.h.
USBDeviceConnection usbMIDI(USB_MIDI_DEVICE_NAME);

static int    lastEventIndex = -1;
static int    inCount        = 0;
static int    outCount       = 0;
static bool   lastBLE        = false;
static unsigned long lastStatusCheck = 0;

// ---- helpers -----------------------------------------------------------

static uint32_t eventColor(const std::string& s) {
    if (s == "NoteOn")        return USB_COL_CYAN;
    if (s == "NoteOff")       return USB_COL_GRAY;
    if (s == "ControlChange") return USB_COL_YELLOW;
    if (s == "PitchBend")     return USB_COL_MAGENTA;
    if (s == "ProgramChange") return USB_COL_LIME;
    return USB_COL_WHITE;
}

static void formatEvent(const MIDIEventData& ev, char* buf, int len) {
    if (ev.status == "NoteOn") {
        snprintf(buf, len, "NOTE+  %-3s  v=%-3d  ch%d",
                 ev.noteOctave.c_str(), ev.velocity, ev.channel);
    } else if (ev.status == "NoteOff") {
        snprintf(buf, len, "NOTE-  %-3s  v=%-3d  ch%d",
                 ev.noteOctave.c_str(), ev.velocity, ev.channel);
    } else if (ev.status == "ControlChange") {
        snprintf(buf, len, "CC#%-3d  v=%-3d        ch%d",
                 ev.note, ev.velocity, ev.channel);
    } else if (ev.status == "PitchBend") {
        snprintf(buf, len, "PB  %+6d             ch%d",
                 ev.pitchBend - 8192, ev.channel);
    } else if (ev.status == "ProgramChange") {
        snprintf(buf, len, "PC   prog=%-3d          ch%d",
                 ev.note, ev.channel);
    } else {
        snprintf(buf, len, "%-8s                ch%d",
                 ev.status.c_str(), ev.channel);
    }
}

// ---- setup / loop ------------------------------------------------------

void setup() {
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    display.init();

    // Register USB Device transport (BLE is registered automatically by begin()).
    midiHandler.addTransport(&usbMIDI);

    // Start USB stack (enumerates on the host as a MIDI device).
    usbMIDI.begin();

    MIDIHandlerConfig cfg;
    cfg.maxEvents = 40;
    midiHandler.begin(cfg);

    display.setBLE(false);
    display.setUSB(false);
}

void loop() {
    midiHandler.task();

    // Refresh connection dots every 2 s.
    if (millis() - lastStatusCheck >= 2000) {
        lastStatusCheck = millis();

        bool ble = midiHandler.isBleConnected();
        if (ble != lastBLE) {
            lastBLE = ble;
            display.setBLE(ble);
        }

        // USB MIDI device has no per-session state — show "connected" once
        // begin() has been called (the port is always visible to the host).
        display.setUSB(usbMIDI.isConnected());
    }

    // Process new MIDI events.
    const auto& queue = midiHandler.getQueue();
    bool countersChanged = false;

    for (const auto& ev : queue) {
        if (ev.index <= lastEventIndex) continue;
        lastEventIndex = ev.index;
        inCount++;
        countersChanged = true;

        char line[32];
        formatEvent(ev, line, sizeof(line));
        display.pushEvent(eventColor(ev.status), line);

        // Forward every event received (from BLE or USB host) to the other side.
        // BLE → forward to USB; USB → forward to BLE (if connected).
        // midiHandler forwards automatically to all registered transports.
        outCount++;
    }

    if (countersChanged) {
        display.setCounters(inCount, outCount);
    }
}

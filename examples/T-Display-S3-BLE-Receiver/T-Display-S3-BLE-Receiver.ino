// Example: BLE MIDI Receiver with Piano Visualizer
//
// Receives MIDI notes wirelessly via Bluetooth Low Energy and displays
// a real-time piano visualizer on the ST7789 display.
//
// Pair with the T-Display-S3-BLE-Sender example on a second board,
// or use any BLE MIDI app (iOS GarageBand, Android MIDI BLE Connect, etc.)
//
// The display shows:
//   - BLE connection status (Connected / Waiting)
//   - 25-key piano with active notes highlighted
//   - Note names and frequency of active notes
//
// Audio: plays received notes through PCM5102A DAC via I2S (optional hardware).
//
// Dependencies: ESP32_Host_MIDI, LovyanGFX

#include <Arduino.h>
#include <ESP32_Host_MIDI.h>
#include "PianoDisplay.h"
#include "SynthEngine.h"
#include "mapping.h"

// ── Global instances ──────────────────────────────────────────────────────────
SynthEngine synth;

// ── Active notes state ────────────────────────────────────────────────────────
static bool activeNotes[128]     = {};
static bool prevActiveNotes[128] = {};

// ── Display info ──────────────────────────────────────────────────────────────
static PianoInfo info = {};

// ── Simple note analysis (no Gingoduino dependency) ──────────────────────────
static void analyzeNotes() {
    bool ble = info.bleConnected;  // preserve across memset
    memset(&info, 0, sizeof(info));
    info.bleConnected = ble;
    int count = 0;
    int pos = 0;
    for (int n = 0; n < 128; n++) {
        if (activeNotes[n]) {
            if (count == 0) info.rootMidi = n;
            int oct = (n / 12) - 1;
            if (pos > 0 && pos < 60) { info.noteStr[pos++] = ' '; info.noteStr[pos++] = ' '; }
            pos += snprintf(info.noteStr + pos, sizeof(info.noteStr) - pos,
                           "%s%d", NOTE_NAMES[n % 12], oct);
            count++;
        }
    }
    info.noteCount = count;
    if (count > 0) {
        info.rootFreq = 440.0f * powf(2.0f, (info.rootMidi - 69) / 12.0f);
    }
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    piano.init();
    piano.render(activeNotes, info);

    MIDIHandlerConfig cfg;
    cfg.maxEvents       = 64;
    cfg.chordTimeWindow = 0;
    cfg.bleName         = "ESP32 BLE Piano";   // Sender scans for this name
    midiHandler.begin(cfg);

    synth.begin();

    Serial.println("=== BLE MIDI Receiver ===");
    Serial.println("Advertising as 'ESP32 BLE Piano'");
    Serial.println("Waiting for BLE MIDI sender...");
}

// ── Loop ──────────────────────────────────────────────────────────────────────
static uint32_t lastFrameUs = 0;
static const uint32_t FRAME_US = 33000;  // ~30 fps

void loop() {
    midiHandler.task();

    midiHandler.fillActiveNotes(activeNotes);

    if (memcmp(activeNotes, prevActiveNotes, sizeof(activeNotes)) != 0) {
        // Feed synth from state diff
        for (int n = 0; n < 128; n++) {
            if (activeNotes[n] && !prevActiveNotes[n]) synth.noteOn(n, 100);
            if (!activeNotes[n] && prevActiveNotes[n]) synth.noteOff(n);
        }
        memcpy(prevActiveNotes, activeNotes, sizeof(activeNotes));
        analyzeNotes();

        // Auto-adjust view if notes are outside visible range
        for (int n = 0; n < 128; n++) {
            if (activeNotes[n]) {
                if (n < piano.getViewStart() || n >= piano.getViewStart() + 25) {
                    piano.setViewStart(n);
                }
                break;
            }
        }
    }

    // Update BLE status every frame
    info.bleConnected = midiHandler.isBleConnected();

    uint32_t now = micros();
    if (now - lastFrameUs >= FRAME_US) {
        lastFrameUs = now;
        piano.render(activeNotes, info);
    }
}

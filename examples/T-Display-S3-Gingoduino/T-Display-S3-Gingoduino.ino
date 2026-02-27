// Example: Music Theory Analysis on T-Display S3
// Demonstrates ESP32_Host_MIDI + Gingoduino integration with ST7789 display.
//
// What it shows on display + Serial:
//   - 1 note:   Note name, octave, frequency (e.g., "C4  261.63 Hz")
//   - 2 notes:  Interval identification (e.g., "C4, E4 -> 3M Major Third")
//   - 3+ notes: Chord identification (e.g., "C4, E4, G4 -> CM")
//   - After 3+ identified chords: Harmonic field deduction
//     (e.g., "Field: C Major (3/3)")
//
// Requirements:
//   - ESP32_Host_MIDI library
//   - Gingoduino library v0.2.2+ (https://github.com/sauloverissimo/gingoduino)
//   - LovyanGFX library
//   - T-Display S3 board

#include <Arduino.h>
#include <ESP32_Host_MIDI.h>
#include <GingoAdapter.h>
#include "ST7789_Handler.h"

using namespace gingoduino;

// Track state
static int lastChordIdx = 0;
static int lastNoteCount = 0;
static int lastDeduceChordCount = 0;

// Store identified chord names for field deduction
static constexpr uint8_t MAX_HISTORY = 16;
static char chordHistory[MAX_HISTORY][16];
static const char* chordPtrs[MAX_HISTORY];
static uint8_t chordHistoryCount = 0;

// Last field result (persists across chords)
static String fieldLine = "";
static String rolesLine = "";

// Add a chord name to history (avoids duplicates)
static void addToHistory(const char* name) {
    for (uint8_t i = 0; i < chordHistoryCount; i++) {
        if (strcmp(chordHistory[i], name) == 0) return;
    }
    if (chordHistoryCount >= MAX_HISTORY) return;
    strncpy(chordHistory[chordHistoryCount], name, 15);
    chordHistory[chordHistoryCount][15] = '\0';
    chordPtrs[chordHistoryCount] = chordHistory[chordHistoryCount];
    chordHistoryCount++;
}

// Build scale type string
static const char* scaleTypeName(ScaleType st) {
    switch (st) {
        case SCALE_MAJOR:          return "Major";
        case SCALE_NATURAL_MINOR:  return "Minor";
        case SCALE_HARMONIC_MINOR: return "Harm.Min";
        case SCALE_MELODIC_MINOR:  return "Mel.Min";
        default:                   return "";
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    display.init();
    display.print("Initializing...");
    delay(300);

    MIDIHandlerConfig config;
    config.chordTimeWindow = 50;
    config.bleName = "ESP32 Gingoduino";

    midiHandler.begin(config);

    display.print("MIDI + Gingoduino\nReady!\n\nPlay notes...");
    Serial.println("=== T-Display-S3 + Gingoduino ===");
    Serial.println("  1 note   -> note + frequency");
    Serial.println("  2 notes  -> interval");
    Serial.println("  3+ notes -> chord + field");
    Serial.println();
}

void loop() {
    midiHandler.task();

    const auto& queue = midiHandler.getQueue();
    if (queue.empty()) return;

    int currentChord = midiHandler.lastChord(queue);
    if (currentChord <= 0) return;

    // Count NoteOn events in the current chord
    int noteCount = 0;
    for (const auto& ev : queue) {
        if (ev.chordIndex == currentChord && ev.status == "NoteOn") noteCount++;
    }

    // Only re-analyze when chord changes or new notes are added
    if (currentChord == lastChordIdx && noteCount == lastNoteCount) return;

    bool isNewChord = (currentChord != lastChordIdx);
    lastChordIdx = currentChord;
    lastNoteCount = noteCount;

    // Get MIDI note numbers and note names for this chord
    std::vector<std::string> noteNumbers = midiHandler.getChord(
        currentChord, queue, {"note"}
    );
    std::vector<std::string> noteOctaves = midiHandler.getChord(
        currentChord, queue, {"noteOctave"}
    );

    if (noteNumbers.empty()) return;

    // Convert to sorted GingoNote array
    uint8_t midiNotes[7];
    GingoNote gingoNotes[7];
    uint8_t count = min((size_t)7, noteNumbers.size());

    for (uint8_t i = 0; i < count; i++) {
        midiNotes[i] = (uint8_t)atoi(noteNumbers[i].c_str());
    }
    uint8_t n = GingoAdapter::midiToGingoNotes(midiNotes, count, gingoNotes);

    // Build display text
    String displayText = "";

    // Active notes count header
    size_t activeCount = midiHandler.getActiveNotesCount();
    std::string activeStr = midiHandler.getActiveNotes();
    char header[64];
    snprintf(header, sizeof(header), "[%d] %s", (int)activeCount, activeStr.c_str());
    displayText += header;
    displayText += "\n";

    if (n == 1) {
        // --- Single note ---
        int8_t octave = GingoNote::octaveFromMIDI(midiNotes[0]);
        float freq = gingoNotes[0].frequency(octave);

        char buf[32];
        snprintf(buf, sizeof(buf), "%s%d  %.2f Hz", gingoNotes[0].name(), octave, freq);
        displayText += buf;

        if (isNewChord) {
            Serial.printf("%s%d  %.2f Hz\n", gingoNotes[0].name(), octave, freq);
        }

    } else if (n == 2) {
        // --- Interval ---
        GingoInterval iv(gingoNotes[0], gingoNotes[1]);
        char labelBuf[6];
        char nameBuf[24];
        iv.label(labelBuf, sizeof(labelBuf));
        iv.fullName(nameBuf, sizeof(nameBuf));

        String notesStr = "";
        for (size_t i = 0; i < noteOctaves.size(); i++) {
            if (i > 0) notesStr += ", ";
            notesStr += noteOctaves[i].c_str();
        }

        displayText += notesStr;
        displayText += "\n-> ";
        displayText += labelBuf;
        displayText += " (";
        displayText += nameBuf;
        displayText += ")";

        Serial.printf("%s  ->  %s (%s)\n", notesStr.c_str(), labelBuf, nameBuf);

    } else {
        // --- Chord identification ---
        String notesStr = "";
        for (size_t i = 0; i < noteOctaves.size(); i++) {
            if (i > 0) notesStr += ", ";
            notesStr += noteOctaves[i].c_str();
        }

        displayText += notesStr;

        char chordName[16];
        if (GingoChord::identify(gingoNotes, n, chordName, sizeof(chordName))) {
            displayText += "\n-> ";
            displayText += chordName;

            Serial.printf("%s  ->  %s\n", notesStr.c_str(), chordName);
            addToHistory(chordName);

            // --- Harmonic field deduction (after 3+ unique chords) ---
            if (chordHistoryCount >= 3 && chordHistoryCount > lastDeduceChordCount) {
                lastDeduceChordCount = chordHistoryCount;

                FieldMatch results[3];
                uint8_t nResults = GingoAdapter::deduceField(
                    chordPtrs, chordHistoryCount, results, 3
                );

                if (nResults > 0) {
                    char fBuf[48];
                    snprintf(fBuf, sizeof(fBuf), "Field: %s %s (%d/%d)",
                             results[0].tonicName, scaleTypeName(results[0].scaleType),
                             results[0].matched, results[0].total);
                    fieldLine = fBuf;

                    if (results[0].roleCount > 0) {
                        rolesLine = "Roles: ";
                        for (uint8_t r = 0; r < results[0].roleCount; r++) {
                            if (r > 0) rolesLine += ", ";
                            rolesLine += results[0].roles[r];
                        }
                    }

                    Serial.printf("  %s\n", fieldLine.c_str());
                    if (rolesLine.length() > 0) {
                        Serial.printf("  %s\n", rolesLine.c_str());
                    }

                    if (nResults > 1 && results[1].matched == results[0].matched) {
                        Serial.printf("  Also: %s %s\n",
                                      results[1].tonicName,
                                      scaleTypeName(results[1].scaleType));
                    }
                }
            }
        } else {
            displayText += "\n-> (?)";
        }
    }

    // Append persistent field info
    if (fieldLine.length() > 0) {
        displayText += "\n\n";
        displayText += fieldLine;
        if (rolesLine.length() > 0) {
            displayText += "\n";
            displayText += rolesLine;
        }
    }

    // Append chord history
    if (chordHistoryCount > 0) {
        displayText += "\n\nChords: ";
        for (uint8_t i = 0; i < chordHistoryCount; i++) {
            if (i > 0) displayText += " ";
            displayText += chordHistory[i];
        }
    }

    display.print(displayText.c_str());
    delayMicroseconds(10);
}

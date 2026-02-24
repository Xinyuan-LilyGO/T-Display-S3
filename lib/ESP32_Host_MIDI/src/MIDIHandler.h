#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include <deque>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "MIDIHandlerConfig.h"
#include "MIDITransport.h"

// --- Feature detection macros ---
// If ESP32_Host_MIDI.h was included first, these are already defined.
// Otherwise, detect features from ESP-IDF / SDK configuration.
#ifndef ESP32_HOST_MIDI_HAS_USB
  #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3) || \
      defined(CONFIG_IDF_TARGET_ESP32P4)
    #define ESP32_HOST_MIDI_HAS_USB 1
  #else
    #define ESP32_HOST_MIDI_HAS_USB 0
  #endif
#endif

#ifndef ESP32_HOST_MIDI_HAS_BLE
  #if defined(CONFIG_BT_ENABLED)
    #define ESP32_HOST_MIDI_HAS_BLE 1
  #else
    #define ESP32_HOST_MIDI_HAS_BLE 0
  #endif
#endif

#ifndef ESP32_HOST_MIDI_HAS_PSRAM
  #if defined(CONFIG_SPIRAM) || defined(CONFIG_SPIRAM_SUPPORT)
    #define ESP32_HOST_MIDI_HAS_PSRAM 1
  #else
    #define ESP32_HOST_MIDI_HAS_PSRAM 0
  #endif
#endif

#ifndef ESP32_HOST_MIDI_HAS_ETH_MAC
  #if defined(CONFIG_IDF_TARGET_ESP32P4)
    #define ESP32_HOST_MIDI_HAS_ETH_MAC 1
  #else
    #define ESP32_HOST_MIDI_HAS_ETH_MAC 0
  #endif
#endif

#if ESP32_HOST_MIDI_HAS_USB
  #include "USBConnection.h"
#endif

#if ESP32_HOST_MIDI_HAS_BLE
  #include "BLEConnection.h"
#endif

// Structure representing a parsed MIDI event using MIDI 1.0 terminology.
struct MIDIEventData {
  int index;                // Global event counter
  int msgIndex;             // Index linking NoteOn/NoteOff pairs
  unsigned long timestamp;  // Timestamp in milliseconds (millis())
  unsigned long delay;      // Delta time (ms) since previous event
  int channel;              // MIDI channel (1-16)
  std::string status;       // Status type: "NoteOn", "NoteOff", "ControlChange", "ProgramChange", "PitchBend", "ChannelPressure"
  int note;                 // MIDI note number (or controller number for ControlChange)
  std::string noteName;     // Musical note name (e.g., "C", "D#") — empty for non-note messages
  std::string noteOctave;   // Note with octave (e.g., "C4", "D#5") — empty for non-note messages
  int velocity;             // Velocity (or CC value, program number, or pressure value)
  int chordIndex;           // Chord grouping index (simultaneous notes share the same index)
  int pitchBend;            // Pitch Bend value (14-bit, 0-16383, center = 8192). 0 for other types.
};

class MIDIHandler {
public:
  MIDIHandler();
  ~MIDIHandler();

  void begin();
  void begin(const MIDIHandlerConfig& config);
  void task();
  void enableHistory(int capacity);
  void addEvent(const MIDIEventData& event);
  void processQueue();
  void setQueueLimit(int maxEvents);
  const std::deque<MIDIEventData>& getQueue() const;

  void handleMidiMessage(const uint8_t* data, size_t length);

  // Debug callback — called with raw MIDI bytes before parsing.
  // Set to nullptr to disable. Signature: (rawData, rawLength, midiBytes3)
  typedef void (*RawMidiCallback)(const uint8_t* raw, size_t rawLen,
                                   const uint8_t* midi3);
  void setRawMidiCallback(RawMidiCallback cb) { rawMidiCb = cb; }

  std::string getActiveNotesString() const;
  std::string getActiveNotes() const;
  std::vector<std::string> getActiveNotesVector() const;
  size_t getActiveNotesCount() const;
  void fillActiveNotes(bool out[128]) const;
  void clearActiveNotesNow();
  void clearQueue();

  // Register an external transport (ESP-NOW, RTP-MIDI, custom, etc.).
  // The transport must already be initialized (begin() called) before adding.
  // MIDIHandler will call task() on it and receive data via callbacks.
  void addTransport(MIDITransport* transport);

  // MIDI Output — send via any transport that supports sending.
  // channel: 1-16. Returns true if any transport sent the message.
  bool sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
  bool sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
  bool sendControlChange(uint8_t channel, uint8_t controller, uint8_t value);
  bool sendProgramChange(uint8_t channel, uint8_t program);
  bool sendPitchBend(uint8_t channel, int value);  // value: -8192 to 8191
  bool sendRaw(const uint8_t* data, size_t length);
  bool sendBleRaw(const uint8_t* data, size_t length);  // backward compat alias

#if ESP32_HOST_MIDI_HAS_BLE
  bool isBleConnected() const;
#endif

  // Chord event utility methods:
  int lastChord(const std::deque<MIDIEventData>& queue) const;
  std::vector<std::string> getChord(int chord, const std::deque<MIDIEventData>& queue, const std::vector<std::string>& fields = { "all" }, bool includeLabels = false) const;
  std::vector<std::string> getAnswer(const std::string& field = "all", bool includeLabels = false) const;
  std::vector<std::string> getAnswer(const std::vector<std::string>& fields, bool includeLabels = false) const;

private:
  MIDIHandlerConfig config;
  RawMidiCallback rawMidiCb = nullptr;

  std::deque<MIDIEventData> eventQueue;
  int maxEvents;
  int globalIndex;
  int nextMsgIndex;
  unsigned long lastTimestamp;
  unsigned long lastNoteOnTimestamp;

  std::unordered_map<int, int> activeNotes;
  std::unordered_map<int, int> activeMsgIndex;

  int nextChordIndex;
  int currentChordIndex;

  // History buffer (PSRAM when available, heap otherwise)
  MIDIEventData* historyQueue;
  int historyQueueCapacity;
  int historyQueueSize;
  int historyQueueHead;

  void expandHistoryQueue();

  std::string getNoteName(int note) const;
  std::string getNoteWithOctave(int note) const;

  // --- Transport abstraction ---
  static const int MAX_TRANSPORTS = 4;
  MIDITransport* transports[MAX_TRANSPORTS];
  int transportCount;

  void registerTransport(MIDITransport* t);
  static void _onTransportMidiData(void* ctx, const uint8_t* data, size_t len);
  static void _onTransportDisconnected(void* ctx);

  // Built-in transports (owned by MIDIHandler, registered automatically in begin())
#if ESP32_HOST_MIDI_HAS_USB
  USBConnection usbTransport;
#endif
#if ESP32_HOST_MIDI_HAS_BLE
  BLEConnection bleTransport;
#endif
};

extern MIDIHandler midiHandler;

#endif  // MIDI_HANDLER_H

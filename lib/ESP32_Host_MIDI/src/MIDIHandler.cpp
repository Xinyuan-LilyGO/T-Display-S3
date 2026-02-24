#include <Arduino.h>
#include "MIDIHandler.h"
MIDIHandler midiHandler;

#include <algorithm>
#include <cstdio>
#include <sstream>

#if ESP32_HOST_MIDI_HAS_PSRAM
  #include "esp_heap_caps.h"
#endif

MIDIHandler::MIDIHandler()
  : maxEvents(20),
    globalIndex(0),
    nextMsgIndex(1),
    lastTimestamp(0),
    lastNoteOnTimestamp(0),
    nextChordIndex(1),
    currentChordIndex(0),
    historyQueue(nullptr),
    historyQueueCapacity(0),
    historyQueueSize(0),
    historyQueueHead(0),
    transportCount(0)
{
  memset(transports, 0, sizeof(transports));
}

MIDIHandler::~MIDIHandler() {
  if (historyQueue) {
    // Call destructor on each element to properly release std::string
    for (int i = 0; i < historyQueueCapacity; i++) {
      historyQueue[i].~MIDIEventData();
    }
    free(historyQueue);
    historyQueue = nullptr;
  }
}

void MIDIHandler::begin() {
  MIDIHandlerConfig defaultConfig;
  begin(defaultConfig);
}

void MIDIHandler::begin(const MIDIHandlerConfig& cfg) {
  this->config = cfg;
  this->maxEvents = cfg.maxEvents;

#if ESP32_HOST_MIDI_HAS_USB
  registerTransport(&usbTransport);
  usbTransport.begin();
#endif

#if ESP32_HOST_MIDI_HAS_BLE
  registerTransport(&bleTransport);
  bleTransport.begin(std::string(cfg.bleName));
#endif

  if (cfg.historyCapacity > 0) {
    enableHistory(cfg.historyCapacity);
  }
}

void MIDIHandler::task() {
  for (int i = 0; i < transportCount; i++) {
    transports[i]->task();
  }
}

// --- Transport Abstraction ---

void MIDIHandler::_onTransportMidiData(void* ctx, const uint8_t* data, size_t len) {
  static_cast<MIDIHandler*>(ctx)->handleMidiMessage(data, len);
}

void MIDIHandler::_onTransportDisconnected(void* ctx) {
  static_cast<MIDIHandler*>(ctx)->clearActiveNotesNow();
}

void MIDIHandler::registerTransport(MIDITransport* t) {
  if (transportCount >= MAX_TRANSPORTS) return;
  t->setMidiCallback(_onTransportMidiData, this);
  t->setConnectionCallbacks(nullptr, _onTransportDisconnected, this);
  transports[transportCount++] = t;
}

void MIDIHandler::addTransport(MIDITransport* transport) {
  registerTransport(transport);
}

void MIDIHandler::enableHistory(int capacity) {
  if (capacity <= 0) {
    // Disable history, freeing allocated memory
    if (historyQueue) {
      free(historyQueue);
      historyQueue = nullptr;
    }
    historyQueueCapacity = 0;
    historyQueueSize = 0;
    historyQueueHead = 0;
    Serial.println("MIDI history disabled!");
    return;
  }

  // Free existing history before reallocating
  if (historyQueue) {
    free(historyQueue);
    historyQueue = nullptr;
  }

  // Allocate history buffer — prefer PSRAM if available, fall back to heap
#if ESP32_HOST_MIDI_HAS_PSRAM
  historyQueue = static_cast<MIDIEventData*>(heap_caps_malloc(capacity * sizeof(MIDIEventData), MALLOC_CAP_SPIRAM));
#else
  historyQueue = static_cast<MIDIEventData*>(malloc(capacity * sizeof(MIDIEventData)));
#endif

  if (!historyQueue) {
    Serial.println("Failed to allocate memory for history buffer!");
    historyQueueCapacity = 0;
    return;
  }

  historyQueueCapacity = capacity;
  historyQueueSize = 0;
  historyQueueHead = 0;

  // Initialize each object using placement new to avoid std::string issues
  for (int i = 0; i < historyQueueCapacity; i++) {
    new (&historyQueue[i]) MIDIEventData();
  }

  Serial.println("MIDI history enabled!");
}


void MIDIHandler::setQueueLimit(int maxEvents) {
  this->maxEvents = maxEvents;
}

const std::deque<MIDIEventData>& MIDIHandler::getQueue() const {
  return eventQueue;
}

// Expands the dynamic ring buffer.
void MIDIHandler::expandHistoryQueue() {
  int newCapacity = (historyQueueCapacity > 0) ? (historyQueueCapacity * 2) : 10;

#if ESP32_HOST_MIDI_HAS_PSRAM
  MIDIEventData* newQueue = static_cast<MIDIEventData*>(heap_caps_malloc(newCapacity * sizeof(MIDIEventData), MALLOC_CAP_SPIRAM));
#else
  MIDIEventData* newQueue = static_cast<MIDIEventData*>(malloc(newCapacity * sizeof(MIDIEventData)));
#endif

  if (!newQueue) {
    Serial.println("Failed to expand MIDI history buffer!");
    return;
  }
  // Calculate tail index in the current circular buffer.
  int tail = (historyQueueHead + historyQueueCapacity - historyQueueSize) % historyQueueCapacity;
  // Copy events from old buffer to new, preserving order.
  for (int i = 0; i < historyQueueSize; i++) {
    int index = (tail + i) % historyQueueCapacity;
    newQueue[i] = historyQueue[index];
  }
  // Initialize remaining elements in the new buffer
  for (int i = historyQueueSize; i < newCapacity; i++) {
    new (&newQueue[i]) MIDIEventData();
  }
  free(historyQueue);
  historyQueue = newQueue;
  historyQueueCapacity = newCapacity;
  // After copying, new head is immediately after the last element.
  historyQueueHead = historyQueueSize;
  Serial.printf("MIDI history expanded to %d events!\n", historyQueueCapacity);
}

void MIDIHandler::addEvent(const MIDIEventData& event) {
  // Add event to the main queue (stored in SRAM)
  eventQueue.push_back(event);
  processQueue();

  // If history is active, add event to the dynamic buffer
  if (historyQueue != nullptr && historyQueueCapacity > 0) {
    // If buffer is full, expand to avoid discarding events
    if (historyQueueSize == historyQueueCapacity) {
      expandHistoryQueue();
    }
    historyQueue[historyQueueHead] = event;
    historyQueueHead = (historyQueueHead + 1) % historyQueueCapacity;
    historyQueueSize++;
  }
}


void MIDIHandler::processQueue() {
  while (eventQueue.size() > static_cast<size_t>(maxEvents)) {
    eventQueue.pop_front();
  }
}

std::string MIDIHandler::getNoteName(int note) const {
  static const char* names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  return names[note % 12];
}

std::string MIDIHandler::getNoteWithOctave(int note) const {
  int octave = (note / 12) - 1;
  return getNoteName(note) + std::to_string(octave);
}


std::string MIDIHandler::getActiveNotes() const {
  std::ostringstream oss;
  oss << "{";

  // Step 1: Create a sorted vector from the active notes map
  std::vector<int> sortedNotes;
  for (const auto& kv : activeNotes) {
    sortedNotes.push_back(kv.first);
  }

  // Step 2: Sort by note number
  std::sort(sortedNotes.begin(), sortedNotes.end());

  // Step 3: Build the formatted string
  bool first = true;
  for (int note : sortedNotes) {
    if (!first) oss << ", ";
    oss << getNoteName(note);
    first = false;
  }

  oss << "}";
  return oss.str();
}

std::vector<std::string> MIDIHandler::getActiveNotesVector() const {
  std::vector<std::string> activeNotesVector;

  // Use std::map for automatic sorting by MIDI note number
  std::map<int, int> sortedActiveNotes(activeNotes.begin(), activeNotes.end());

  for (const auto& kv : sortedActiveNotes) {
    activeNotesVector.push_back(getNoteName(kv.first));
  }

  return activeNotesVector;
}

std::string MIDIHandler::getActiveNotesString() const {
  std::ostringstream oss;
  oss << "{";

  // Use std::map for automatic sorting
  std::map<int, int> sortedActiveNotes(activeNotes.begin(), activeNotes.end());

  bool first = true;
  for (const auto& kv : sortedActiveNotes) {
    if (!first) oss << ", ";
    oss << getNoteName(kv.first) << ", {" << kv.second << "}";
    first = false;
  }

  oss << "}";
  return oss.str();
}


size_t MIDIHandler::getActiveNotesCount() const {
  return activeNotes.size();
}

void MIDIHandler::fillActiveNotes(bool out[128]) const {
  memset(out, 0, 128);
  for (const auto& kv : activeNotes) {
    if (kv.first >= 0 && kv.first < 128)
      out[kv.first] = true;
  }
}

// Clears active notes
void MIDIHandler::clearActiveNotesNow() {
  activeNotes.clear();
  activeMsgIndex.clear();
  currentChordIndex = 0;
}

// Clears the event queue and resets all state
void MIDIHandler::clearQueue() {
  eventQueue.clear();
  clearActiveNotesNow();
  globalIndex = 0;
  nextMsgIndex = 1;
  lastTimestamp = 0;
  lastNoteOnTimestamp = 0;
  nextChordIndex = 1;
}

int MIDIHandler::lastChord(const std::deque<MIDIEventData>& queue) const {
  int maxChord = 0;
  for (const auto& event : queue) {
    if (event.chordIndex > maxChord) {
      maxChord = event.chordIndex;
    }
  }
  return maxChord;
}

std::vector<std::string> MIDIHandler::getChord(int chord, const std::deque<MIDIEventData>& queue, const std::vector<std::string>& fields, bool includeLabels) const {
  std::vector<MIDIEventData> chordEvents;

  // Filter only NoteOn events from the specified chord
  for (const auto& event : queue) {
    if (event.chordIndex == chord && event.status == "NoteOn") {
      chordEvents.push_back(event);
    }
  }

  // Sort events by note number
  std::sort(chordEvents.begin(), chordEvents.end(), [](const MIDIEventData& a, const MIDIEventData& b) {
    return a.note < b.note;
  });

  std::vector<std::string> result;

  // If "all" was requested, return all fields
  if (fields.size() == 1 && fields[0] == "all") {
    for (const auto& event : chordEvents) {
      std::ostringstream oss;
      if (includeLabels) {
        oss << "{index:" << event.index
            << ", msgIndex:" << event.msgIndex
            << ", timestamp:" << event.timestamp
            << ", delay:" << event.delay
            << ", channel:" << event.channel
            << ", status:" << event.status
            << ", note:" << event.note
            << ", noteName:" << event.noteName
            << ", noteOctave:" << event.noteOctave
            << ", velocity:" << event.velocity
            << ", chordIndex:" << event.chordIndex
            << ", pitchBend:" << event.pitchBend << "}";
      } else {
        oss << "{ " << event.index
            << ", " << event.msgIndex
            << ", " << event.timestamp
            << ", " << event.delay
            << ", " << event.channel
            << ", " << event.status
            << ", " << event.note
            << ", " << event.noteName
            << ", " << event.noteOctave
            << ", " << event.velocity
            << ", " << event.chordIndex
            << ", " << event.pitchBend << " }";
      }
      result.push_back(oss.str());
    }
  }
  // If a single field was requested
  else if (fields.size() == 1) {
    std::string field = fields[0];
    for (const auto& event : chordEvents) {
      if (field == "noteName") {
        result.push_back(event.noteName);
      } else if (field == "noteOctave") {
        result.push_back(event.noteOctave);
      } else if (field == "status") {
        result.push_back(event.status);
      } else if (field == "note") {
        result.push_back(std::to_string(event.note));
      } else if (field == "timestamp") {
        result.push_back(std::to_string(event.timestamp));
      } else if (field == "velocity") {
        result.push_back(std::to_string(event.velocity));
      } else if (field == "channel") {
        result.push_back(std::to_string(event.channel));
      } else if (field == "pitchBend") {
        result.push_back(std::to_string(event.pitchBend));
      }
    }
  }
  // If multiple fields were requested
  else {
    for (const auto& event : chordEvents) {
      std::ostringstream oss;
      bool first = true;
      for (const auto& field : fields) {
        if (!first) oss << ", ";
        if (field == "noteName") {
          oss << event.noteName;
        } else if (field == "noteOctave") {
          oss << event.noteOctave;
        } else if (field == "status") {
          oss << event.status;
        } else if (field == "note") {
          oss << event.note;
        } else if (field == "timestamp") {
          oss << event.timestamp;
        } else if (field == "velocity") {
          oss << event.velocity;
        } else if (field == "channel") {
          oss << event.channel;
        } else if (field == "pitchBend") {
          oss << event.pitchBend;
        }
        first = false;
      }
      result.push_back(oss.str());
    }
  }

  return result;
}

std::vector<std::string> MIDIHandler::getAnswer(const std::string& field, bool includeLabels) const {
  return getAnswer(std::vector<std::string>{ field }, includeLabels);
}

std::vector<std::string> MIDIHandler::getAnswer(const std::vector<std::string>& fields, bool includeLabels) const {
  std::vector<std::string> result;
  const std::deque<MIDIEventData>& queue = getQueue();

  if (!queue.empty()) {
    int lastChordIdx = lastChord(queue);
    result = getChord(lastChordIdx, queue, fields, includeLabels);
  }

  return result;
}


void MIDIHandler::handleMidiMessage(const uint8_t* data, size_t length) {
  // USB-MIDI: 4+ bytes (CIN + MIDI), skip first byte.
  // BLE/raw MIDI: 2-3 bytes, use directly.
  const uint8_t* midiData;
  if (length >= 4)      midiData = data + 1;  // USB-MIDI: skip CIN byte
  else if (length >= 2) midiData = data;      // BLE/raw MIDI: status + data
  else return;

  // Debug callback — fire before parsing
  if (rawMidiCb) rawMidiCb(data, length, midiData);

  unsigned long now = millis();

  uint8_t midiStatus = midiData[0] & 0xF0;
  int channel = (midiData[0] & 0x0F) + 1;
  unsigned long diff = (globalIndex == 0) ? 0 : (now - lastTimestamp);
  lastTimestamp = now;

  // Channel messages other than NoteOn/NoteOff
  if (midiStatus == 0xB0) {  // Control Change
    MIDIEventData event;
    event.index = ++globalIndex;
    event.msgIndex = 0;
    event.timestamp = now;
    event.delay = diff;
    event.channel = channel;
    event.status = "ControlChange";
    event.note = midiData[1];        // Controller number
    event.noteName = "";
    event.noteOctave = "";
    event.velocity = midiData[2];    // CC value
    event.chordIndex = currentChordIndex;
    event.pitchBend = 0;
    addEvent(event);
    return;
  }

  if (midiStatus == 0xC0) {  // Program Change
    MIDIEventData event;
    event.index = ++globalIndex;
    event.msgIndex = 0;
    event.timestamp = now;
    event.delay = diff;
    event.channel = channel;
    event.status = "ProgramChange";
    event.note = midiData[1];  // Program number
    event.noteName = "";
    event.noteOctave = "";
    event.velocity = 0;
    event.chordIndex = currentChordIndex;
    event.pitchBend = 0;
    addEvent(event);
    return;
  }

  if (midiStatus == 0xD0) {  // Channel Pressure (Aftertouch)
    MIDIEventData event;
    event.index = ++globalIndex;
    event.msgIndex = 0;
    event.timestamp = now;
    event.delay = diff;
    event.channel = channel;
    event.status = "ChannelPressure";
    event.note = 0;
    event.noteName = "";
    event.noteOctave = "";
    event.velocity = midiData[1];  // Pressure value
    event.chordIndex = currentChordIndex;
    event.pitchBend = 0;
    addEvent(event);
    return;
  }

  if (midiStatus == 0xE0) {  // Pitch Bend
    int pitchValue = (midiData[1] & 0x7F) | ((midiData[2] & 0x7F) << 7);
    MIDIEventData event;
    event.index = ++globalIndex;
    event.msgIndex = 0;
    event.timestamp = now;
    event.delay = diff;
    event.channel = channel;
    event.status = "PitchBend";
    event.note = 0;
    event.noteName = "";
    event.noteOctave = "";
    event.velocity = 0;
    event.chordIndex = currentChordIndex;
    event.pitchBend = pitchValue;
    addEvent(event);
    return;
  }

  // NoteOn / NoteOff
  int note = midiData[1];
  int velocity = midiData[2];
  std::string statusType;
  int msgIndex = 0;
  int chordIdx = currentChordIndex;

  if (midiStatus == 0x90) {  // NoteOn
    if (velocity > 0) {
      // Velocity filter: ignore ghost notes below threshold
      if (config.velocityThreshold > 0 && velocity < config.velocityThreshold) {
        return;
      }

      statusType = "NoteOn";
      msgIndex = nextMsgIndex++;

      // Chord detection: determine if this NoteOn starts a new chord
      bool startNewChord = false;
      if (activeNotes.empty()) {
        startNewChord = true;
      } else if (config.chordTimeWindow > 0 && (now - lastNoteOnTimestamp) > config.chordTimeWindow) {
        startNewChord = true;
      }

      if (startNewChord) {
        currentChordIndex = nextChordIndex++;
      }

      lastNoteOnTimestamp = now;
      chordIdx = currentChordIndex;
      activeNotes[note] = currentChordIndex;
      activeMsgIndex[note] = msgIndex;
    } else {  // NoteOn with velocity 0 equals NoteOff
      statusType = "NoteOff";
      auto it = activeNotes.find(note);
      if (it != activeNotes.end()) {
        chordIdx = it->second;
        msgIndex = activeMsgIndex[note];
        activeNotes.erase(it);
        activeMsgIndex.erase(note);
      } else {
        chordIdx = currentChordIndex;
      }
    }
  } else if (midiStatus == 0x80) {  // NoteOff
    statusType = "NoteOff";
    auto it = activeNotes.find(note);
    if (it != activeNotes.end()) {
      chordIdx = it->second;
      msgIndex = activeMsgIndex[note];
      activeNotes.erase(it);
      activeMsgIndex.erase(note);
    } else {
      chordIdx = currentChordIndex;
    }
  } else {
    return;  // Unrecognized MIDI message
  }

  if (activeNotes.empty()) {
    currentChordIndex = 0;
  }

  MIDIEventData event;
  event.index = ++globalIndex;
  event.msgIndex = msgIndex;
  event.timestamp = now;
  event.delay = diff;
  event.channel = channel;
  event.status = statusType;
  event.note = note;
  event.noteName = getNoteName(note);
  event.noteOctave = getNoteWithOctave(note);
  event.velocity = velocity;
  event.chordIndex = chordIdx;
  event.pitchBend = 0;

  addEvent(event);
}

// --- MIDI Output (via any transport that supports sending) ---

bool MIDIHandler::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (channel == 0 || channel > 16) return false;
  uint8_t data[3] = { (uint8_t)(0x90 | ((channel - 1) & 0x0F)), note, velocity };
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, 3)) return true;
  }
  return false;
}

bool MIDIHandler::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (channel == 0 || channel > 16) return false;
  uint8_t data[3] = { (uint8_t)(0x80 | ((channel - 1) & 0x0F)), note, velocity };
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, 3)) return true;
  }
  return false;
}

bool MIDIHandler::sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
  if (channel == 0 || channel > 16) return false;
  uint8_t data[3] = { (uint8_t)(0xB0 | ((channel - 1) & 0x0F)), controller, value };
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, 3)) return true;
  }
  return false;
}

bool MIDIHandler::sendProgramChange(uint8_t channel, uint8_t program) {
  if (channel == 0 || channel > 16) return false;
  uint8_t data[2] = { (uint8_t)(0xC0 | ((channel - 1) & 0x0F)), program };
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, 2)) return true;
  }
  return false;
}

bool MIDIHandler::sendPitchBend(uint8_t channel, int value) {
  if (channel == 0 || channel > 16) return false;
  // Normalize -8192..8191 to 0..16383 (center = 8192)
  uint16_t v = (uint16_t)(value + 8192) & 0x3FFF;
  uint8_t data[3] = { (uint8_t)(0xE0 | ((channel - 1) & 0x0F)),
                      (uint8_t)(v & 0x7F),
                      (uint8_t)((v >> 7) & 0x7F) };
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, 3)) return true;
  }
  return false;
}

bool MIDIHandler::sendRaw(const uint8_t* data, size_t length) {
  for (int i = 0; i < transportCount; i++) {
    if (transports[i]->sendMidiMessage(data, length)) return true;
  }
  return false;
}

bool MIDIHandler::sendBleRaw(const uint8_t* data, size_t length) {
  return sendRaw(data, length);
}

#if ESP32_HOST_MIDI_HAS_BLE
bool MIDIHandler::isBleConnected() const {
  return bleTransport.isConnected();
}
#endif


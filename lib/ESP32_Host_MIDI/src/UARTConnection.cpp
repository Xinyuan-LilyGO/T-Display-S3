#include "UARTConnection.h"

UARTConnection::UARTConnection()
    : _serial(nullptr),
      _initialized(false),
      _txPin(-1),
      _bufLen(0),
      _expectedLen(0),
      _runningStatus(0),
      _inSysex(false)
{
    memset(_buf, 0, sizeof(_buf));
}

bool UARTConnection::begin(HardwareSerial& serialPort, int rxPin, int txPin) {
    if (_initialized) return true;

    _serial  = &serialPort;
    _txPin   = txPin;

    // Standard MIDI serial: 31250 baud, 8 data bits, no parity, 1 stop bit.
    serialPort.begin(31250, SERIAL_8N1, rxPin, txPin);

    _initialized = true;
    dispatchConnected();
    return true;
}

bool UARTConnection::isConnected() const {
    return _initialized;
}

// ---------- Receive ----------

void UARTConnection::task() {
    if (!_initialized || !_serial) return;
    while (_serial->available()) {
        _processByte((uint8_t)_serial->read());
    }
}

uint8_t UARTConnection::_midiMsgLength(uint8_t statusByte) {
    if (statusByte >= 0xF0) {
        // System messages
        switch (statusByte) {
            case 0xF0: return 0;  // SysEx — variable length, handled separately
            case 0xF1: return 2;  // MIDI Time Code Quarter Frame
            case 0xF2: return 3;  // Song Position Pointer
            case 0xF3: return 2;  // Song Select
            case 0xF6: return 1;  // Tune Request
            case 0xF7: return 1;  // End of SysEx
            default:   return 1;  // 0xF8–0xFF: Real-time (Clock, Start, Stop…)
        }
    }
    // Channel messages — upper nibble determines length
    switch (statusByte & 0xF0) {
        case 0x80: return 3;  // Note Off
        case 0x90: return 3;  // Note On
        case 0xA0: return 3;  // Polyphonic Key Pressure
        case 0xB0: return 3;  // Control Change
        case 0xC0: return 2;  // Program Change
        case 0xD0: return 2;  // Channel Pressure
        case 0xE0: return 3;  // Pitch Bend
        default:   return 1;
    }
}

void UARTConnection::_processByte(uint8_t byte) {
    if (byte & 0x80) {
        // --- Status byte ---

        // Real-time messages (0xF8–0xFF) can appear anywhere in the stream,
        // even between the bytes of another message. Dispatch immediately
        // without disturbing the current accumulator state.
        if (byte >= 0xF8) {
            dispatchMidiData(&byte, 1);
            return;
        }

        // SysEx start: ignore everything until End of SysEx (0xF7).
        if (byte == 0xF0) {
            _inSysex = true;
            _bufLen = 0;
            _expectedLen = 0;
            return;
        }

        // End of SysEx.
        if (byte == 0xF7) {
            _inSysex = false;
            _bufLen = 0;
            _expectedLen = 0;
            return;
        }

        // Any other status byte while inside SysEx aborts it (protocol error
        // recovery — treat the new status as the start of a fresh message).
        _inSysex = false;

        // Start accumulating the new message.
        _buf[0]      = byte;
        _bufLen      = 1;
        _expectedLen = _midiMsgLength(byte);

        // Update running status for channel messages (0x80–0xEF).
        // System common messages (0xF1–0xF6) clear running status.
        if (byte < 0xF0) {
            _runningStatus = byte;
        } else {
            _runningStatus = 0;
        }

        // Single-byte messages are complete immediately.
        if (_expectedLen == 1) {
            dispatchMidiData(_buf, 1);
            _bufLen = 0;
        }

    } else {
        // --- Data byte ---
        if (_inSysex) return;

        if (_bufLen == 0) {
            // No active status — apply running status if available.
            if (_runningStatus == 0) return;  // Nothing to do; discard.

            _buf[0]      = _runningStatus;
            _buf[1]      = byte;
            _bufLen      = 2;
            _expectedLen = _midiMsgLength(_runningStatus);

            if (_bufLen >= _expectedLen) {
                dispatchMidiData(_buf, _expectedLen);
                _bufLen = 0;
            }
            return;
        }

        // Append to current message.
        if (_bufLen < sizeof(_buf)) {
            _buf[_bufLen++] = byte;
        }

        if (_bufLen >= _expectedLen) {
            dispatchMidiData(_buf, _expectedLen);
            // Keep _buf[0] (running status) but reset the accumulator
            // so the next data byte (if any) re-uses the same status.
            _bufLen = 0;
        }
    }
}

// ---------- Send ----------

bool UARTConnection::sendMidiMessage(const uint8_t* data, size_t length) {
    if (!_initialized || !_serial || _txPin < 0 || length == 0) return false;
    _serial->write(data, length);
    return true;
}

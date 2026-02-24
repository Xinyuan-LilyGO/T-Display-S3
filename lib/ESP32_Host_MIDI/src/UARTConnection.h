#ifndef UART_CONNECTION_H
#define UART_CONNECTION_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "MIDITransport.h"

// UARTConnection — MIDI DIN-5 serial transport at 31250 baud.
//
// Usage:
//   UARTConnection uartMIDI;
//   uartMIDI.begin(Serial1, /*rxPin=*/18, /*txPin=*/17);
//   midiHandler.addTransport(&uartMIDI);
//
// Hardware: connect an optocoupler (6N138, PC900V, TLP2361) between the
// MIDI IN DIN-5 connector and the ESP32 RX pin.
// MIDI OUT requires only two 220Ω resistors (no optocoupler needed on TX).
// Pass txPin = -1 if only receiving.

class UARTConnection : public MIDITransport {
public:
    UARTConnection();

    // Configures and opens the serial port at 31250 baud (MIDI standard).
    //   serialPort : HardwareSerial instance (Serial1, Serial2, …)
    //   rxPin      : GPIO for MIDI IN (via optocoupler). Pass -1 for TX-only.
    //   txPin      : GPIO for MIDI OUT. Pass -1 for RX-only.
    // Returns true on success. Idempotent — safe to call more than once.
    bool begin(HardwareSerial& serialPort, int rxPin, int txPin = -1);

    // Drains the HardwareSerial RX buffer and dispatches complete MIDI
    // messages via MIDITransport callbacks. Call from loop().
    void task() override;

    // Returns true after a successful begin().
    // MIDI DIN-5 has no handshake — "connected" means the port is open.
    bool isConnected() const override;

    // Sends raw MIDI bytes over the TX pin.
    // Returns false if txPin was not configured (-1) or begin() not called.
    bool sendMidiMessage(const uint8_t* data, size_t length) override;

private:
    HardwareSerial* _serial;
    bool _initialized;
    int _txPin;

    // MIDI byte accumulator — builds complete messages from the byte stream.
    uint8_t _buf[3];        // Up to 3 bytes per message
    uint8_t _bufLen;        // Bytes accumulated so far
    uint8_t _expectedLen;   // Total bytes expected for current message
    uint8_t _runningStatus; // Last channel status byte (running status)
    bool _inSysex;          // True while inside a SysEx message (ignored)

    // Returns the total byte count for a given MIDI status byte.
    uint8_t _midiMsgLength(uint8_t statusByte);

    // Processes one incoming byte through the state machine.
    void _processByte(uint8_t byte);
};

#endif // UART_CONNECTION_H

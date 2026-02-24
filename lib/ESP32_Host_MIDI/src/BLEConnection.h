#ifndef BLE_CONNECTION_H
#define BLE_CONNECTION_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <freertos/portmacro.h>
#include <freertos/semphr.h>
#include "MIDITransport.h"

// Standard BLE MIDI Service UUIDs (Apple/MIDI Association specification)
#define BLE_MIDI_SERVICE_UUID        "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"
#define BLE_MIDI_CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3"

// Structure to store a raw BLE MIDI packet (after BLE MIDI header is stripped).
struct RawBleMessage {
    uint8_t data[20];  // Up to 18 MIDI bytes (BLE MIDI safe MTU minus 2-byte header)
    size_t length;
};

class BLEConnection : public MIDITransport {
public:
    BLEConnection();
    virtual ~BLEConnection();

    // Initializes the BLE MIDI server and starts advertising.
    void begin(const std::string& deviceName = "ESP32 MIDI BLE");

    // Drains the ring buffer and dispatches MIDI data via MIDITransport callbacks. Call from loop().
    void task() override;

    // Returns whether a BLE central is currently connected.
    bool isConnected() const override;

    // Sends a MIDI message via BLE NOTIFY.
    // data: raw MIDI bytes (status + data, no BLE header, no CIN byte).
    // Returns true if connected and notification was sent.
    bool sendMidiMessage(const uint8_t* data, size_t length) override;

protected:
    BLEServer* pServer;
    BLECharacteristic* pCharacteristic;
    BLECharacteristicCallbacks* pBleCallback;   // Managed to prevent memory leak
    BLEServerCallbacks* pServerCallback;        // Managed to prevent memory leak
    SemaphoreHandle_t sendMutex;

    // Ring buffer for incoming BLE MIDI packets.
    // Protected by spinlock — same pattern as USBConnection.
    static const int QUEUE_SIZE = 64;
    RawBleMessage bleQueue[QUEUE_SIZE];
    volatile int queueHead;
    volatile int queueTail;
    portMUX_TYPE queueMux;

    bool enqueueMidiMessage(const uint8_t* data, size_t length);
    bool dequeueMidiMessage(RawBleMessage& msg);
    void processQueue();
};

#endif // BLE_CONNECTION_H

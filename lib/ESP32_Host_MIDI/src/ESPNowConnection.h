#ifndef ESPNOW_CONNECTION_H
#define ESPNOW_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <freertos/portmacro.h>
#include "MIDITransport.h"

// Structure to store a raw ESP-NOW MIDI packet.
struct RawEspNowMessage {
    uint8_t data[4];   // Up to 3 MIDI bytes (status + data1 + data2)
    size_t length;
};

class ESPNowConnection : public MIDITransport {
public:
    ESPNowConnection();
    ~ESPNowConnection();

    // Initializes WiFi (STA mode, no connection) and ESP-NOW.
    // channel: WiFi channel (0 = default, 1-14 = fixed). Returns false on failure.
    bool begin(uint8_t channel = 0);

    // Drains the ring buffer and dispatches MIDI data via MIDITransport callbacks. Call from loop().
    void task() override;

    // Returns true after successful begin().
    bool isConnected() const override;

    // Sends raw MIDI bytes via ESP-NOW (broadcast or unicast).
    bool sendMidiMessage(const uint8_t* data, size_t length) override;

    // --- Peer management ---

    // Adds a unicast peer. If no peers are added, broadcast is used.
    bool addPeer(const uint8_t mac[6]);

    // Removes a peer.
    bool removePeer(const uint8_t mac[6]);

    // Returns this device's MAC address (so the other side can add it as a peer).
    void getLocalMAC(uint8_t mac[6]) const;

private:
    bool initialized;
    uint8_t broadcastMAC[6];  // FF:FF:FF:FF:FF:FF
    bool hasPeers;

    // Ring buffer for incoming ESP-NOW MIDI packets.
    // Protected by spinlock — same pattern as USBConnection/BLEConnection.
    static const int QUEUE_SIZE = 64;
    RawEspNowMessage espNowQueue[QUEUE_SIZE];
    volatile int queueHead;
    volatile int queueTail;
    portMUX_TYPE queueMux;

    bool enqueueMidiMessage(const uint8_t* data, size_t length);
    bool dequeueMidiMessage(RawEspNowMessage& msg);
    void processQueue();

    // Static callbacks — ESP-NOW uses global callbacks (no void* context),
    // so we use a singleton pointer to route to the instance.
    static ESPNowConnection* _instance;

    #if ESP_ARDUINO_VERSION_MAJOR >= 3
    static void _onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len);
    #else
    static void _onReceive(const uint8_t* mac, const uint8_t* data, int len);
    #endif

    #if ESP_ARDUINO_VERSION_MAJOR >= 3
    static void _onSend(const wifi_tx_info_t* info, esp_now_send_status_t status);
    #else
    static void _onSend(const uint8_t* mac, esp_now_send_status_t status);
    #endif
};

#endif // ESPNOW_CONNECTION_H

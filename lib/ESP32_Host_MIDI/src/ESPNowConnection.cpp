#include "ESPNowConnection.h"

// Singleton pointer — ESP-NOW callbacks are global (no void* context parameter).
ESPNowConnection* ESPNowConnection::_instance = nullptr;

ESPNowConnection::ESPNowConnection()
    : initialized(false),
      hasPeers(false),
      queueHead(0),
      queueTail(0),
      queueMux(portMUX_INITIALIZER_UNLOCKED)
{
    memset(broadcastMAC, 0xFF, 6);
}

ESPNowConnection::~ESPNowConnection() {
    if (initialized) {
        esp_now_deinit();
        initialized = false;
    }
    if (_instance == this) _instance = nullptr;
}

bool ESPNowConnection::begin(uint8_t channel) {
    if (initialized) return true;

    // WiFi must be started before ESP-NOW.
    // If already active (user connected to a network), don't reinitialize.
    if (WiFi.getMode() == WIFI_OFF) {
        WiFi.mode(WIFI_STA);
    }
    // Disable power saving for lowest latency.
    esp_wifi_set_ps(WIFI_PS_NONE);

    // Set fixed channel if specified (1-14).
    if (channel > 0 && channel <= 14) {
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    }

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    _instance = this;

    esp_now_register_recv_cb(_onReceive);
    esp_now_register_send_cb(_onSend);

    // Register broadcast peer (always available for sending).
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    esp_now_add_peer(&peerInfo);

    initialized = true;
    dispatchConnected();
    return true;
}

void ESPNowConnection::task() {
    processQueue();
}

bool ESPNowConnection::isConnected() const {
    return initialized;
}

// ---------- Receive (callback → ring buffer → task → dispatch) ----------

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void ESPNowConnection::_onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
#else
void ESPNowConnection::_onReceive(const uint8_t* mac, const uint8_t* data, int len) {
#endif
    if (!_instance || len < 2 || len > 3) return;
    _instance->enqueueMidiMessage(data, len);
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void ESPNowConnection::_onSend(const wifi_tx_info_t* info, esp_now_send_status_t status) {
    (void)info;
#else
void ESPNowConnection::_onSend(const uint8_t* mac, esp_now_send_status_t status) {
    (void)mac;
#endif
    (void)status;
}

// ---------- Send ----------

bool ESPNowConnection::sendMidiMessage(const uint8_t* data, size_t length) {
    if (!initialized || length == 0 || length > 3) return false;
    esp_err_t result = esp_now_send(broadcastMAC, data, length);
    return (result == ESP_OK);
}

// ---------- Peer Management ----------

bool ESPNowConnection::addPeer(const uint8_t mac[6]) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result == ESP_OK) {
        hasPeers = true;
        return true;
    }
    return false;
}

bool ESPNowConnection::removePeer(const uint8_t mac[6]) {
    return (esp_now_del_peer(mac) == ESP_OK);
}

void ESPNowConnection::getLocalMAC(uint8_t mac[6]) const {
    WiFi.macAddress(mac);
}

// ---------- Ring Buffer ----------
// Same pattern as USBConnection/BLEConnection.
// Spinlock protects cross-task access: enqueue runs in the WiFi task,
// dequeue runs in the main loop.

bool ESPNowConnection::enqueueMidiMessage(const uint8_t* data, size_t length) {
    portENTER_CRITICAL(&queueMux);
    int next = (queueHead + 1) % QUEUE_SIZE;
    if (next == queueTail) {
        portEXIT_CRITICAL(&queueMux);
        return false;
    }
    size_t copyLen = (length > sizeof(espNowQueue[0].data))
                   ? sizeof(espNowQueue[0].data) : length;
    memcpy(espNowQueue[queueHead].data, data, copyLen);
    espNowQueue[queueHead].length = copyLen;
    queueHead = next;
    portEXIT_CRITICAL(&queueMux);
    return true;
}

bool ESPNowConnection::dequeueMidiMessage(RawEspNowMessage& msg) {
    portENTER_CRITICAL(&queueMux);
    if (queueTail == queueHead) {
        portEXIT_CRITICAL(&queueMux);
        return false;
    }
    msg = espNowQueue[queueTail];
    queueTail = (queueTail + 1) % QUEUE_SIZE;
    portEXIT_CRITICAL(&queueMux);
    return true;
}

void ESPNowConnection::processQueue() {
    RawEspNowMessage msg;
    while (dequeueMidiMessage(msg)) {
        dispatchMidiData(msg.data, msg.length);
    }
}

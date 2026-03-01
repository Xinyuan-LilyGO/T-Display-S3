#include "SenderDisplay.h"
#include "mapping.h"
#include <cstring>

SenderDisplay senderDisplay;

// ── Mini piano geometry ──────────────────────────────────────────────────────
static const int MINI_KEYS     = 25;      // C to C, 2 octaves
static const int MINI_START    = 48;      // C3 (covers most demo sequences)
static const int MINI_WHITE    = 15;
static const int MINI_Y        = 90;
static const int MINI_H        = 50;
static const int MINI_WHITE_W  = 21;
static const int MINI_BLACK_W  = 12;
static const int MINI_BLACK_H  = 30;
static const int MINI_X0       = (SD_SCREEN_W - MINI_WHITE * MINI_WHITE_W) / 2;

static const int S2W[12] = {0,-1,1,-1,2,3,-1,4,-1,5,-1,6};
static const int BLN[12] = {-1,0,-1,2,-1,-1,5,-1,7,-1,9,-1};

SenderDisplay::SenderDisplay()
    : _screen(&_tft)
{
}

void SenderDisplay::init() {
    _tft.init();
    _tft.setRotation(2);
    _tft.setBrightness(255);
    _tft.fillScreen(TFT_BLACK);
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    _screen.setColorDepth(16);
    if (!_screen.createSprite(SD_SCREEN_W, SD_SCREEN_H)) {
        Serial.println("[SenderDisplay] ERROR: sprite alloc failed!");
    }
    _screen.setTextDatum(lgfx::top_left);
}

void SenderDisplay::render(const SenderInfo& info) {
    _screen.fillScreen(SD_COL_BG);

    _drawStatusBar(info);
    _drawSequenceInfo(info);
    _drawMidiBytes(info);
    _drawMiniPiano(info.activeNotes);
    _drawProgressBar(info);
    _drawControls(info);

    _tft.startWrite();
    _screen.pushSprite(0, 0);
    _tft.endWrite();
}

// ── Status bar (BLE connection) ─────────────────────────────────────────────

void SenderDisplay::_drawStatusBar(const SenderInfo& info) {
    _screen.fillRect(0, 0, SD_SCREEN_W, 16, SD_COL_HEADER);

    _screen.setFont(&fonts::Font0);

    // BLE status indicator
    uint16_t dotCol, textCol;
    const char* statusText;

    if (info.bleConnected) {
        dotCol    = SD_COL_BLE_ON;
        textCol   = SD_COL_BLE_ON;
        statusText = "BLE Connected to \"ESP32 BLE Piano\"";
    } else if (info.bleScanning) {
        dotCol    = SD_COL_BLE_SCAN;
        textCol   = SD_COL_BLE_SCAN;
        statusText = "BLE Scanning...";
    } else {
        dotCol    = SD_COL_BLE_OFF;
        textCol   = SD_COL_BLE_OFF;
        statusText = "BLE Disconnected";
    }

    // Status dot
    _screen.fillCircle(8, 8, 4, dotCol);

    // Status text
    _screen.setTextColor(textCol, SD_COL_HEADER);
    _screen.drawString(statusText, 16, 4);
}

// ── Sequence name + current notes ───────────────────────────────────────────

void SenderDisplay::_drawSequenceInfo(const SenderInfo& info) {
    int y = 20;

    // Sequence name (large)
    _screen.setFont(&fonts::Font2);
    _screen.setTextColor(SD_COL_TEXT, SD_COL_BG);
    _screen.drawString(info.sequenceName ? info.sequenceName : "---", 4, y);

    // Playing/Stopped badge
    if (info.playing) {
        _screen.fillRoundRect(260, y, 56, 16, 3, SD_COL_PLAYING);
        _screen.setTextColor(0x0000, SD_COL_PLAYING);
        _screen.setFont(&fonts::Font0);
        _screen.drawString("PLAYING", 265, y + 4);
    } else {
        _screen.fillRoundRect(260, y, 56, 16, 3, SD_COL_STOPPED);
        _screen.setTextColor(0x0000, SD_COL_STOPPED);
        _screen.setFont(&fonts::Font0);
        _screen.drawString("STOPPED", 265, y + 4);
    }

    y += 22;

    // "Sending: C4  E4  G4" — human-readable note names
    _screen.setFont(&fonts::Font2);

    if (info.currentNoteCount > 0 && info.currentStatus == 0x90) {
        _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
        _screen.drawString("Sending:", 4, y);

        int x = 68;
        for (int i = 0; i < info.currentNoteCount && i < 6; i++) {
            uint8_t note = info.currentNotes[i];
            char nbuf[8];
            snprintf(nbuf, sizeof(nbuf), "%s%d", midiNoteName(note), midiNoteOctave(note));

            // Note badge (yellow for white keys, orange for black keys)
            int bw = strlen(nbuf) * 10 + 10;
            bool isBlack = (S2W[note % 12] < 0);
            uint16_t badgeCol = isBlack ? SD_COL_CHORD : SD_COL_NOTE;
            _screen.fillRoundRect(x, y - 1, bw, 17, 3, badgeCol);
            _screen.setTextColor(0x0000, badgeCol);
            _screen.drawString(nbuf, x + 5, y);
            x += bw + 4;
        }
    } else if (!info.playing) {
        _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
        _screen.drawString("Press B2 to play", 4, y);
    } else {
        _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
        _screen.drawString("...", 4, y);
    }
}

// ── MIDI bytes (educational hex display) ────────────────────────────────────

void SenderDisplay::_drawMidiBytes(const SenderInfo& info) {
    int y = 64;

    _screen.setFont(&fonts::Font0);

    if (info.currentNoteCount > 0 && info.currentStatus != 0) {
        // Label
        _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
        _screen.drawString("MIDI:", 4, y);

        // Status byte
        char hexBuf[8];
        int x = 38;

        // Status byte with meaning
        snprintf(hexBuf, sizeof(hexBuf), "0x%02X", info.currentStatus);
        _screen.fillRoundRect(x, y - 1, 34, 11, 2, 0x2945);
        _screen.setTextColor(SD_COL_ACCENT, 0x2945);
        _screen.drawString(hexBuf, x + 2, y);
        x += 38;

        // Data bytes
        for (int i = 0; i < info.currentNoteCount && i < 2; i++) {
            uint8_t val = (i == 0) ? info.currentNotes[0] : info.currentVelocity;
            snprintf(hexBuf, sizeof(hexBuf), "0x%02X", val);
            _screen.fillRoundRect(x, y - 1, 34, 11, 2, 0x2945);
            _screen.setTextColor(SD_COL_NOTE, 0x2945);
            _screen.drawString(hexBuf, x + 2, y);
            x += 38;
        }

        // Human-readable description
        x += 4;
        const char* desc = (info.currentStatus == 0x90) ? "NoteOn" : "NoteOff";
        _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
        char descBuf[32];
        snprintf(descBuf, sizeof(descBuf), "%s ch1", desc);
        _screen.drawString(descBuf, x, y);

    } else {
        _screen.setTextColor(0x2945, SD_COL_BG);
        _screen.drawString("MIDI: ---", 4, y);
    }

    // Divider
    _screen.drawFastHLine(0, 78, SD_SCREEN_W, SD_COL_DIVIDER);

    // Label for piano section
    _screen.setFont(&fonts::Font0);
    _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
    _screen.drawString("C3", MINI_X0 - 14, MINI_Y + 20);
    _screen.drawString("C5", MINI_X0 + MINI_WHITE * MINI_WHITE_W + 2, MINI_Y + 20);
}

// ── Mini piano (same visual as PianoDisplay) ────────────────────────────────

void SenderDisplay::_drawMiniPiano(const bool notes[128]) {
    // Background
    _screen.fillRect(MINI_X0 - 1, MINI_Y, MINI_WHITE * MINI_WHITE_W + 2, MINI_H, SD_COL_KEY_BORDER);

    if (!notes) return;

    // White keys
    for (int n = MINI_START; n < MINI_START + MINI_KEYS; n++) {
        int st = n % 12;
        if (S2W[st] < 0) continue;
        int wi = ((n - MINI_START) / 12) * 7 + S2W[st];
        int x = MINI_X0 + wi * MINI_WHITE_W;
        uint16_t col = notes[n] ? SD_COL_KEY_W_ACT : SD_COL_KEY_WHITE;
        _screen.fillRect(x, MINI_Y + 1, MINI_WHITE_W - 1, MINI_H - 2, col);
    }

    // Black keys
    for (int n = MINI_START; n < MINI_START + MINI_KEYS; n++) {
        int st = n % 12;
        if (S2W[st] >= 0) continue;
        int nbSt = BLN[st];
        int nbN  = (n / 12) * 12 + nbSt;
        int nbWi = ((nbN - MINI_START) / 12) * 7 + S2W[nbSt];
        int x = MINI_X0 + nbWi * MINI_WHITE_W + MINI_WHITE_W - MINI_BLACK_W / 2;
        uint16_t col = notes[n] ? SD_COL_KEY_B_ACT : SD_COL_KEY_BLACK;
        _screen.fillRect(x, MINI_Y + 1, MINI_BLACK_W, MINI_BLACK_H, col);
        _screen.drawRect(x, MINI_Y + 1, MINI_BLACK_W, MINI_BLACK_H, SD_COL_KEY_BORDER);
    }
}

// ── Progress bar ────────────────────────────────────────────────────────────

void SenderDisplay::_drawProgressBar(const SenderInfo& info) {
    int y = 144;

    _screen.setFont(&fonts::Font0);
    _screen.setTextColor(SD_COL_DIM, SD_COL_BG);

    // Step counter
    char stepBuf[16];
    snprintf(stepBuf, sizeof(stepBuf), "Step %d/%d", info.currentStep + 1, info.totalSteps);
    _screen.drawString(stepBuf, 4, y);

    // Progress bar
    int barX = 70;
    int barW = 240;
    int barH = 6;
    _screen.fillRoundRect(barX, y + 1, barW, barH, 2, SD_COL_BAR_BG);

    if (info.totalSteps > 0) {
        int fillW = (barW * (info.currentStep + 1)) / info.totalSteps;
        if (fillW < 4) fillW = 4;
        _screen.fillRoundRect(barX, y + 1, fillW, barH, 2, SD_COL_BAR_FG);
    }
}

// ── Controls help ───────────────────────────────────────────────────────────

void SenderDisplay::_drawControls(const SenderInfo& info) {
    int y = 158;

    _screen.setFont(&fonts::Font0);
    _screen.setTextColor(SD_COL_DIM, SD_COL_BG);
    _screen.drawString("B1: Next Seq", 4, y);

    _screen.setTextColor(info.playing ? SD_COL_PLAYING : SD_COL_DIM, SD_COL_BG);
    _screen.drawString(info.playing ? "B2: Stop" : "B2: Play", 240, y);
}

#include "PianoDisplay.h"
#include "mapping.h"
#include <cstring>

PianoDisplay piano;

PianoDisplay::PianoDisplay()
    : _screen(&_tft), _viewStart(VIEW_DEFAULT)
{
}

void PianoDisplay::init() {
    _tft.init();
    _tft.setRotation(2);
    _tft.setBrightness(255);
    _tft.fillScreen(TFT_BLACK);
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    _screen.setColorDepth(16);
    if (!_screen.createSprite(SCREEN_W, SCREEN_H)) {
        Serial.println("[PianoDisplay] ERROR: sprite alloc failed!");
    }
    _screen.setTextDatum(lgfx::top_left);
}

void PianoDisplay::render(const bool activeNotes[128], const PianoInfo& info) {
    _drawPiano(activeNotes);
    _drawInfoBar(activeNotes, info);

    _tft.startWrite();
    _screen.pushSprite(0, 0);
    _tft.endWrite();
}

void PianoDisplay::shiftOctave(int delta) {
    int next = _viewStart + delta;
    if (next < VIEW_MIN) next = VIEW_MIN;
    if (next > VIEW_MAX) next = VIEW_MAX;
    _viewStart = next;
}

void PianoDisplay::setViewStart(int midi) {
    int newStart = (midi / 12) * 12;
    if (newStart > VIEW_MAX) newStart = VIEW_MAX;
    if (newStart < VIEW_MIN) newStart = VIEW_MIN;
    if (newStart == _viewStart) return;
    _viewStart = newStart;
}

// ── Private: Piano drawing ──────────────────────────────────────────────────

void PianoDisplay::_drawPiano(const bool activeNotes[128]) {
    _screen.fillRect(0, PIANO_Y, SCREEN_W, PIANO_H, COL_KEY_BORDER);

    // White keys
    for (int n = _viewStart; n < _viewStart + KEYS_SPAN; n++) {
        int st = n % 12;
        if (SEMITONE_TO_WHITE[st] < 0) continue;
        int wi = ((n - _viewStart) / 12) * 7 + SEMITONE_TO_WHITE[st];
        if (wi < 0 || wi >= WHITE_KEYS) continue;
        int x = PIANO_X + wi * WHITE_KEY_W;
        uint32_t col = activeNotes[n] ? COL_WHITE_ACTIVE : COL_WHITE_NORMAL;
        _screen.fillRect(x, PIANO_Y + 1, WHITE_KEY_W - 1, WHITE_KEY_H - 2, col);
        _screen.drawFastHLine(x, PIANO_Y + WHITE_KEY_H - 2, WHITE_KEY_W - 1, COL_KEY_BORDER);
    }
    // Black keys
    for (int n = _viewStart; n < _viewStart + KEYS_SPAN; n++) {
        int st = n % 12;
        if (SEMITONE_TO_WHITE[st] >= 0) continue;
        int nbSt = BLACK_LEFT_NEIGHBOR[st];
        int nbNote = (n / 12) * 12 + nbSt;
        int nbWi = ((nbNote - _viewStart) / 12) * 7 + SEMITONE_TO_WHITE[nbSt];
        int x = PIANO_X + nbWi * WHITE_KEY_W + WHITE_KEY_W - BLACK_KEY_W / 2;
        uint32_t col = activeNotes[n] ? COL_BLACK_ACTIVE : COL_BLACK_NORMAL;
        _screen.fillRect(x, PIANO_Y + 1, BLACK_KEY_W, BLACK_KEY_H, col);
        _screen.drawRect(x, PIANO_Y + 1, BLACK_KEY_W, BLACK_KEY_H, COL_KEY_BORDER);
    }
}

// ── Private: Info bar ───────────────────────────────────────────────────────

void PianoDisplay::_drawInfoBar(const bool activeNotes[128],
                                 const PianoInfo& info) {
    _screen.fillRect(0, 0, SCREEN_W, INFO_H, COL_INFO_BG);
    _screen.drawFastHLine(0, INFO_H - 1, SCREEN_W, COL_HEADER_BG);

    // ── BLE status (top-left corner) ──────────────────────────────────────────
    _screen.setFont(&fonts::Font0);
    if (info.bleConnected) {
        _screen.fillRoundRect(2, 2, 78, 12, 2, 0x0300);  // dark green bg
        _screen.setTextColor(COL_BLE_ON, 0x0300);
        _screen.drawString("BLE Connected", 5, 4);
    } else {
        _screen.fillRoundRect(2, 2, 78, 12, 2, 0x2945);
        _screen.setTextColor(COL_BLE_OFF, 0x2945);
        _screen.drawString("BLE Waiting..", 5, 4);
    }

    // ── Range label (left) ────────────────────────────────────────────────────
    int viewEnd = _viewStart + KEYS_SPAN - 1;
    char rangeLabel[16];
    snprintf(rangeLabel, sizeof(rangeLabel), "%s%d-%s%d",
        NOTE_NAMES[_viewStart % 12], (_viewStart / 12) - 1,
        NOTE_NAMES[viewEnd   % 12], (viewEnd   / 12) - 1);

    _screen.setFont(&fonts::Font2);
    _screen.setTextColor(COL_DIM, COL_INFO_BG);
    _screen.drawString(rangeLabel, 3, 18);

    // ── Right zone: Count + Hz ────────────────────────────────────────────────
    _screen.drawFastVLine(264, 16, INFO_H - 18, COL_DIVIDER);

    if (info.noteCount > 0) {
        char cntBuf[4];
        snprintf(cntBuf, sizeof(cntBuf), "%d", info.noteCount);
        _screen.setTextColor(COL_ACCENT, COL_INFO_BG);
        _screen.drawString(cntBuf, 269, 18);
    }

    if (info.rootFreq > 0.0f) {
        char hzBuf[12];
        snprintf(hzBuf, sizeof(hzBuf), "%dHz", (int)(info.rootFreq + 0.5f));
        _screen.setTextColor(COL_FREQ, COL_INFO_BG);
        _screen.drawString(hzBuf, 268, 34);
    }

    // ── Center zone: Note names ───────────────────────────────────────────────
    _screen.setClipRect(82, 16, 180, INFO_H - 16);

    if (info.noteCount == 0) {
        _screen.setFont(&fonts::Font2);
        _screen.setTextColor(COL_WAITING, COL_INFO_BG);
        _screen.drawString("Waiting for MIDI...", 84, 22);
    } else {
        // Row 1: note names
        _screen.setFont(&fonts::Font2);
        _screen.setTextColor(COL_NOTE, COL_INFO_BG);
        _screen.drawString(info.noteStr, 84, 18);

        // Row 2: root note detail
        if (info.noteCount == 1) {
            int octave = (info.rootMidi / 12) - 1;
            char detBuf[32];
            snprintf(detBuf, sizeof(detBuf), "Oct.%d  MIDI %d", octave, info.rootMidi);
            _screen.setFont(&fonts::Font0);
            _screen.setTextColor(COL_ACCENT, COL_INFO_BG);
            _screen.drawString(detBuf, 84, 36);
        }
    }

    _screen.clearClipRect();

    // ── Out-of-view indicators ────────────────────────────────────────────────
    bool hasBelow = false, hasAbove = false;
    for (int n = 0; n < 128; n++) {
        if (!activeNotes[n]) continue;
        if (n < _viewStart)              hasBelow = true;
        if (n >= _viewStart + KEYS_SPAN) hasAbove = true;
    }
    if (hasBelow) {
        _screen.fillRoundRect(0, 22, 14, 14, 3, 0xF800);
        _screen.setFont(&fonts::Font0);
        _screen.setTextColor(COL_TEXT, 0xF800);
        _screen.drawString("<", 3, 25);
    }
    if (hasAbove) {
        _screen.fillRoundRect(306, 22, 14, 14, 3, 0x001F);
        _screen.setFont(&fonts::Font0);
        _screen.setTextColor(COL_TEXT, 0x001F);
        _screen.drawString(">", 309, 25);
    }
}

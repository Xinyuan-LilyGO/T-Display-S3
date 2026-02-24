#include "PianoDisplay.h"
#include "mapping.h"
#include <cstring>

PianoDisplay piano;

PianoDisplay::PianoDisplay()
    : _screen(&_tft), _viewStart(VIEW_DEFAULT)
{
}

// ── Public ────────────────────────────────────────────────────────────────────

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
    // Always redraw full frame — avoids any change-detection bugs.
    // The sprite approach makes this cheap (all drawing is in RAM).
    _drawPiano(activeNotes);
    _drawInfoBar(activeNotes, info);

    // Single transfer — eliminates diagonal tearing
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
    Serial.printf("[PianoDisplay] setViewStart: %d -> %d (MIDI %d)\n",
                  _viewStart, newStart, midi);
    _viewStart = newStart;
}

// ── Private: Piano drawing ──────────────────────────────────────────────────

void PianoDisplay::_drawPiano(const bool activeNotes[128]) {
    _screen.fillRect(0, PIANO_Y, SCREEN_W, PIANO_H, COL_KEY_BORDER);

    // White keys first (bottom layer) — inline logic matching debug mini-piano
    for (int n = _viewStart; n < _viewStart + KEYS_SPAN; n++) {
        int st = n % 12;
        if (SEMITONE_TO_WHITE[st] < 0) continue;  // skip black keys
        int wi = ((n - _viewStart) / 12) * 7 + SEMITONE_TO_WHITE[st];
        if (wi < 0 || wi >= WHITE_KEYS) continue;
        int x = PIANO_X + wi * WHITE_KEY_W;
        uint32_t col = activeNotes[n] ? COL_WHITE_ACTIVE : COL_WHITE_NORMAL;
        _screen.fillRect(x, PIANO_Y + 1, WHITE_KEY_W - 1, WHITE_KEY_H - 2, col);
        _screen.drawFastHLine(x, PIANO_Y + WHITE_KEY_H - 2, WHITE_KEY_W - 1, COL_KEY_BORDER);
    }
    // Black keys on top — inline logic matching debug mini-piano
    for (int n = _viewStart; n < _viewStart + KEYS_SPAN; n++) {
        int st = n % 12;
        if (SEMITONE_TO_WHITE[st] >= 0) continue;  // skip white keys
        int nbSt = BLACK_LEFT_NEIGHBOR[st];
        int nbNote = (n / 12) * 12 + nbSt;
        int nbWi = ((nbNote - _viewStart) / 12) * 7 + SEMITONE_TO_WHITE[nbSt];
        int x = PIANO_X + nbWi * WHITE_KEY_W + WHITE_KEY_W - BLACK_KEY_W / 2;
        uint32_t col = activeNotes[n] ? COL_BLACK_ACTIVE : COL_BLACK_NORMAL;
        _screen.fillRect(x, PIANO_Y + 1, BLACK_KEY_W, BLACK_KEY_H, col);
        _screen.drawRect(x, PIANO_Y + 1, BLACK_KEY_W, BLACK_KEY_H, COL_KEY_BORDER);
    }
}

// ── Private: Info bar (Explorer-inspired design) ────────────────────────────

void PianoDisplay::_drawBadge(int x, int y, int w, int h, uint16_t bg,
                               const char* text) {
    _screen.fillRoundRect(x, y, w, h, 3, bg);
    _screen.setTextColor(0x0000, bg);
    int tw = _screen.textWidth(text);
    _screen.drawString(text, x + (w - tw) / 2, y + 2);
}

void PianoDisplay::_drawInfoBar(const bool activeNotes[128],
                                 const PianoInfo& info) {
    // Background
    _screen.fillRect(0, 0, SCREEN_W, INFO_H, COL_INFO_BG);
    _screen.drawFastHLine(0, INFO_H - 1, SCREEN_W, COL_HEADER_BG);

    // ── Left zone (0-59): Range + nav ───────────────────────────────────────
    _screen.drawFastVLine(60, 2, INFO_H - 4, COL_DIVIDER);

    int viewEnd = _viewStart + KEYS_SPAN - 1;
    char rangeLabel[16];
    snprintf(rangeLabel, sizeof(rangeLabel), "%s%d-%s%d",
        NOTE_NAMES[_viewStart % 12], (_viewStart / 12) - 1,
        NOTE_NAMES[viewEnd   % 12], (viewEnd   / 12) - 1);

    _screen.setFont(&fonts::Font2);
    _screen.setTextColor(COL_DIM, COL_INFO_BG);
    _screen.drawString(rangeLabel, 3, 4);

    // Nav arrows
    _screen.setTextColor(COL_DIVIDER, COL_INFO_BG);
    _screen.drawString("\x11\x10", 3, 28);   // ◄►

    // ── Right zone (264-319): Count + Hz ────────────────────────────────────
    _screen.drawFastVLine(264, 2, INFO_H - 4, COL_DIVIDER);

    _screen.setFont(&fonts::Font2);
    if (info.noteCount > 0) {
        char cntBuf[4];
        snprintf(cntBuf, sizeof(cntBuf), "%d", info.noteCount);
        _screen.setTextColor(COL_ACCENT, COL_INFO_BG);
        _screen.drawString(cntBuf, 269, 4);
    }

    if (info.rootFreq > 0.0f) {
        char hzBuf[12];
        snprintf(hzBuf, sizeof(hzBuf), "%dHz", (int)(info.rootFreq + 0.5f));
        _screen.setTextColor(COL_FREQ, COL_INFO_BG);
        _screen.drawString(hzBuf, 268, 28);
    }

    // ── Center zone (62-262): Main analysis ─────────────────────────────────
    _screen.setClipRect(62, 0, 200, INFO_H);
    const int CX = 64;

    if (info.noteCount == 0) {
        // Idle state
        _screen.setFont(&fonts::Font2);
        _screen.setTextColor(COL_WAITING, COL_INFO_BG);
        _screen.drawString("Waiting for MIDI...", CX, 16);

    } else if (info.noteCount == 1) {
        // ── Single note ─────────────────────────────────────────────────────
        // Row 1: note name badge (yellow) + MIDI number
        _screen.setFont(&fonts::Font2);
        _drawBadge(CX, 3, 40, 19, COL_NOTE, info.noteStr);

        _screen.setFont(&fonts::Font2);
        _screen.setTextColor(COL_DIM, COL_INFO_BG);
        char midiBuf[12];
        snprintf(midiBuf, sizeof(midiBuf), "MIDI %d", info.rootMidi);
        _screen.drawString(midiBuf, CX + 46, 5);

        // Row 2: octave + frequency
        int octave = (info.rootMidi / 12) - 1;
        char detBuf[40];
        snprintf(detBuf, sizeof(detBuf), "Oct.%d  %.1f Hz", octave, info.rootFreq);
        _screen.setTextColor(COL_ACCENT, COL_INFO_BG);
        _screen.drawString(detBuf, CX, 28);

    } else if (info.noteCount == 2) {
        // ── Interval ────────────────────────────────────────────────────────
        // Row 1: note badges (yellow) + interval badge (cyan)
        _screen.setFont(&fonts::Font2);

        // Parse note names from noteStr "C4  E4"
        char n1[8] = "", n2[8] = "";
        sscanf(info.noteStr, "%7s %7s", n1, n2);

        int bx = CX;
        _drawBadge(bx, 3, 34, 19, COL_NOTE, n1); bx += 37;
        _drawBadge(bx, 3, 34, 19, COL_NOTE, n2); bx += 40;

        if (info.intervalLabel[0]) {
            _drawBadge(bx, 3, 30, 19, COL_INTERVAL, info.intervalLabel);
        }

        // Row 2: full interval name
        _screen.setFont(&fonts::Font2);
        _screen.setTextColor(COL_TEXT, COL_INFO_BG);
        _screen.drawString(info.intervalName, CX, 28);

    } else {
        // ── Chord (3+ notes) ────────────────────────────────────────────────
        // Row 1: chord badge (orange) + full name (yellow text)
        _screen.setFont(&fonts::Font2);
        const char* chordDisp = info.chordName[0] ? info.chordName : "?";
        int cw = strlen(chordDisp) * 12 + 14;
        if (cw < 36) cw = 36;
        _drawBadge(CX, 3, cw, 19, COL_CHORD, chordDisp);

        _screen.setTextColor(COL_NOTE, COL_INFO_BG);
        _screen.drawString(info.chordFullName, CX + cw + 4, 5);

        // Row 2: note badges (yellow, smaller) + formula
        _screen.setFont(&fonts::Font0);

        // Parse individual notes from noteStr
        char tmpStr[64];
        strncpy(tmpStr, info.noteStr, sizeof(tmpStr) - 1);
        tmpStr[sizeof(tmpStr) - 1] = '\0';

        int bx = CX;
        char* tok = strtok(tmpStr, " ");
        while (tok && bx < 220) {
            int tw = strlen(tok) * 6 + 8;
            _screen.fillRoundRect(bx, 28, tw, 15, 2, COL_NOTE);
            _screen.setTextColor(0x0000, COL_NOTE);
            _screen.drawString(tok, bx + 4, 31);
            bx += tw + 3;
            tok = strtok(nullptr, " ");
        }

        // Formula after badges
        if (info.formula[0] && bx < 250) {
            _screen.setTextColor(COL_DIM, COL_INFO_BG);
            char fmtBuf[64];
            snprintf(fmtBuf, sizeof(fmtBuf), "[%s]", info.formula);
            _screen.drawString(fmtBuf, bx + 2, 31);
        }
    }

    _screen.clearClipRect();

    // ── Out-of-view indicators ──────────────────────────────────────────────
    bool hasBelow = false, hasAbove = false;
    for (int n = 0; n < 128; n++) {
        if (!activeNotes[n]) continue;
        if (n < _viewStart)              hasBelow = true;
        if (n >= _viewStart + KEYS_SPAN) hasAbove = true;
    }
    if (hasBelow) {
        _screen.fillRoundRect(0, 18, 14, 14, 3, 0xF800);
        _screen.setFont(&fonts::Font0);
        _screen.setTextColor(COL_TEXT, 0xF800);
        _screen.drawString("<", 3, 21);
    }
    if (hasAbove) {
        _screen.fillRoundRect(306, 18, 14, 14, 3, 0x001F);
        _screen.setFont(&fonts::Font0);
        _screen.setTextColor(COL_TEXT, 0x001F);
        _screen.drawString(">", 309, 21);
    }
}


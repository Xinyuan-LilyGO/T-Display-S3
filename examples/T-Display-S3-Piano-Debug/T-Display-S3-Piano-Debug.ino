// Debug: MIDI Event Monitor for T-Display S3
//
// Shows incoming MIDI events directly on the ST7789 display.
// Use to diagnose USB MIDI issues without Serial port.
//
// Top:    two headers — manual tracking vs fillActiveNotes (MIDIHandler map)
// Middle: colour-coded event log
// Bottom: mini-piano bar (25 keys) using fillActiveNotes — same as Piano example
//
// Colour coding:
//   Cyan    — RAW bytes
//   Green   — NoteOn
//   Yellow  — NoteOff
//   Orange  — ControlChange / PitchBend
//   White   — STATE / info
//
// Controls:
//   Button 1 (GPIO  0): scroll log up
//   Button 2 (GPIO 14): clear log + reset state
//
// Dependencies: ESP32_Host_MIDI, LovyanGFX

#include <Arduino.h>
#include <LovyanGFX.h>
#include <ESP32_Host_MIDI.h>
#include "mapping.h"

// ── LGFX — T-Display S3 (ST7789, 8-bit parallel) ────────────────────────────
class LGFX : public lgfx::LGFX_Device {
    lgfx::Bus_Parallel8 _bus;
    lgfx::Panel_ST7789  _panel;
    lgfx::Light_PWM     _bl;
public:
    LGFX() {
        { auto cfg = _bus.config();
          cfg.pin_wr=8; cfg.pin_rd=9; cfg.pin_rs=7;
          cfg.pin_d0=39; cfg.pin_d1=40; cfg.pin_d2=41; cfg.pin_d3=42;
          cfg.pin_d4=45; cfg.pin_d5=46; cfg.pin_d6=47; cfg.pin_d7=48;
          _bus.config(cfg); _panel.setBus(&_bus); }
        { auto cfg = _panel.config();
          cfg.pin_cs=6; cfg.pin_rst=5; cfg.pin_busy=-1;
          cfg.offset_rotation=1; cfg.offset_x=35;
          cfg.readable=false; cfg.invert=true;
          cfg.rgb_order=false; cfg.dlen_16bit=false; cfg.bus_shared=false;
          cfg.panel_width=170; cfg.panel_height=320;
          _panel.config(cfg); }
        setPanel(&_panel);
        { auto cfg = _bl.config();
          cfg.pin_bl=38; cfg.invert=false; cfg.freq=22000; cfg.pwm_channel=7;
          _bl.config(cfg); _panel.setLight(&_bl); }
    }
};

static const int SCREEN_W = 320;
static const int SCREEN_H = 170;

static LGFX        tft;
static LGFX_Sprite screen(&tft);

// ── Colours ──────────────────────────────────────────────────────────────────
#define COL_BG       0x0841
#define COL_HEADER   0x07FF   // cyan
#define COL_RAW      0x07FF   // cyan
#define COL_NOTEON   0x07E0   // bright green
#define COL_NOTEOFF  0xFFE0   // yellow
#define COL_CC       0xFBE0   // orange
#define COL_STATE    0xFFFF   // white
#define COL_INFO     0xBDF7   // light grey

// ── Event log ────────────────────────────────────────────────────────────────
static const int LOG_CAP  = 80;
static const int LOG_WIDE = 54;

struct LogEntry {
    char     line[LOG_WIDE];
    uint16_t color;
};

static LogEntry logBuf[LOG_CAP];
static int  logHead  = 0;
static int  logCount = 0;
static int  logScroll = 0;

static void clearLog() {
    logHead  = 0;
    logCount = 0;
    logScroll = 0;
}

static void addLogC(uint16_t color, const char* fmt, ...) {
    char buf[LOG_WIDE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    int idx = (logHead + logCount) % LOG_CAP;
    strncpy(logBuf[idx].line, buf, LOG_WIDE - 1);
    logBuf[idx].line[LOG_WIDE - 1] = '\0';
    logBuf[idx].color = color;

    if (logCount < LOG_CAP) logCount++;
    else logHead = (logHead + 1) % LOG_CAP;
    logScroll = 0;
}

// ── Active notes — two independent tracking methods ──────────────────────────
static bool manualNotes[128] = {};   // tracked from queue events
static bool fillNotes[128]   = {};   // from MIDIHandler::fillActiveNotes

// ── MIDI processing ──────────────────────────────────────────────────────────
static int lastEventIdx = -1;

static void processEvents() {
    const auto& queue = midiHandler.getQueue();
    for (const auto& ev : queue) {
        if (ev.index <= lastEventIdx) continue;
        lastEventIdx = ev.index;

        if (ev.status == "NoteOn" && ev.velocity > 0) {
            manualNotes[ev.note] = true;
            addLogC(COL_NOTEON, "ON  n=%d(%s) v=%d c=%d i=%d",
                    ev.note, ev.noteOctave.c_str(),
                    ev.velocity, ev.chordIndex, ev.index);
        } else if (ev.status == "NoteOff" ||
                  (ev.status == "NoteOn" && ev.velocity == 0)) {
            manualNotes[ev.note] = false;
            addLogC(COL_NOTEOFF, "OFF n=%d(%s) v=%d i=%d",
                    ev.note, ev.noteOctave.c_str(), ev.velocity, ev.index);
        } else if (ev.status == "ControlChange") {
            addLogC(COL_CC, "CC  ctrl=%d val=%d ch=%d i=%d",
                    ev.note, ev.velocity, ev.channel, ev.index);
        } else if (ev.status == "PitchBend") {
            addLogC(COL_CC, "PB  val=%d ch=%d i=%d",
                    ev.pitchBend, ev.channel, ev.index);
        } else {
            addLogC(COL_INFO, "%-11s n=%d v=%d i=%d",
                    ev.status.c_str(), ev.note, ev.velocity, ev.index);
        }
    }
}

// ── Raw MIDI callback ────────────────────────────────────────────────────────
static void onRawMidi(const uint8_t* raw, size_t len, const uint8_t* m) {
    uint8_t st = m[0] & 0xF0;
    if (st == 0x80 || st == 0x90) {
        addLogC(COL_RAW, "RAW [%02X%02X%02X%02X] m=%02X%02X%02X",
                len>0?raw[0]:0, len>1?raw[1]:0,
                len>2?raw[2]:0, len>3?raw[3]:0,
                m[0], m[1], m[2]);
    }
}

// ── Mini-piano helpers (same logic as PianoDisplay) ──────────────────────────
static const int MINI_KEYS    = 25;      // C to C, 2 octaves
static const int MINI_START   = 48;      // C3 (default view)
static const int MINI_H       = 20;      // height of mini-piano bar
static const int MINI_Y       = SCREEN_H - MINI_H;
static const int MINI_WHITE_W = 12;      // narrow keys to fit 15 white in ~180px
static const int MINI_BLACK_W = 8;
static const int MINI_BLACK_H = 12;

static const int S2W[12] = {0,-1,1,-1,2,3,-1,4,-1,5,-1,6};  // semitone to white index
static const int BLN[12] = {-1,0,-1,2,-1,-1,5,-1,7,-1,9,-1}; // black left neighbor

static bool isBk(int n)  { return S2W[n % 12] < 0; }

static void drawMiniPiano(const bool notes[128]) {
    int x0 = (SCREEN_W - 15 * MINI_WHITE_W) / 2;

    // Background
    screen.fillRect(0, MINI_Y, SCREEN_W, MINI_H, 0x0000);

    // White keys
    for (int n = MINI_START; n < MINI_START + MINI_KEYS; n++) {
        if (isBk(n)) continue;
        int wi = ((n - MINI_START) / 12) * 7 + S2W[n % 12];
        int x = x0 + wi * MINI_WHITE_W;
        uint16_t col = notes[n] ? 0x07FF : 0xFFFF;  // cyan if active
        screen.fillRect(x, MINI_Y + 1, MINI_WHITE_W - 1, MINI_H - 2, col);
    }
    // Black keys
    for (int n = MINI_START; n < MINI_START + MINI_KEYS; n++) {
        if (!isBk(n)) continue;
        int nbSt = BLN[n % 12];
        int nbN  = (n / 12) * 12 + nbSt;
        int nbWi = ((nbN - MINI_START) / 12) * 7 + S2W[nbSt];
        int x = x0 + nbWi * MINI_WHITE_W + MINI_WHITE_W - MINI_BLACK_W / 2;
        uint16_t col = notes[n] ? 0xFBE0 : 0x2104;  // orange if active
        screen.fillRect(x, MINI_Y + 1, MINI_BLACK_W, MINI_BLACK_H, col);
    }
}

// ── Render ────────────────────────────────────────────────────────────────────
static void render() {
    screen.fillScreen(COL_BG);

    screen.setFont(&fonts::Font0);

    // ── Header line 1: manual tracking ───────────────────────────────────────
    screen.setTextColor(COL_NOTEON, COL_BG);  // green
    char hdr1[LOG_WIDE];
    int p1 = snprintf(hdr1, sizeof(hdr1), "MANUAL: ");
    int mc = 0;
    for (int n = 0; n < 128 && p1 < LOG_WIDE - 4; n++) {
        if (manualNotes[n]) { mc++; p1 += snprintf(hdr1+p1, sizeof(hdr1)-p1, "%d ", n); }
    }
    if (mc == 0) p1 += snprintf(hdr1+p1, sizeof(hdr1)-p1, "(none)");
    screen.drawString(hdr1, 1, 1);

    // ── Header line 2: fillActiveNotes (MIDIHandler internal map) ────────────
    screen.setTextColor(COL_NOTEOFF, COL_BG);  // yellow
    char hdr2[LOG_WIDE];
    int p2 = snprintf(hdr2, sizeof(hdr2), "FILL:   ");
    int fc = 0;
    for (int n = 0; n < 128 && p2 < LOG_WIDE - 4; n++) {
        if (fillNotes[n]) { fc++; p2 += snprintf(hdr2+p2, sizeof(hdr2)-p2, "%d ", n); }
    }
    if (fc == 0) p2 += snprintf(hdr2+p2, sizeof(hdr2)-p2, "(none)");
    screen.drawString(hdr2, 1, 10);

    screen.drawFastHLine(0, 18, SCREEN_W, 0x2945);

    // ── Log lines ────────────────────────────────────────────────────────────
    const int LINE_H  = 8;
    const int Y0      = 20;
    const int LOG_END = MINI_Y - 1;  // stop before mini-piano
    const int VIS     = (LOG_END - Y0) / LINE_H;

    int firstLine = max(0, logCount - VIS - logScroll);
    int y = Y0;

    for (int i = firstLine; i < logCount && y + LINE_H <= LOG_END; i++) {
        int idx = (logHead + i) % LOG_CAP;
        screen.setTextColor(logBuf[idx].color, COL_BG);
        screen.drawString(logBuf[idx].line, 1, y);
        y += LINE_H;
    }

    // ── Scroll bar ───────────────────────────────────────────────────────────
    if (logCount > VIS) {
        int barH = max(4, (LOG_END - Y0) * VIS / logCount);
        int maxScr = max(1, logCount - VIS);
        int barY = Y0 + (LOG_END - Y0 - barH) * (maxScr - logScroll) / maxScr;
        screen.fillRect(SCREEN_W - 3, Y0, 3, LOG_END - Y0, 0x2945);
        screen.fillRect(SCREEN_W - 3, barY, 3, barH, COL_HEADER);
    }

    // ── Mini-piano bar at bottom (uses fillNotes = MIDIHandler map) ──────────
    drawMiniPiano(fillNotes);

    // ── Label: which source drives the mini-piano ────────────────────────────
    screen.setFont(&fonts::Font0);
    screen.setTextColor(COL_INFO, 0x0000);
    screen.drawString("fillActiveNotes", 1, MINI_Y + 2);

    tft.startWrite();
    screen.pushSprite(0, 0);
    tft.endWrite();
}

// ── Buttons ──────────────────────────────────────────────────────────────────
static unsigned long btn1Last = 0, btn2Last = 0;
static const unsigned long DEBOUNCE_MS = 150;

// ── Setup ────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);

    tft.init();
    tft.setRotation(2);
    tft.setBrightness(255);
    tft.fillScreen(TFT_BLACK);
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    screen.setColorDepth(16);
    if (!screen.createSprite(SCREEN_W, SCREEN_H)) {
        Serial.println("[Debug] ERROR: sprite alloc failed");
    }
    screen.setTextDatum(lgfx::top_left);

    MIDIHandlerConfig cfg;
    cfg.maxEvents       = 64;
    cfg.chordTimeWindow = 0;
    cfg.bleName         = "ESP32-Debug";
    midiHandler.begin(cfg);

    midiHandler.setRawMidiCallback(onRawMidi);

    addLogC(COL_INFO, "Ready. B1=scroll  B2=clear");
    render();
}

// ── Loop ─────────────────────────────────────────────────────────────────────
static uint32_t lastRenderMs = 0;

void loop() {
    midiHandler.task();
    processEvents();

    // Fill from MIDIHandler's authoritative internal map (same as Piano example)
    midiHandler.fillActiveNotes(fillNotes);

    uint32_t nowMs = millis();

    if (nowMs - lastRenderMs >= 100) {
        lastRenderMs = nowMs;
        render();
    }

    // Button 1 — scroll up
    if (digitalRead(PIN_BUTTON_1) == LOW && nowMs - btn1Last > DEBOUNCE_MS) {
        btn1Last = nowMs;
        int vis = (MINI_Y - 20) / 8;
        int maxScroll = max(0, logCount - vis);
        logScroll = min(logScroll + 1, maxScroll);
    }

    // Button 2 — clear everything
    if (digitalRead(PIN_BUTTON_2) == LOW && nowMs - btn2Last > DEBOUNCE_MS) {
        btn2Last = nowMs;
        clearLog();
        memset(manualNotes, 0, sizeof(manualNotes));
        memset(fillNotes, 0, sizeof(fillNotes));
        midiHandler.clearQueue();
        lastEventIdx = -1;
        addLogC(COL_INFO, "Cleared. B1=scroll  B2=clear");
    }
}

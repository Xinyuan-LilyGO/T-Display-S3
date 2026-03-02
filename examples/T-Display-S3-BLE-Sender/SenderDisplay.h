#ifndef SENDER_DISPLAY_H
#define SENDER_DISPLAY_H

// ── Didactic display for BLE MIDI Sender ─────────────────────────────────────
// Shows what the sender is transmitting: note names, MIDI bytes, piano keys.
// Designed for educational demonstrations and photo/video documentation.
//
// Layout (320×170 landscape):
//   ┌──────────────────────────────────────────────┐
//   │ BLE: Connected to "ESP32 BLE Piano"    16px  │
//   ├──────────────────────────────────────────────┤
//   │  Sequence: Pop I-V-vi-IV               20px  │
//   │  Sending: C4  E4  G4                   16px  │
//   │  MIDI: [0x90] [0x3C] [0x64]            12px  │
//   ├──────────────────────────────────────────────┤
//   │  [mini piano keys with highlights]     50px  │
//   ├──────────────────────────────────────────────┤
//   │  Step [3/4]  ████████░░░░░░             8px  │
//   │  B1: Next         B2: Play/Stop        12px  │
//   └──────────────────────────────────────────────┘

#include <LovyanGFX.h>
#include "MusicSequences.h"

// ── Screen ────────────────────────────────────────────────────────────────────
static const int SD_SCREEN_W = 320;
static const int SD_SCREEN_H = 170;

// ── Colours (same palette as PianoDisplay for visual consistency) ─────────────
#define SD_COL_BG         0x1082   // dark grey-blue
#define SD_COL_HEADER     0x2945
#define SD_COL_DIVIDER    0x2945
#define SD_COL_TEXT       0xFFFF   // white
#define SD_COL_DIM        0x8410   // grey
#define SD_COL_ACCENT     0x07FF   // cyan
#define SD_COL_NOTE       0xFFE0   // yellow
#define SD_COL_CHORD      0xFBE0   // orange
#define SD_COL_BLE_ON     0x07FF   // cyan
#define SD_COL_BLE_OFF    0xF800   // red
#define SD_COL_BLE_SCAN   0xFFE0   // yellow
#define SD_COL_PLAYING    0x07E0   // green
#define SD_COL_STOPPED    0x8410   // grey
#define SD_COL_BAR_BG     0x2945
#define SD_COL_BAR_FG     0x07FF   // cyan

// Mini piano
#define SD_COL_KEY_WHITE  0xFFFF
#define SD_COL_KEY_BLACK  0x0841
#define SD_COL_KEY_W_ACT  0x07FF   // cyan
#define SD_COL_KEY_B_ACT  0xFBE0   // orange
#define SD_COL_KEY_BORDER 0x0000

// ── Info passed to display each frame ─────────────────────────────────────────
struct SenderInfo {
    // BLE state
    bool     bleConnected;
    bool     bleScanning;

    // Sequence
    const char* sequenceName;
    int      currentStep;
    int      totalSteps;
    bool     playing;

    // Current notes being sent
    uint8_t  currentNotes[6];
    uint8_t  currentNoteCount;
    uint8_t  currentVelocity;
    uint8_t  currentStatus;    // MIDI status byte (0x90 NoteOn, 0x80 NoteOff, 0 = idle)

    // Active notes (for mini piano)
    const bool* activeNotes;   // pointer to bool[128]
};

// ── SenderDisplay class ───────────────────────────────────────────────────────
class SenderDisplay {
public:
    SenderDisplay();

    void init();
    void render(const SenderInfo& info);

private:
    class LGFX : public lgfx::LGFX_Device {
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
    private:
        lgfx::Bus_Parallel8 _bus;
        lgfx::Panel_ST7789  _panel;
        lgfx::Light_PWM     _bl;
    };

    void _drawStatusBar(const SenderInfo& info);
    void _drawSequenceInfo(const SenderInfo& info);
    void _drawMidiBytes(const SenderInfo& info);
    void _drawMiniPiano(const bool notes[128]);
    void _drawProgressBar(const SenderInfo& info);
    void _drawControls(const SenderInfo& info);

    LGFX        _tft;
    LGFX_Sprite _screen;
};

extern SenderDisplay senderDisplay;

#endif // SENDER_DISPLAY_H

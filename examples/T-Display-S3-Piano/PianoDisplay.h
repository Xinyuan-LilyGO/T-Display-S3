#ifndef PIANO_DISPLAY_H
#define PIANO_DISPLAY_H

// ── Piano Visualizer for MIDI-25 keyboards (e.g. Arturia Minilab 25) ─────────
// Display: ST7789 170×320 landscape (320×170 after rotation) via LovyanGFX
// Shows 25 keys (C to C, 2 octaves + 1), matching a standard 25-key MIDI keyboard.
//
// Anti-tearing: full-screen sprite (320×170, ~108 KB in PSRAM).
// ALL drawing happens in RAM; a single pushSprite() transfers the entire
// frame to the display, eliminating the diagonal tearing caused by the
// ST7789 scan line crossing multiple separate write transactions.
//
// Auto-view: automatically adjusts the visible octave range when the
// keyboard sends notes outside the current view.

#include <LovyanGFX.h>
#include <string>
#include <cstring>

// ── Screen (landscape) ────────────────────────────────────────────────────────
static const int SCREEN_W  = 320;
static const int SCREEN_H  = 170;

// ── Info bar ──────────────────────────────────────────────────────────────────
static const int INFO_H    = 48;
static const int PIANO_Y   = INFO_H;
static const int PIANO_H   = SCREEN_H - INFO_H;   // 122 px

// ── Piano key geometry — 25 keys (C to C inclusive = 15 white keys) ───────────
static const int KEYS_SPAN    = 25;
static const int WHITE_KEYS   = 15;
static const int WHITE_KEY_W  = 21;   // 15 × 21 = 315 px → fits 320 px
static const int WHITE_KEY_H  = PIANO_H;
static const int BLACK_KEY_W  = 12;
static const int BLACK_KEY_H  = int(PIANO_H * 0.60f);   // ≈ 73 px
static const int PIANO_X      = (SCREEN_W - WHITE_KEYS * WHITE_KEY_W) / 2;

// ── MIDI view range ───────────────────────────────────────────────────────────
static const int VIEW_DEFAULT  = 48;   // C3  (typical Minilab 25 range)
static const int VIEW_MIN      = 0;    // C-1 (allow full range for auto-view)
static const int VIEW_MAX      = 103;  // G7  (103+24 = 127, last valid MIDI)

// ── Colours — inspired by Gingoduino TDisplayS3Explorer palette ──────────────
// Piano keys
#define COL_WHITE_NORMAL  0xFFFF
#define COL_WHITE_ACTIVE  0x07FF   // cyan (accent)
#define COL_BLACK_NORMAL  0x0841   // near-black
#define COL_BLACK_ACTIVE  0xFBE0   // warm orange
#define COL_KEY_BORDER    0x0000

// Info bar (Explorer palette)
#define COL_INFO_BG       0x1082   // dark grey-blue (C_BG)
#define COL_HEADER_BG     0x2945   // slightly lighter (C_HEADER)
#define COL_DIVIDER       0x2945   // separator
#define COL_DIM           0x8410   // grey (labels, hints)
#define COL_TEXT          0xFFFF   // white
#define COL_ACCENT        0x07FF   // cyan (C_ACCENT)
#define COL_NOTE          0xFFE0   // yellow (C_NOTE)
#define COL_CHORD         0xFBE0   // orange (C_CHORD)
#define COL_INTERVAL      0x07FF   // cyan (C_INTERVAL)
#define COL_FREQ          0xFFE0   // yellow
#define COL_WAITING       0x8410   // dim grey

// ── Music analysis data ──────────────────────────────────────────────────────
enum PianoLang { LANG_EN = 0, LANG_PT = 1 };

struct PianoInfo {
    int      noteCount;           // 0 = silence
    int      rootMidi;            // MIDI note number of lowest active note
    char     noteStr[64];         // "C4  E4  G4"
    char     chordName[16];       // "CM", "Am7", "" if < 3 notes
    char     chordFullName[40];   // "C Major", "A Minor 7"
    char     intervalLabel[8];    // "3M"  (only if noteCount == 2)
    char     intervalName[40];    // "Major Third"
    char     formula[64];         // "P1  3M  5J" (if 3+ notes)
    float    rootFreq;            // Hz of lowest active note (0 if none)
};

// ── Semitone lookup tables ────────────────────────────────────────────────────
static const int  SEMITONE_TO_WHITE[12]    = {0,-1,1,-1,2,3,-1,4,-1,5,-1,6};
static const int  BLACK_LEFT_NEIGHBOR[12]  = {-1,0,-1,2,-1,-1,5,-1,7,-1,9,-1};
static const char* const NOTE_NAMES[12]    = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

// ── PianoDisplay class ────────────────────────────────────────────────────────
class PianoDisplay {
public:
    PianoDisplay();

    void init();
    void render(const bool activeNotes[128], const PianoInfo& info);
    void shiftOctave(int delta);
    void setViewStart(int midi);
    int  getViewStart() const { return _viewStart; }

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

    void _drawPiano(const bool activeNotes[128]);
    void _drawInfoBar(const bool activeNotes[128], const PianoInfo& info);
    void _drawBadge(int x, int y, int w, int h, uint16_t bg, const char* text);

    LGFX         _tft;
    LGFX_Sprite  _screen;
    int          _viewStart;
};

extern PianoDisplay piano;

#endif // PIANO_DISPLAY_H

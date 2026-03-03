#ifndef ST7789_HANDLER_H
#define ST7789_HANDLER_H

#include <LovyanGFX.h>
#include <cstdint>

// Color palette — blue/teal theme for USB
#define USB_COL_BG        0x0000  // Black background
#define USB_COL_HEADER    0x19F4  // Dark blue  — header / counter bar
#define USB_COL_DIVIDER   0x4208  // Dark gray  — divider line / label
#define USB_COL_WHITE     0xFFFF  // White text
#define USB_COL_GREEN     0x07E0  // Green  — connected
#define USB_COL_ORANGE    0xFD20  // Orange — waiting / scanning
#define USB_COL_RED       0xF800  // Red    — disconnected
#define USB_COL_CYAN      0x07FF  // Cyan   — NoteOn
#define USB_COL_GRAY      0x4208  // Gray   — NoteOff
#define USB_COL_YELLOW    0xFFE0  // Yellow — ControlChange
#define USB_COL_MAGENTA   0xF81F  // Magenta — PitchBend
#define USB_COL_LIME      0x07E0  // Green  — ProgramChange

// Layout — portrait 170 × 320
#define USB_Y_HEADER    0
#define USB_H_HEADER   34
#define USB_Y_USB      34
#define USB_H_USB      22
#define USB_Y_BLE      56
#define USB_H_BLE      22
#define USB_Y_DIVIDER  78
#define USB_H_DIVIDER  14
#define USB_Y_EVENTS   92
#define USB_H_EVENT    20
#define USB_N_EVENTS   10
#define USB_Y_COUNTERS (USB_Y_EVENTS + USB_N_EVENTS * USB_H_EVENT)  // = 292
#define USB_H_COUNTERS 28          // 292 + 28 = 320

#define USB_DISPLAY_W  170
#define USB_DISPLAY_H  320
#define USB_MARGIN      4

class ST7789_Handler {
public:
    ST7789_Handler();
    void init();

    // Updates the USB status row.
    // connected = true  → green dot "USB  Connected"
    // connected = false → orange dot "USB  Waiting for host..."
    void setUSB(bool connected);

    // Updates the BLE status row.
    // connected = true  → green dot "BLE  Connected"
    // connected = false → orange dot "BLE  Scanning..."
    void setBLE(bool connected);

    // Pushes a new event to the top of the scrolling list.
    void pushEvent(uint32_t color, const char* line);

    // Updates the IN / OUT counters in the bottom bar.
    void setCounters(int in, int out);

private:
    class LGFX : public lgfx::LGFX_Device {
    public:
        LGFX(void) {
            {
                auto cfg = _bus_instance.config();
                cfg.pin_wr = 8;  cfg.pin_rd = 9;  cfg.pin_rs = 7;
                cfg.pin_d0 = 39; cfg.pin_d1 = 40; cfg.pin_d2 = 41; cfg.pin_d3 = 42;
                cfg.pin_d4 = 45; cfg.pin_d5 = 46; cfg.pin_d6 = 47; cfg.pin_d7 = 48;
                _bus_instance.config(cfg);
                _panel_instance.setBus(&_bus_instance);
            }
            {
                auto cfg = _panel_instance.config();
                cfg.pin_cs  = 6;  cfg.pin_rst = 5;  cfg.pin_busy = -1;
                cfg.offset_rotation = 1;
                cfg.offset_x = 35;
                cfg.readable = false;  cfg.invert = true;
                cfg.rgb_order = false; cfg.dlen_16bit = false; cfg.bus_shared = false;
                cfg.panel_width  = USB_DISPLAY_W;
                cfg.panel_height = USB_DISPLAY_H;
                _panel_instance.config(cfg);
            }
            setPanel(&_panel_instance);
            {
                auto cfg = _light_instance.config();
                cfg.pin_bl = 38; cfg.invert = false;
                cfg.freq = 22000; cfg.pwm_channel = 7;
                _light_instance.config(cfg);
                _panel_instance.setLight(&_light_instance);
            }
        }
    private:
        lgfx::Bus_Parallel8  _bus_instance;
        lgfx::Panel_ST7789   _panel_instance;
        lgfx::Light_PWM      _light_instance;
    };

    LGFX _tft;

    struct EventEntry { uint32_t color; char line[32]; };
    EventEntry _events[USB_N_EVENTS];
    int        _eventCount;

    void _fillRow(int y, int h, uint32_t bg);
    void _drawText(int x, int y, uint32_t color, uint32_t bg, uint8_t size, const char* text);
    void _drawStatusRow(int y, int h, uint32_t dotColor, const char* label);
    void _redrawEvents();
};

extern ST7789_Handler display;

#endif // ST7789_HANDLER_H

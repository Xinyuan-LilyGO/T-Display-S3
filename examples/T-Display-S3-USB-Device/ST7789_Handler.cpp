#include "ST7789_Handler.h"
#include "mapping.h"
#include <cstdio>
#include <cstring>

ST7789_Handler display;

ST7789_Handler::ST7789_Handler() : _eventCount(0) {
    memset(_events, 0, sizeof(_events));
}

void ST7789_Handler::_fillRow(int y, int h, uint32_t bg) {
    _tft.fillRect(0, y, USB_DISPLAY_W, h, bg);
}

void ST7789_Handler::_drawText(int x, int y, uint32_t color, uint32_t bg,
                               uint8_t size, const char* text) {
    _tft.setTextSize(size);
    _tft.setTextColor(color, bg);
    _tft.drawString(text, x, y);
}

// Draws a status row: colored dot + label text.
void ST7789_Handler::_drawStatusRow(int y, int h, uint32_t dotColor, const char* label) {
    _fillRow(y, h, USB_COL_BG);
    _tft.fillCircle(USB_MARGIN + 4, y + h / 2, 4, dotColor);
    _drawText(USB_MARGIN + 14, y + (h - 8) / 2, USB_COL_WHITE, USB_COL_BG, 1, label);
}

void ST7789_Handler::init() {
    _tft.init();
    _tft.setRotation(2);
    _tft.fillScreen(USB_COL_BG);

    // Backlight guarantee for battery operation
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    // Header
    _fillRow(USB_Y_HEADER, USB_H_HEADER, USB_COL_HEADER);
    const char* title = "USB  MIDI";
    int titleW = strlen(title) * 12;
    int titleX = (USB_DISPLAY_W - titleW) / 2;
    int titleY = USB_Y_HEADER + (USB_H_HEADER - 16) / 2;
    _drawText(titleX, titleY, USB_COL_WHITE, USB_COL_HEADER, 2, title);

    // Status rows (initial state = waiting/scanning)
    setUSB(false);
    setBLE(false);

    // Divider
    _fillRow(USB_Y_DIVIDER, USB_H_DIVIDER, USB_COL_BG);
    int lineY = USB_Y_DIVIDER + USB_H_DIVIDER / 2;
    _tft.drawFastHLine(0, lineY, USB_DISPLAY_W, USB_COL_DIVIDER);
    const char* lbl = " EVENTS ";
    int lblW = strlen(lbl) * 6;
    _drawText((USB_DISPLAY_W - lblW) / 2, USB_Y_DIVIDER + 2,
              USB_COL_DIVIDER, USB_COL_BG, 1, lbl);

    // Events area — blank
    _fillRow(USB_Y_EVENTS, USB_N_EVENTS * USB_H_EVENT, USB_COL_BG);

    // Counters bar
    _fillRow(USB_Y_COUNTERS, USB_H_COUNTERS, USB_COL_HEADER);
    setCounters(0, 0);
}

void ST7789_Handler::setUSB(bool connected) {
    if (connected) {
        _drawStatusRow(USB_Y_USB, USB_H_USB, USB_COL_GREEN,
                       "USB  Connected");
    } else {
        _drawStatusRow(USB_Y_USB, USB_H_USB, USB_COL_ORANGE,
                       "USB  Waiting for host...");
    }
}

void ST7789_Handler::setBLE(bool connected) {
    if (connected) {
        _drawStatusRow(USB_Y_BLE, USB_H_BLE, USB_COL_GREEN,
                       "BLE  Connected");
    } else {
        _drawStatusRow(USB_Y_BLE, USB_H_BLE, USB_COL_ORANGE,
                       "BLE  Scanning...");
    }
}

void ST7789_Handler::pushEvent(uint32_t color, const char* line) {
    for (int i = USB_N_EVENTS - 1; i > 0; i--) _events[i] = _events[i - 1];
    _events[0].color = color;
    strncpy(_events[0].line, line, sizeof(_events[0].line) - 1);
    _events[0].line[sizeof(_events[0].line) - 1] = '\0';
    if (_eventCount < USB_N_EVENTS) _eventCount++;
    _redrawEvents();
}

void ST7789_Handler::_redrawEvents() {
    for (int i = 0; i < USB_N_EVENTS; i++) {
        int y = USB_Y_EVENTS + i * USB_H_EVENT;
        _fillRow(y, USB_H_EVENT, USB_COL_BG);
        if (i < _eventCount) {
            _drawText(USB_MARGIN, y + 6, _events[i].color, USB_COL_BG, 1, _events[i].line);
        }
    }
}

void ST7789_Handler::setCounters(int in, int out) {
    _fillRow(USB_Y_COUNTERS, USB_H_COUNTERS, USB_COL_HEADER);
    char buf[20];
    snprintf(buf, sizeof(buf), "IN : %d", in);
    _drawText(USB_MARGIN, USB_Y_COUNTERS + 8, USB_COL_WHITE, USB_COL_HEADER, 1, buf);
    snprintf(buf, sizeof(buf), "OUT: %d", out);
    _drawText(USB_DISPLAY_W / 2 + 4, USB_Y_COUNTERS + 8, USB_COL_WHITE, USB_COL_HEADER, 1, buf);
}

// Arduino GFX display driver for SSD1306

#ifndef _Arduino_SSD1306_H_
#define _Arduino_SSD1306_H_

#include "../Arduino_GFX.h"
#include "../databus/Arduino_Wire.h"

// SSD1306 commands, See Datasheet
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_SEGREMAPINV 0xA1
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3

class Arduino_SSD1306 : public Arduino_G
{
public:
  Arduino_SSD1306(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, int16_t w = 128, int16_t h = 64);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;

  void invertDisplay(bool);
  void displayOn();
  void displayOff();
  void setBrightness(uint8_t brightness);

protected:
  Arduino_DataBus *_bus;
  int8_t _rst;
  int8_t _pages;
  uint8_t _rotation;

  uint8_t _contrast = 0x8F;
  uint8_t _colStart;
  uint8_t _colEnd;

private:
};

#endif // _Arduino_SSD1306_H_

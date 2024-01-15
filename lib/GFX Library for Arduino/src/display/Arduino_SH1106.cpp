// Arduino GFX display driver for SH1106

#include "Arduino_SH1106.h"

Arduino_SH1106::Arduino_SH1106(Arduino_DataBus *bus, int8_t rst, int16_t w, int16_t h)
    : Arduino_G(w, h), _bus(bus), _rst(rst)
{
  _rotation = 0;
  _pages = (h + 7) / 8;
}

bool Arduino_SH1106::begin(int32_t speed)
{
  // println("SH1106::begin()");

  if (!_bus)
  {
    // println("SH1106::bus not given");
  }
  else if (!_bus->begin(speed))
  {
    // println("SH1106::bus not started");
    return false;
  }

  // println("SH1106::Initialize Display");

  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(20);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(20);
  }

  static const uint8_t init_sequence[] = {
      BEGIN_WRITE,
      WRITE_BYTES, 26,
      0x00, // sequence of commands, Co = 0, D/C = 0
      SH110X_DISPLAYOFF,
      SH110X_SETDISPLAYCLOCKDIV, 0x80, // 0xD5, 0x80,
      SH110X_SETMULTIPLEX, 0x3F,       // 0xA8, 0x3F,
      SH110X_SETDISPLAYOFFSET, 0x00,   // 0xD3, 0x00,
      SH110X_SETSTARTLINE,             // 0x40
      SH110X_DCDC, 0x8B,               // DC/DC on
      SH110X_SEGREMAP + 1,             // 0xA1
      SH110X_COMSCANDEC,               // 0xC8
      SH110X_SETCOMPINS, 0x12,         // 0xDA, 0x12,
      SH110X_SETCONTRAST, 0xFF,        // 0x81, 0xFF
      SH110X_SETPRECHARGE, 0x1F,       // 0xD9, 0x1F,
      SH110X_SETVCOMDETECT, 0x40,      // 0xDB, 0x40,
      0x33,                            // Set VPP to 9V
      SH110X_NORMALDISPLAY,
      SH110X_MEMORYMODE, 0x10, // 0x20, 0x00
      SH110X_DISPLAYALLON_RESUME,
      END_WRITE,

      DELAY, 100,

      BEGIN_WRITE,
      WRITE_BYTES, 2,
      0x00, // Co = 0, D/C = 0
      SH110X_DISPLAYON,
      END_WRITE};

  _bus->batchOperation(init_sequence, sizeof(init_sequence));

  return true;
}

void Arduino_SH1106::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t, uint16_t)
{
  // printf("SH1106::drawBitmap %d/%d w:%d h:%d\n", xStart, yStart, w, h);
  uint16_t bufferLength = TWI_BUFFER_LENGTH;

  // transfer the whole bitmap
  for (uint8_t p = y / 8; p < h / 8; p++)
  {
    uint8_t *pptr = bitmap + (p * w); // page start pointer
    uint16_t bytesOut = 0;

    // start page sequence
    _bus->beginWrite();
    _bus->write(0x00);
    _bus->write(SH110X_SETPAGEADDR + p);
    _bus->write(SH110X_SETLOWCOLUMN + 2); // set column
    _bus->write(SH110X_SETHIGHCOLUMN + 0);
    _bus->endWrite();

    // send out page data
    for (int i = x; i < w; i++)
    {
      if (!bytesOut)
      {
        _bus->beginWrite();
        _bus->write(SH110X_SETSTARTLINE + 0);
        bytesOut = 1;
      }

      _bus->write(*pptr++);
      bytesOut++;

      if (bytesOut == bufferLength)
      {
        _bus->endWrite();
        bytesOut = 0;
      }
    }
    if (bytesOut)
    {
      _bus->endWrite();
    }
  }
} // drawBitmap()

void Arduino_SH1106::drawIndexedBitmap(int16_t, int16_t, uint8_t *, uint16_t *, int16_t, int16_t, int16_t)
{
  // println("SH1106::Not Implemented drawIndexedBitmap()");
}

void Arduino_SH1106::draw3bitRGBBitmap(int16_t, int16_t, uint8_t *, int16_t, int16_t)
{
  // println("SH1106::Not Implemented draw3bitRGBBitmap()");
}

void Arduino_SH1106::draw16bitRGBBitmap(int16_t, int16_t, uint16_t *, int16_t, int16_t)
{
  // println("SH1106::Not Implemented draw16bitRGBBitmap()");
}

void Arduino_SH1106::draw24bitRGBBitmap(int16_t, int16_t, uint8_t *, int16_t, int16_t)
{
  // println("SH1106::Not Implemented draw24bitRGBBitmap()");
}

void Arduino_SH1106::invertDisplay(bool)
{
  // _bus->sendCommand(i ? ILI9488_INVON : ILI9488_INVOFF);
}

void Arduino_SH1106::displayOn(void)
{
  uint8_t seq[] = {
      BEGIN_WRITE,
      WRITE_BYTES, 2,
      0x00, // sequence of commands
      SH110X_DISPLAYON,
      END_WRITE};
  _bus->batchOperation(seq, sizeof(seq));
}

void Arduino_SH1106::displayOff(void)
{
  uint8_t seq[] = {
      BEGIN_WRITE,
      WRITE_BYTES, 2,
      0x00, // sequence of commands
      SH110X_DISPLAYOFF,
      END_WRITE};
  _bus->batchOperation(seq, sizeof(seq));
}

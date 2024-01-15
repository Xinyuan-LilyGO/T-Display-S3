/*
 * start rewrite from:
 * https://github.com/modi12jin/Arduino-ESP32-WEA2012.git
 */
#include "Arduino_WEA2012.h"
#include "SPI.h"

Arduino_WEA2012::Arduino_WEA2012(Arduino_DataBus *bus, int8_t rst)
    : Arduino_TFT(bus, rst, 0, false, WEA2012_TFTWIDTH, WEA2012_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_WEA2012::begin(int32_t speed)
{
  _override_datamode = SPI_MODE3; // always use SPI_MODE3

  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_WEA2012::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  // not implemented
}

void Arduino_WEA2012::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(WEA2012_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(WEA2012_PASET, y, y + h - 1);
  }

  _bus->writeCommand(WEA2012_RAMWR); // write to RAM
}

void Arduino_WEA2012::invertDisplay(bool i)
{
  _bus->sendCommand(i ? WEA2012_INVON : WEA2012_INVOFF);
}

void Arduino_WEA2012::displayOn(void)
{
  _bus->sendCommand(WEA2012_SLPOUT);
  delay(WEA2012_SLPOUT_DELAY);
}

void Arduino_WEA2012::displayOff(void)
{
  _bus->sendCommand(WEA2012_SLPIN);
  delay(WEA2012_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_WEA2012::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(WEA2012_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(WEA2012_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(WEA2012_SWRESET);
    delay(WEA2012_RST_DELAY);
  }

  _bus->batchOperation(wea2012_init_operations, sizeof(wea2012_init_operations));

  invertDisplay(false);
}

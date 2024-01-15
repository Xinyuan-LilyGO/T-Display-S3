// Databus Adapter for the Arduino Wire bus (I2C bus)

#include "Arduino_Wire.h"

Arduino_Wire::
    Arduino_Wire(uint8_t i2c_addr, TwoWire *wire)
    : _i2c_addr(i2c_addr), _wire(wire) {}

bool Arduino_Wire::begin(int32_t speed, int8_t)
{

  if (speed != GFX_NOT_DEFINED)
  {
    _speed = speed;
    _wire->setClock(_speed);
  }

  _wire->begin();

  // find device
  _wire->beginTransmission(_i2c_addr);
  if (_wire->endTransmission())
  {
    // println("Wire::Device not found.");
    is_found = false;
  }
  else
  {
    is_found = true;
  }

  return is_found;
}

void Arduino_Wire::beginWrite()
{
  // println("Wire::beginWrite()");
  if (_speed != GFX_NOT_DEFINED)
  {
    _wire->setClock(_speed);
  }
  _wire->beginTransmission(_i2c_addr);
}

void Arduino_Wire::endWrite()
{
  // println("\nWire::endWrite()");
  _wire->endTransmission();
}

void Arduino_Wire::write(uint8_t d)
{
  // printf("(0x%02x) ", d);
  _wire->write(d);
}

void Arduino_Wire::writeCommand(uint8_t)
{
  // println("Wire::writeCommand()");
  // not implemented.
}

void Arduino_Wire::writeCommand16(uint16_t)
{
  // println("Wire::writeCommand16()");
  // not implemented.
}

void Arduino_Wire::write16(uint16_t)
{
  // println("Wire::write16()");
  // not implemented
}

void Arduino_Wire::writeRepeat(uint16_t, uint32_t)
{
  // println("Wire::writeRepeat()");
  // not implemented
}

void Arduino_Wire::writePixels(uint16_t *, uint32_t)
{
  // println("Wire::writePixels()");
  // not implemented
}

#if !defined(LITTLE_FOOT_PRINT)
void Arduino_Wire::writeBytes(uint8_t *, uint32_t)
{
  // println("Wire::writeBytes()");
  // not implemented
}
#endif // !defined(LITTLE_FOOT_PRINT)

void Arduino_Wire::writeRegister(uint8_t, uint8_t *, size_t)
{
  // println("Wire::writeRegister()");
}

uint8_t Arduino_Wire::readRegister(uint8_t, uint8_t *, size_t)
{
  // println("Wire::readRegister()");
  return 0;
}

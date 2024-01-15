#if defined(ARDUINO_ARCH_SAM)

#include "Arduino_DUEPAR16.h"

Arduino_DUEPAR16::Arduino_DUEPAR16()
{
}

bool Arduino_DUEPAR16::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);
  _cs   = 40;
  _dc   = 38;
  _wr   = 39;
  _rd   = 43;
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, HIGH);
  digitalWrite(_wr, HIGH);
  digitalWrite(_rd, HIGH);
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_wr, OUTPUT);
  pinMode(_rd, OUTPUT);
  setDataPins(OUTPUT);
  return true;
}

void Arduino_DUEPAR16::setDataPins(uint8_t mode)
{
  pinMode(29, mode);
  pinMode(28, mode);
  pinMode(27, mode);
  pinMode(26, mode);
  pinMode(25, mode);
  pinMode(24, mode);
  pinMode(23, mode);
  pinMode(22, mode);
  pinMode(30, mode);
  pinMode(31, mode);
  pinMode(32, mode);
  pinMode(33, mode);
  pinMode(34, mode);
  pinMode(35, mode);
  pinMode(36, mode);
  pinMode(37, mode);
}

void Arduino_DUEPAR16::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_DUEPAR16::endWrite()
{
  CS_HIGH();
}

void Arduino_DUEPAR16::writeCommand(uint8_t c)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
}

void Arduino_DUEPAR16::writeCommand16(uint16_t c)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
}

void Arduino_DUEPAR16::write(uint8_t d)
{
  WRITE16(d);
}

void Arduino_DUEPAR16::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_DUEPAR16::writeRepeat(uint16_t p, uint32_t len)
{
  writeData16(p, len);
}

void Arduino_DUEPAR16::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE16(*data++);
  }
}

#if !defined(LITTLE_FOOT_PRINT)

void Arduino_DUEPAR16::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
  WRITE16(d);
}

void Arduino_DUEPAR16::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
  WRITE16(d);
}

void Arduino_DUEPAR16::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_DUEPAR16::writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();
  WRITE16(c);
  DC_HIGH();
  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_DUEPAR16::writeBytes(uint8_t *data, uint32_t len)
{
  while (len > 1)
  {
    _data16.msb = *data++;
    _data16.lsb = *data++;
    WRITE16(_data16.value);
    len -= 2;
  }
  if (len)
  {
    WRITE16(*data);
  }
}

void Arduino_DUEPAR16::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    WRITE16(idx[*data++]);
  }
}

void Arduino_DUEPAR16::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE16(_data16.value);
    WRITE16(_data16.value);
  }
}

#endif // !defined(LITTLE_FOOT_PRINT)

INLINE void Arduino_DUEPAR16::WRITE16(uint16_t d)
{
  writeData16(d, 1);
}

void Arduino_DUEPAR16::writeData16(uint16_t d, uint32_t num)
{
  //                |       |       |       |         Ruler for byte MS bits 31, 23, 15 and 7
  //                     B          AA   DD AD  DDDD  Marker for register bits used
  REG_PIOA_CODR = 0b00000000000000001100000010000000; // Clear bits
  REG_PIOB_CODR = 0b00000100000000000000000000000000; // Clear bits
  //                                        W CCCCC   // WR bit
  REG_PIOC_CODR = 0b00000000000000000000000010111110; // Clear WR bit as well
  REG_PIOD_CODR = 0b00000000000000000000011001001111; // Clear bits

  // The compiler efficiently codes this
  // so it is quite quick.                    Port.bit
  if (d & 0x8000) REG_PIOD_SODR = 0x1 << 6; // D.6
  if (d & 0x4000) REG_PIOD_SODR = 0x1 << 3; // D.3
  if (d & 0x2000) REG_PIOD_SODR = 0x1 << 2; // D.2
  if (d & 0x1000) REG_PIOD_SODR = 0x1 << 1; // D.1
  if (d & 0x0800) REG_PIOD_SODR = 0x1 << 0; // D.0
  if (d & 0x0400) REG_PIOA_SODR = 0x1 << 15; // A.15
  if (d & 0x0200) REG_PIOA_SODR = 0x1 << 14; // A.14
  if (d & 0x0100) REG_PIOB_SODR = 0x1 << 26; // B.26

  // so it is quite quick.                    Port.bit
  if (d & 0x0080) REG_PIOD_SODR = 0x1 << 9; // D.9
  if (d & 0x0040) REG_PIOA_SODR = 0x1 << 7; // A.7
  if (d & 0x0020) REG_PIOD_SODR = 0x1 << 10; // D.10
  if (d & 0x0010) REG_PIOC_SODR = 0x1 << 1; // C.1
  if (d & 0x0008) REG_PIOC_SODR = 0x1 << 2; // C.2
  if (d & 0x0004) REG_PIOC_SODR = 0x1 << 3; // C.3
  if (d & 0x0002) REG_PIOC_SODR = 0x1 << 4; // C.4
  if (d & 0x0001) REG_PIOC_SODR = 0x1 << 5; // C.5
  while (num > 0)
  {
    // WR_STB;
    REG_PIOC_CODR = 0x1 << 7;  
    REG_PIOC_CODR = 0x1 << 7;  
    REG_PIOC_CODR = 0x1 << 7;  
    REG_PIOC_SODR = 0x1 << 7;
    num--;
  }
}

/******** low level bit twiddling **********/

INLINE void Arduino_DUEPAR16::DC_HIGH(void)
{
  digitalWrite(_dc, HIGH);
}

INLINE void Arduino_DUEPAR16::DC_LOW(void)
{
  digitalWrite(_dc, LOW);
}

INLINE void Arduino_DUEPAR16::CS_HIGH(void)
{
  digitalWrite(_cs, HIGH);
}

INLINE void Arduino_DUEPAR16::CS_LOW(void)
{
  digitalWrite(_cs, LOW);
}

#endif // #if defined(ARDUINO_ARCH_SAM)

#include "Arduino_UNOPAR8.h"

// for MCUFriend UNO kind of shields. -jz-
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)

Arduino_UNOPAR8::Arduino_UNOPAR8()
{
}

bool Arduino_UNOPAR8::begin(int32_t, int8_t)
{
  pinMode(A2, OUTPUT);    // LCD DC
  digitalWrite(A2, HIGH); // Data mode

  pinMode(A3, OUTPUT);    // LCD CS
  digitalWrite(A3, HIGH); // Disable chip select

  pinMode(A1, OUTPUT);    // LCD WR
  digitalWrite(A1, HIGH); // Set write strobe high (inactive)

  pinMode(A0, OUTPUT);    // LCD RD
  digitalWrite(A0, HIGH); // Disable RD

  pinMode(8, OUTPUT); // LCD D0
  pinMode(9, OUTPUT); // LCD D1
  pinMode(2, OUTPUT); // LCD D2
  pinMode(3, OUTPUT); // LCD D3
  pinMode(4, OUTPUT); // LCD D4
  pinMode(5, OUTPUT); // LCD D5
  pinMode(6, OUTPUT); // LCD D6
  pinMode(7, OUTPUT); // LCD D7

  return true;
}

void Arduino_UNOPAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_UNOPAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_UNOPAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_UNOPAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_UNOPAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_UNOPAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_UNOPAR8::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
#if defined(ARDUINO_AVR_UNO)
    WRITE(_data16.msb);
    // WR_STB
    PORTC &= ~_BV(1);
    PORTC |= _BV(1);
    while (--len)
    {
      // WR_STB
      PORTC &= ~_BV(1);
      PORTC |= _BV(1);
      // WR_STB
      PORTC &= ~_BV(1);
      PORTC |= _BV(1);
    }
#elif defined(ARDUINO_AVR_MEGA2560)
    WRITE(_data16.msb);
    // WR_STB
    PORTF &= ~_BV(1);
    PORTF |= _BV(1);
    while (--len)
    {
      // WR_STB
      PORTF &= ~_BV(1);
      PORTF |= _BV(1);
      // WR_STB
      PORTF &= ~_BV(1);
      PORTF |= _BV(1);
    }
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    WRITE(_data16.msb);
    // WR_STB
    R_PORT0->PORR = bit(0);
    R_PORT0->POSR = bit(0);
    while (--len)
    {
      // WR_STB
      R_PORT0->PORR = bit(0);
      R_PORT0->POSR = bit(0);
      // WR_STB
      R_PORT0->PORR = bit(0);
      R_PORT0->POSR = bit(0);
    }
#endif
  }
  else
  {
    while (len--)
    {
      WRITE(_data16.msb);
      WRITE(_data16.lsb);
    }
  }
}

void Arduino_UNOPAR8::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}

void Arduino_UNOPAR8::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

INLINE void Arduino_UNOPAR8::WRITE(uint8_t d)
{
#if defined(ARDUINO_AVR_UNO)
  PORTB = (PORTB & 0xFC) | (d & 0x03); // LCD D0-1
  PORTD = (PORTD & 0x03) | (d & 0xFC); // LCD D2-7
  // WR_STB
  PORTC &= ~_BV(1);
  PORTC |= _BV(1);
#elif defined(ARDUINO_AVR_MEGA2560)
  PORTH = (PORTH & 0x87) | ((d & 0x03) << 5) | ((d & 0xC0) >> 3); // LCD D0-1, D6-7
  PORTE = (PORTE & 0xC7) | ((d & 0x0C) << 2) | ((d & 0x20) >> 2); // LCD D2-3, D5
  (d & 0x10) ? PORTG |= _BV(5) : PORTG &= ~_BV(5);                // LCD D4
  // WR_STB
  PORTF &= ~_BV(1);
  PORTF |= _BV(1);
#elif defined(ARDUINO_UNOR4_MINIMA)
  R_PORT3->PORR = 0x18;
  R_PORT3->POSR = (d & 0x01) << 4; // LCD D0
  R_PORT3->POSR = (d & 0x02) << 2; // LCD D1
  R_PORT1->PORR = 0x00FC;
  R_PORT1->POSR = _xset_mask[d >> 2]; // LCD D2-7
  // WR_STB
  R_PORT0->PORR = bit(0);
  R_PORT0->POSR = bit(0);
#elif defined(ARDUINO_UNOR4_WIFI)
  R_PORT3->PORR = 0x18;
  R_PORT3->POSR = (d & 0x01) << 4; // LCD D0
  R_PORT3->POSR = (d & 0x02) << 2; // LCD D1
  R_PORT1->PORR = 0x18F0;
  R_PORT1->POSR = ((d & 0x3C) << 2) | ((d & 0xC0) << 5); // LCD D2-7
  // WR_STB
  R_PORT0->PORR = bit(0);
  R_PORT0->POSR = bit(0);
#endif
}

/******** low level bit twiddling **********/

INLINE void Arduino_UNOPAR8::DC_HIGH(void)
{
#if defined(ARDUINO_AVR_UNO)
  PORTC |= _BV(2); // RS_H;
#elif defined(ARDUINO_AVR_MEGA2560)
  PORTF |= _BV(2);  // RS_H;
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  R_PORT0->POSR = bit(1);
#endif
}

INLINE void Arduino_UNOPAR8::DC_LOW(void)
{
#if defined(ARDUINO_AVR_UNO)
  PORTC &= ~_BV(2); // RS_L;
#elif defined(ARDUINO_AVR_MEGA2560)
  PORTF &= ~_BV(2); // RS_L;
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  R_PORT0->PORR = bit(1);
#endif
}

INLINE void Arduino_UNOPAR8::CS_HIGH(void)
{
#if defined(ARDUINO_AVR_UNO)
  PORTC |= _BV(3); // CS_H;
#elif defined(ARDUINO_AVR_MEGA2560)
  PORTF |= _BV(3);  // CS_H;
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  R_PORT0->POSR = bit(2);
#endif
}

INLINE void Arduino_UNOPAR8::CS_LOW(void)
{
#if defined(ARDUINO_AVR_UNO)
  PORTC &= ~_BV(3); // CS_L;
#elif defined(ARDUINO_AVR_MEGA2560)
  PORTF &= ~_BV(3); // CS_L;
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  R_PORT0->PORR = bit(2);
#endif
}
#endif // #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)

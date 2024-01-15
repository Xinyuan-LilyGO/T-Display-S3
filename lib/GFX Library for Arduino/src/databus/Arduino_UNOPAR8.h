#pragma once

#include "Arduino_DataBus.h"

// for MCUFriend UNO kind of shields. -jz-
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)

#if defined(ARDUINO_UNOR4_MINIMA)
static const uint16_t _xset_mask[] = {
    // 0x0     0x1     0x2     0x3     0x4     0x5     0x6     0x7
    0x0000, 0x0020, 0x0010, 0x0030, 0x0008, 0x0028, 0x0018, 0x0038,
    0x0004, 0x0024, 0x0014, 0x0034, 0x000C, 0x002C, 0x001C, 0x003C,
    0x0040, 0x0060, 0x0050, 0x0070, 0x0048, 0x0068, 0x0058, 0x0078,
    0x0044, 0x0064, 0x0054, 0x0074, 0x004C, 0x006C, 0x005C, 0x007C,
    0x0080, 0x00A0, 0x0090, 0x00B0, 0x0088, 0x00A8, 0x0098, 0x00B8,
    0x0084, 0x00A4, 0x0094, 0x00B4, 0x008C, 0x00AC, 0x009C, 0x00BC,
    0x00C0, 0x00E0, 0x00D0, 0x00F0, 0x00C8, 0x00E8, 0x00D8, 0x00F8,
    0x00C4, 0x00E4, 0x00D4, 0x00F4, 0x00CC, 0x00EC, 0x00DC, 0x00FC};
#endif

class Arduino_UNOPAR8 : public Arduino_DataBus
{
public:
  Arduino_UNOPAR8(); // Constructor

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;
  void writeRepeat(uint16_t p, uint32_t len) override;
  void writeBytes(uint8_t *data, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

private:
  INLINE void WRITE(uint8_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
};

#endif // #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)

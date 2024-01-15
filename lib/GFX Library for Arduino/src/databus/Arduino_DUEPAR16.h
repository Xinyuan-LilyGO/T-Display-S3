#if defined(ARDUINO_ARCH_SAM)

#ifndef _ARDUINO_DUEPAR16_H_
#define _ARDUINO_DUEPAR16_H_

#include "Arduino_DataBus.h"

// for MCUFriend MEGA kind of shields on Arduino DUE. -jz-

class Arduino_DUEPAR16 : public Arduino_DataBus
{
  public:
    Arduino_DUEPAR16(); // Constructor

    bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
    void beginWrite() override;
    void endWrite() override;
    void writeCommand(uint8_t) override;
    void writeCommand16(uint16_t) override;
    void write(uint8_t) override;
    void write16(uint16_t) override;
    void writeRepeat(uint16_t p, uint32_t len) override;
    void writePixels(uint16_t *data, uint32_t len) override;

#if !defined(LITTLE_FOOT_PRINT)
  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;
#endif // !defined(LITTLE_FOOT_PRINT)

  protected:
  private:
    void setDataPins(uint8_t mode);
    void writeData16(uint16_t d, uint32_t num = 1);
    INLINE void WRITE16(uint16_t d);
    INLINE void WRITEREPEAT(uint16_t p, uint32_t len);
    INLINE void DC_HIGH(void);
    INLINE void DC_LOW(void);
    INLINE void CS_HIGH(void);
    INLINE void CS_LOW(void);
    int8_t _cs, _dc, _wr, _rd;
};

#endif // _ARDUINO_DUEPAR16_H_

#endif // #if defined(ARDUINO_ARCH_SAM)

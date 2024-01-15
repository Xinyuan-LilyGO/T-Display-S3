// Databus Adapter for the Arduino Wire bus (I2C bus)

#ifndef _Arduino_Wire_H_
#define _Arduino_Wire_H_

#include "Arduino_DataBus.h"
#include <Wire.h>

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif

class Arduino_Wire : public Arduino_DataBus
{
public:
  Arduino_Wire(uint8_t i2c_addr, TwoWire *wire = &Wire);

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

protected:
  void writeRegister(uint8_t reg, uint8_t *data, size_t len);
  uint8_t readRegister(uint8_t reg, uint8_t *data, size_t len);

  uint8_t output_buf = 0;
  bool is_found;

  uint8_t _i2c_addr;
  TwoWire *_wire;

  int32_t _speed;
private:
};

#endif // _Arduino_Wire_H_

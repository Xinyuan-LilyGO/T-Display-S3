#ifndef _ARDUINO_XCA9554SWSPI_H_
#define _ARDUINO_XCA9554SWSPI_H_

#include <Wire.h>

#include "Arduino_DataBus.h"

#define XCA9554_IIC_i2c_addr 0x38

#define XCA9554_INPUT_PORT_REG 0x00
#define XCA9554_OUTPUT_PORT_REG 0x01
#define XCA9554_INVERSION_PORT_REG 0x02
#define XCA9554_CONFIG_PORT_REG 0x03

class Arduino_XCA9554SWSPI : public Arduino_DataBus
{
public:
  Arduino_XCA9554SWSPI(int8_t rst, int8_t cs, int8_t sck, int8_t mosi, TwoWire *wire = &Wire, uint8_t i2c_addr = XCA9554_IIC_i2c_addr);

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

  void pinMode(uint8_t pin, uint8_t mode);
  void digitalWrite(uint8_t pin, uint8_t val);
  int digitalRead(uint8_t pin);

protected:
  void writeRegister(uint8_t reg, uint8_t *data, size_t len);
  uint8_t readRegister(uint8_t reg, uint8_t *data, size_t len);

  uint8_t output_buf = 0;
  bool is_found;

  int8_t _rst, _cs, _sck, _mosi;
  TwoWire *_wire;
  uint8_t _i2c_addr;

private:
};

#endif // _ARDUINO_XCA9554SWSPI_H_

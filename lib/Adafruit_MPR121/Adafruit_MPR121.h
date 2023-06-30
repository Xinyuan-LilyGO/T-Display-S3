/*!
 *  @file Adafruit_MPR121.h
 *
 *  This is a library for the MPR121 12-Channel Capacitive Sensor
 *
 *  Designed specifically to work with the MPR121 board.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/1982
 *
 *  These sensors use I2C to communicate, 2+ pins are required to interface
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_MPR121_H
#define ADAFRUIT_MPR121_H

#include "Arduino.h"
#include <Wire.h>

// The default I2C address
#define MPR121_I2CADDR_DEFAULT 0x5A ///< default I2C address

/*!
 *  Device register map
 */
enum {
  MPR121_TOUCHSTATUS_L = 0x00,
  MPR121_TOUCHSTATUS_H = 0x01,
  MPR121_FILTDATA_0L = 0x04,
  MPR121_FILTDATA_0H = 0x05,
  MPR121_BASELINE_0 = 0x1E,
  MPR121_MHDR = 0x2B,
  MPR121_NHDR = 0x2C,
  MPR121_NCLR = 0x2D,
  MPR121_FDLR = 0x2E,
  MPR121_MHDF = 0x2F,
  MPR121_NHDF = 0x30,
  MPR121_NCLF = 0x31,
  MPR121_FDLF = 0x32,
  MPR121_NHDT = 0x33,
  MPR121_NCLT = 0x34,
  MPR121_FDLT = 0x35,

  MPR121_TOUCHTH_0 = 0x41,
  MPR121_RELEASETH_0 = 0x42,
  MPR121_DEBOUNCE = 0x5B,
  MPR121_CONFIG1 = 0x5C,
  MPR121_CONFIG2 = 0x5D,
  MPR121_CHARGECURR_0 = 0x5F,
  MPR121_CHARGETIME_1 = 0x6C,
  MPR121_ECR = 0x5E,
  MPR121_AUTOCONFIG0 = 0x7B,
  MPR121_AUTOCONFIG1 = 0x7C,
  MPR121_UPLIMIT = 0x7D,
  MPR121_LOWLIMIT = 0x7E,
  MPR121_TARGETLIMIT = 0x7F,

  MPR121_GPIODIR = 0x76,
  MPR121_GPIOEN = 0x77,
  MPR121_GPIOSET = 0x78,
  MPR121_GPIOCLR = 0x79,
  MPR121_GPIOTOGGLE = 0x7A,

  MPR121_SOFTRESET = 0x80,
};

//.. thru to 0x1C/0x1D

/*!
 *  @brief  Class that stores state and functions for interacting with MPR121
 *  proximity capacitive touch sensor controller.
 */
class Adafruit_MPR121 {
public:
  // Hardware I2C
  Adafruit_MPR121();

  boolean begin(uint8_t i2caddr = MPR121_I2CADDR_DEFAULT, TwoWire *theWire = &Wire);

  uint16_t filteredData(uint8_t t);
  uint16_t baselineData(uint8_t t);

  uint8_t readRegister8(uint8_t reg);
  uint16_t readRegister16(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  uint16_t touched(void);
  // Add deprecated attribute so that the compiler shows a warning
  void setThreshholds(uint8_t touch, uint8_t release)
      __attribute__((deprecated));
  void setThresholds(uint8_t touch, uint8_t release);

private:
  int8_t _i2caddr;
  TwoWire *_wire;
};

#endif

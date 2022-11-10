/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 Micky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      TouchCommon.tpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#pragma once

#if defined(ARDUINO)
#include <Wire.h>
#endif

#ifdef _BV
#undef _BV
#endif
#define _BV(b) (1ULL << (uint64_t)(b))

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

#define COMBINE_H8L4_H(h, l)  (raw_data[h & 0xFF] << 4 | raw_data[l & 0xFF] >> 4)
#define COMBINE_H8L4_L(h, l)  (raw_data[h & 0xFF] << 4 | raw_data[l & 0xFF] & 0xF)

#define COMBINE_H4L8(h, l)    ((raw_data[h] & 0x0F) << 8 | raw_data[l])

#define IS_BIT_SET(val, mask) (((val) & (mask)) == (mask))

#if !defined(ARDUINO)
#define log_e(...)
#define log_i(...)
#define log_d(...)

#define LOW            0x0
#define HIGH           0x1

// GPIO FUNCTIONS
#define INPUT          0x01
#define OUTPUT         0x03
#define PULLUP         0x04
#define INPUT_PULLUP   0x05
#define PULLDOWN       0x08
#define INPUT_PULLDOWN 0x09

#define RISING         0x01
#define FALLING        0x02

#endif

template <class chipType> class TouchLibCommon {
  typedef int (*iic_fptr_t)(uint8_t devAddr, uint16_t regAddr, uint8_t *data, uint8_t len);

public:
#if defined(ARDUINO)
  bool begin(TwoWire &w, int sda, int scl, uint8_t addr, int rst) {
    if (__has_init)
      return thisChip().initImpl();
    __has_init = true;
    __wire = &w;
    __wire->begin(sda, scl);
    __addr = addr;
    __rst = rst;
    return thisChip().initImpl();
  }
#endif

  bool begin(uint8_t addr, int rst, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback) {
    if (__has_init)
      return thisChip().initImpl();
    __has_init = true;
    thisReadRegCallback = readRegCallback;
    thisWriteRegCallback = writeRegCallback;
    __addr = addr;
    __rst = rst;
    return thisChip().initImpl();
  }

  // private:
protected:
  int readRegister(uint8_t reg) {
    uint8_t val = 0;
    if (thisReadRegCallback != nullptr) {
      if (thisReadRegCallback(__addr, reg, &val, 1) != 0) {
        return 0;
      }
      return val;
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg);
      if (__wire->endTransmission() != 0) {
        return -1;
      }
      __wire->requestFrom(__addr, 1U);
      return __wire->read();
    }
#endif
    return -1;
  }
  int readRegister(uint16_t reg) {
    uint8_t val = 0;
    if (thisReadRegCallback != nullptr) {
      if (thisReadRegCallback(__addr, reg, &val, 1) != 0) {
        return 0;
      }
      return val;
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write((uint8_t)(reg >> 8));
      __wire->write((uint8_t)(reg & 0xff));
      if (__wire->endTransmission() != 0) {
        return -1;
      }
      __wire->requestFrom(__addr, 1U);
      return __wire->read();
    }
#endif
    return -1;
  }
  int writeRegister(uint8_t reg, uint8_t val) {
    if (thisWriteRegCallback) {
      return thisWriteRegCallback(__addr, reg, &val, 1);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg);
      __wire->write(val);
      return (__wire->endTransmission() == 0) ? 0 : -1;
    }
#endif
    return -1;
  }

  int writeRegister(uint16_t reg, uint8_t val) {
    if (thisWriteRegCallback) {
      return thisWriteRegCallback(__addr, reg, &val, 1);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg >> 8);
      __wire->write(reg & 0xff);
      __wire->write(val);
      return (__wire->endTransmission() == 0) ? 0 : -1;
    }
#endif
    return -1;
  }

  int writeRegister(uint16_t reg, uint16_t val) {
    if (thisWriteRegCallback) {
      return thisWriteRegCallback(__addr, reg, (uint8_t *)&val, 2);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg >> 8);
      __wire->write(reg & 0xff);
      __wire->write(val >> 8);
      __wire->write(val & 0xff);
      return (__wire->endTransmission() == 0) ? 0 : -1;
    }
#endif
    return -1;
  }

  int readRegister(uint8_t reg, uint8_t *buf, uint8_t lenght) {
    if (thisReadRegCallback) {
      return thisReadRegCallback(__addr, reg, buf, lenght);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg);
      if (__wire->endTransmission() != 0) {
        return -1;
      }
      __wire->requestFrom(__addr, lenght);
      return __wire->readBytes(buf, lenght) == lenght ? 0 : -1;
    }
#endif
    return -1;
  }

  int readRegister(uint16_t reg, uint8_t *buf, uint8_t lenght) {
    if (thisReadRegCallback) {
      return thisReadRegCallback(__addr, reg, buf, lenght);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg >> 8);
      __wire->write(reg & 0xff);
      if (__wire->endTransmission() != 0) {
        return -1;
      }
      __wire->requestFrom(__addr, lenght);
      return __wire->readBytes(buf, lenght) == lenght ? 0 : -1;
    }
#endif
    return -1;
  }

  int writeRegister(uint8_t reg, uint8_t *buf, uint8_t lenght) {
    if (thisWriteRegCallback) {
      return thisWriteRegCallback(__addr, reg, buf, lenght);
    }
#if defined(ARDUINO)
    if (__wire) {
      __wire->beginTransmission(__addr);
      __wire->write(reg);
      __wire->write(buf, lenght);
      return (__wire->endTransmission() == 0) ? 0 : -1;
    }
#endif
    return -1;
  }

  bool inline clrRegisterBit(uint8_t registers, uint8_t bit) {
    int val = readRegister(registers);
    if (val == -1) {
      return false;
    }
    return writeRegister(registers, (val & (~_BV(bit)))) == 0;
  }

  bool inline setRegisterBit(uint8_t registers, uint8_t bit) {
    int val = readRegister(registers);
    if (val == -1) {
      return false;
    }
    return writeRegister(registers, (val | (_BV(bit)))) == 0;
  }

  bool inline getRegisterBit(uint8_t registers, uint8_t bit) {
    int val = readRegister(registers);
    if (val == -1) {
      return false;
    }
    return val & _BV(bit);
  }

  uint16_t inline readRegisterH8L4(uint8_t highReg, uint8_t lowReg) {
    int h8 = readRegister(highReg);
    int l4 = readRegister(lowReg);
    if (h8 == -1 || l4 == -1)
      return 0;
    return (h8 << 4) | (l4 & 0x0F);
  }

  uint16_t inline readRegisterH8L5(uint8_t highReg, uint8_t lowReg) {
    int h8 = readRegister(highReg);
    int l5 = readRegister(lowReg);
    if (h8 == -1 || l5 == -1)
      return 0;
    return (h8 << 5) | (l5 & 0x1F);
  }

  uint16_t inline readRegisterH6L8(uint8_t highReg, uint8_t lowReg) {
    int h6 = readRegister(highReg);
    int l8 = readRegister(lowReg);
    if (h6 == -1 || l8 == -1)
      return 0;
    return ((h6 & 0x3F) << 8) | l8;
  }

  uint16_t inline readRegisterH5L8(uint8_t highReg, uint8_t lowReg) {
    int h5 = readRegister(highReg);
    int l8 = readRegister(lowReg);
    if (h5 == -1 || l8 == -1)
      return 0;
    return ((h5 & 0x1F) << 8) | l8;
  }

  /*
   * CRTP Helper
   */
protected:
  bool begin() {
#if defined(ARDUINO)
    if (__has_init)
      return thisChip().initImpl();
    __has_init = true;
    log_i("SDA:%d SCL:%d RST:%d", __sda, __scl, __rst);
    if (__rst != -1) {
      pinMode(__rst, OUTPUT);
      digitalWrite(__rst, 0);
      delay(200);
      digitalWrite(__rst, 1);
      delay(200);
    }
    __wire->begin(__sda, __scl);

#endif /*ARDUINO*/
    return thisChip().initImpl();
  }

  void end() {
#if defined(ARDUINO)
    if (__wire) {
#if defined(ESP_IDF_VERSION)
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 4, 0)
      __wire->end();
#endif /*ESP_IDF_VERSION*/
#endif /*ESP_IDF_VERSION*/
    }
#endif /*ARDUINO*/
  }

  inline const chipType &thisChip() const { return static_cast<const chipType &>(*this); }

  inline chipType &thisChip() { return static_cast<chipType &>(*this); }

protected:
  bool __has_init = false;
#if defined(ARDUINO)
  TwoWire *__wire = NULL;
#endif
  int __sda = -1;
  int __scl = -1;
  int __rst = -1;
  uint8_t __addr = 0xFF;
  iic_fptr_t thisReadRegCallback = NULL;
  iic_fptr_t thisWriteRegCallback = NULL;
};

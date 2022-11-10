/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 micky
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
 * @file      TouchLibCSTMutual.tpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include "REG/CSTMutualConstants.h"
#include "TouchLibCommon.tpp"
#include "TouchLibInterface.hpp"

class TouchLibCSTMutual : public TouchLibCommon<TouchLibCSTMutual>, public TouchLibInterface
{
  friend class TouchLibCommon<TouchLibCSTMutual>;

public:
#if defined(ARDUINO)
  TouchLibCSTMutual(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = CTS328_SLAVE_ADDRESS, int rst = -1)
  {
    __wire = &w;
    __sda = sda;
    __scl = scl;
    __addr = addr;
    __rst = rst;
  }
#endif

  TouchLibCSTMutual()
  {
#if defined(ARDUINO)
    __wire = &Wire;
    __sda = SDA;
    __scl = SCL;
    __rst = -1;
#endif
    __addr = CTS328_SLAVE_ADDRESS;
  }

  ~TouchLibCSTMutual()
  {
    log_i("~TouchLibCSTMutual");
    deinit();
  }

  bool init() { return begin(); }

  void deinit() { end(); }

  bool enableSleep() { return this->writeRegister((uint8_t)(CHIP_DEEP_SLEEP_REG >> 8), (uint8_t)(CHIP_DEEP_SLEEP_REG & 0xFF)); }

  bool read()
  {
    this->readRegister(MODE_NORMAL_0_REG, raw_data, sizeof(raw_data));
    this->writeRegister(MODE_NORMAL_0_REG, (uint8_t)0xAB); // sync signal
    // return raw_data[MODE_NORMAL_6_REG & 0xFF] == 0XAB ? 1 : 0;
    return ((raw_data[MODE_NORMAL_5_REG & 0xff] & 0x0f) != 0 ? 1 : 0);
  }

  uint8_t getPointNum() { return raw_data[MODE_NORMAL_5_REG & 0xff] & 0x0f; }

  TP_Point getPoint(uint8_t n)
  {
    TP_Point t;
    switch (n)
    {
    case 0:
      t.state = raw_data[MODE_NORMAL_0_REG & 0xFF] & 0xF;
      t.x = COMBINE_H8L4_H(MODE_NORMAL_1_REG, MODE_NORMAL_3_REG);
      t.y = COMBINE_H8L4_L(MODE_NORMAL_2_REG, MODE_NORMAL_3_REG);
      t.pressure = raw_data[MODE_NORMAL_4_REG & 0xF];
      break;
    case 1:
      t.state = raw_data[MODE_NORMAL_7_REG & 0xFF] & 0xF;
      t.x = COMBINE_H8L4_H(MODE_NORMAL_8_REG, MODE_NORMAL_10_REG);
      t.y = COMBINE_H8L4_L(MODE_NORMAL_9_REG, MODE_NORMAL_10_REG);
      t.pressure = raw_data[MODE_NORMAL_11_REG & 0xF];
      break;
    case 2:
      t.state = raw_data[MODE_NORMAL_12_REG & 0xFF] & 0xF;
      t.x = COMBINE_H8L4_H(MODE_NORMAL_13_REG, MODE_NORMAL_15_REG);
      t.y = COMBINE_H8L4_L(MODE_NORMAL_14_REG, MODE_NORMAL_15_REG);
      t.pressure = raw_data[MODE_NORMAL_16_REG & 0xF];
      break;
    case 3:
      t.state = raw_data[MODE_NORMAL_17_REG & 0xFF] & 0xF;
      t.x = COMBINE_H8L4_H(MODE_NORMAL_18_REG, MODE_NORMAL_20_REG);
      t.y = COMBINE_H8L4_L(MODE_NORMAL_19_REG, MODE_NORMAL_20_REG);
      t.pressure = raw_data[MODE_NORMAL_21_REG & 0xF];
      break;
    case 4:
      t.state = raw_data[MODE_NORMAL_22_REG & 0xFF] & 0xF;
      t.x = COMBINE_H8L4_H(MODE_NORMAL_23_REG, MODE_NORMAL_25_REG);
      t.y = COMBINE_H8L4_L(MODE_NORMAL_24_REG, MODE_NORMAL_25_REG);
      t.pressure = raw_data[MODE_NORMAL_26_REG & 0xF];
      break;
    default:
      log_i("The parameter range of getPoint is between 0 and 4.");
      break;
    }
    t.id = n;

    if (rotation == 0)
    {
    }
    else if (rotation == 1)
    {
      uint16_t tmp = t.x;
      t.x = t.y;
      t.y = tmp;
    }
    return t;
  }

  void setRotation(uint8_t r) { rotation = r % 4; }

  uint8_t getRotation() { return rotation; }

protected:
  bool initImpl() { return true; }
  uint8_t raw_data[27] = {0};
  uint8_t rotation = 0;
};
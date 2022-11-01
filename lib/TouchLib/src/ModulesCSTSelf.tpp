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
 * @file      ModulesCSTSelf.tpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include "REG/CSTSelfConstants.h"
#include "TouchLibCommon.tpp"
#include "TouchLibInterface.hpp"

class TouchLibCSTSelf : public TouchLibCommon<TouchLibCSTSelf>, public TouchLibInterface, public TouchLibGesture {
  friend class TouchLibCommon<TouchLibCSTSelf>;

public:
#if defined(ARDUINO)
  TouchLibCSTSelf(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = CTS826_SLAVE_ADDRESS, int rst = -1) {
    __wire = &w;
    __sda = sda;
    __scl = scl;
    __addr = addr;
    __rst = rst;
  }
#endif

  TouchLibCSTSelf() {
#if defined(ARDUINO)
    __wire = &Wire;
    __sda = SDA;
    __scl = SCL;
    __rst = -1;
#endif
    __addr = CTS826_SLAVE_ADDRESS;
  }

  ~TouchLibCSTSelf() {
    log_i("~TouchLibCSTSelf");
    deinit();
  }

  bool init() { return begin(); }

  void deinit() { end(); }

  bool enableSleep() { return this->writeRegister(SLEEP_REG, (uint8_t)(0x03)); }

  bool read() {
    this->readRegister(WORK_MODE_REG, raw_data, sizeof(raw_data));
    return raw_data[TOUCH_NUM_REG] > 0;
  }

  uint8_t getPointNum() { return raw_data[TOUCH_NUM_REG] & 0x0f; }

  TP_Point getPoint(uint8_t n) {
    TP_Point t;
    switch (n) {
    case 0:
      t.state = raw_data[TOUCH1_XH_REG] >> 6;
      t.x = COMBINE_H4L8(TOUCH1_XH_REG, TOUCH1_XL_REG);
      t.y = COMBINE_H4L8(TOUCH1_YH_REG, TOUCH1_YL_REG);
      break;
    case 1:
      t.state = raw_data[TOUCH2_XH_REG] >> 6;
      t.x = COMBINE_H4L8(TOUCH2_XH_REG, TOUCH2_XL_REG);
      t.y = COMBINE_H4L8(TOUCH2_YH_REG, TOUCH2_YL_REG);
      break;
    default:
      log_i("The parameter range of getPoint is between 0 and 1.");
      break;
    }
    t.id = n;

    if (rotation == 0) {
    } else if (rotation == 1) {
      uint16_t tmp = t.x;
      t.x = t.y;
      t.y = tmp;
    }
    return t;
  }

  void setRotation(uint8_t r) { rotation = r % 4; }

  uint8_t getRotation() { return rotation; }

  bool isEnableGesture() { return this->readRegister(GES_STATE_REG); }

  bool enableGesture() {
    uint8_t t = 0x01;
    return this->writeRegister(GES_STATE_REG, t);
  }

  bool disableGesture() {
    uint8_t t = 0x00;
    return this->writeRegister(GES_STATE_REG, t);
  }

  uint8_t getGesture(void) { return this->readRegister(GESTURE_ID_REG); }

protected:
  bool initImpl() { return true; }
  uint8_t raw_data[13] = {0};
  uint8_t rotation = 0;
};
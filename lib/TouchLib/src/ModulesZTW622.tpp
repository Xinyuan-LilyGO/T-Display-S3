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
 * @file      TouchLibZTW622.tpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include "REG/ZTW622Constants.h"
#include "TouchLibCommon.tpp"
#include "TouchLibInterface.hpp"

class TouchLibZTW622 : public TouchLibCommon<TouchLibZTW622>, public TouchLibInterface {
  friend class TouchLibCommon<TouchLibZTW622>;

public:
#if defined(ARDUINO)
  TouchLibZTW622(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = ZTW622_SLAVE_ADDRESS, int rst = -1) {
    __wire = &w;
    __sda = sda;
    __scl = scl;
    __addr = addr;
    __rst = rst;
  }
#endif

  TouchLibZTW622() {
#if defined(ARDUINO)
    __wire = &Wire;
    __sda = SDA;
    __scl = SCL;
    __rst = -1;
#endif
    __addr = ZTW622_SLAVE_ADDRESS;
  }

  ~TouchLibZTW622() {
    log_i("~TouchLibZTW622");
    deinit();
  }

  bool init() {
    begin();
    this->writeRegister((uint8_t)(CLEAR_INT_CMD_REG >> 8), CLEAR_INT_CMD_REG & 0xff);
    return 0;
  }

  void deinit() { end(); }

  bool enableSleep() { return 1; }

  bool read() {
    this->readRegister(REPORT_ALL_POINTS_INFO_REG, raw_data, 10);
    // this->readRegister(POINT_COUNT_TIMESTAMP_REG, &raw_data[2], 2);
    for (uint8_t i = 0; i < 10; i++) {
      Serial.printf("raw_data[%d]:0x%02X  ", i, raw_data[i]);
    }
    Serial.println();
    return 0;
  }

  uint8_t getPointNum() { return 1; }

  TP_Point getPoint(uint8_t n) {
    TP_Point t;
    return t;
  }
  void setRotation(uint8_t r) { rotation = r % 4; }

  uint8_t getRotation() { return 1; }

protected:
  bool initImpl() { return true; }
  uint8_t raw_data[27] = {0};
};
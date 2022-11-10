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
 * @file      ModulesGT911.tpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include "REG/GT911Constants.h"
#include "TouchLibCommon.tpp"
#include "TouchLibInterface.hpp"

class TouchLibGT911 : public TouchLibCommon<TouchLibGT911>, public TouchLibInterface {
  friend class TouchLibCommon<TouchLibGT911>;

public:
#if defined(ARDUINO)
  TouchLibGT911(TwoWire &w, int sda = SDA, int scl = SCL, uint8_t addr = GT911_SLAVE_ADDRESS2, int rst = -1) {
    __wire = &w;
    __sda = sda;
    __scl = scl;
    __addr = addr;
    __rst = rst;
  }
#endif

  TouchLibGT911() {
#if defined(ARDUINO)
    __wire = &Wire;
    __sda = SDA;
    __scl = SCL;
    __rst = -1;
#endif
    __addr = GT911_SLAVE_ADDRESS2;
  }

  ~TouchLibGT911() {
    log_i("~TouchLibGT911");
    deinit();
  }

  bool init() {
    begin();
    if (__rst == -1) {

      // this->writeRegister(GT911_COMMAND, (uint8_t)0x05);
      // delay(200);
      this->writeRegister(GT911_COMMAND, (uint8_t)0x02); // software reset
      delay(200);

      // this->writeRegister(GT911_COMMAND, (uint8_t)0x00);
      // uint8_t buf;
      // buf = this->readRegister(GT911_MODULE_SWITCH_1);
      // buf = buf & 0xfc;
      // buf = buf | 0x01;
      // this->writeRegister(GT911_MODULE_SWITCH_1, buf);

      // buf = this->readRegister(GT911_MODULE_SWITCH_1);
      // Serial.printf("2buf :0x%02X \r\n", buf);
      return 0;
    } else
      return 1;
  }

  void deinit() { end(); }

  bool enableSleep() { return 0; /* this->writeRegister(GT911_COMMAND, (uint8_t)0x05); */ }

  bool read() {
    this->readRegister(GT911_POINT_INFO, raw_data, sizeof(raw_data));
    this->writeRegister(GT911_POINT_INFO, (uint8_t)0x00); // sync signal
    return (raw_data[0] & 0xF) != 0 ? true : false;
  }

  uint8_t getPointNum() { return raw_data[0] & 0xF; }

  TP_Point getPoint(uint8_t n) {
    if (n > 4) {
      log_i("The parameter range of getPoint is between 0 and 4.");
      return TP_Point(0, 0, 0, 0, 0, 0);
    }

    TP_Point t;
    uint16_t point_reg[5] = GT911_POINTS_REG;
    uint16_t offset = point_reg[n] - GT911_POINT_INFO;

    t.id = raw_data[offset];
    t.x = raw_data[offset + 1] + (raw_data[offset + 2] << 8);
    t.y = raw_data[offset + 3] + (raw_data[offset + 4] << 8);
    t.size = raw_data[offset + 5] | (raw_data[offset + 6] << 8);

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

protected:
  bool initImpl() { return true; }
  uint8_t raw_data[40] = {0};
  uint8_t rotation = 0;
};
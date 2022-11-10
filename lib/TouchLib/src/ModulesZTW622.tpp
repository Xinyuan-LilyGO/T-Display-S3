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
    int ret = 0;
    begin();
    ret = this->writeRegister((uint16_t)(0x000C), (uint16_t)(0x0001));
    if (ret) {
      log_i("power sequence error (vendor cmd enable)");
      return false;
    }
    /* Read the value of the 0xCC00 address, the ID of the chip */
    ret = this->readRegister((uint16_t)(0xCC00), (uint8_t *)&chip_code, 2);
    if (ret) {
      log_i("fail to read chip code");
      return false;
    }
    log_i("chip code = 0x%x\n", chip_code);

    ret = this->writeRegister((uint8_t)(0xC0), (uint8_t)(0x04));
    if (ret) {
      log_i("power sequence error (intn clear)");
      return false;
    }

    ret = this->writeRegister((uint16_t)(0xC002), (uint16_t)(0x0001));
    if (ret) {
      log_i("power sequence error (nvm init)");
      return false;
    }

    ret = this->writeRegister((uint16_t)(0xC001), (uint16_t)(0x0001));
    if (ret) {
      log_i("power sequence error (program start)");
      return false;
    }
    delay(150);

    ret = this->readRegister(ZINITIX_FIRMWARE_VERSION, (uint8_t *)&firmware_version, 2);
    if (ret) {
      log_i("fail to read FIRMWARE_VERSION");
      return false;
    }
    /* 0x07 version cannot wake up from sleep */
    if (firmware_version != 0x07)
      ztw622_has_sleep_function = true;
    log_i("touch FW version = %d\r\n", firmware_version);

    ret = this->readRegister(ZINITIX_DATA_VERSION_REG, (uint8_t *)&reg_data_version, 2);
    if (ret) {
      log_i("fail to read reg version");
      return false;
    }
    log_i("touch register version = %d\r\n", reg_data_version);

    if (reg_data_version >= 3)
      ztw622_has_palm_coverage_detection = true;

    ret = this->writeRegister(ZINITIX_INITIAL_TOUCH_MODE, (uint16_t)(0x0001));
    if (ret) {
      log_i("fail to write inital touch mode");
      return false;
    }
    ret = this->writeRegister(ZINITIX_TOUCH_MODE, (uint16_t)(0x0001));
    if (ret) {
      log_i("fail to write touh mode");
      return false;
    }
    ret = this->writeRegister(ZINITIX_SUPPORTED_FINGER_NUM, (uint16_t)(0x0001));
    if (ret) {
      log_i("fail to write finger num");
      return false;
    }

    ret = this->writeRegister(ZINITIX_X_RESOLUTION, (uint16_t)(0x0001));
    if (ret) {
      log_i("fail to write finger num");
      return false;
    }
    ret = this->writeRegister(ZINITIX_Y_RESOLUTION, (uint16_t)(0x0001));
    if (ret) {
      log_i("fail to write finger num");
      return false;
    }
    ret = this->writeRegister((uint8_t)(0x00), (uint8_t)(0x06));
    if (ret) {
      log_i("fail to write finger num");
      return false;
    }
    for (int i = 0; i < 10; i++) {
      this->writeRegister((uint8_t)(0x00), (uint8_t)(0x03));
      delay(10);
    }

    return 1;
  }

  void deinit() { end(); }

  bool enableSleep() { return 1; }

  bool read() {
    this->readRegister(ZINITIX_POINT_STATUS_REG, raw_data, sizeof(raw_data));

    // for (int i = 0; i < 16; i++) {
    //   Serial.printf("[%d]0x%02X ", i, raw_data[i]);
    // }
    // Serial.println();

    // this->readRegister(POINT_COUNT_TIMESTAMP_REG, &raw_data[2], 2);

    // if ((raw_data[3] << 8 | raw_data[2]) != 0) {
    // this->writeRegister((uint8_t)(ZINITIX_CLEAR_INT_CMD >> 8), (uint8_t)(ZINITIX_CLEAR_INT_CMD & 0xFF));
    // return 1;
    // }
    // return 0;
    return (raw_data[1] & BIT_POINT_DETECTED);
  }

  uint8_t getPointNum() { return (raw_data[3] << 8 | raw_data[2]); }

  TP_Point getPoint(uint8_t n) {
    if (n > 4) {
      log_i("The parameter range of getPoint is between 0 and 4.");
      return TP_Point(0, 0, 0, 0, 0, 0);
    }

    TP_Point t;
    uint16_t point_reg[5] = ZINITIX_POINTS_REG;
    uint16_t offset = point_reg[n] - ZINITIX_POINT_STATUS_REG;

    t.id = n;
    t.x = raw_data[offset] + (raw_data[offset + 1] << 8);
    t.y = raw_data[offset + 2] + (raw_data[offset + 3] << 8);
    t.pressure = raw_data[offset + 4];
    t.state = raw_data[offset + 5];

    if (rotation == 0) {
    } else if (rotation == 1) {
      uint16_t tmp = t.x;
      t.x = t.y;
      t.y = tmp;
    }
    return t;
  }
  void setRotation(uint8_t r) { rotation = r % 4; }

  uint8_t getRotation() { return 1; }

protected:
  bool initImpl() { return true; }
  uint8_t raw_data[33] = {0};
  uint8_t rotation = 0;
  bool ztw622_has_sleep_function = false;
  bool ztw622_has_palm_coverage_detection = false;
  uint16_t chip_code = 0;
  uint16_t firmware_version = 0;
  uint16_t reg_data_version = 0;
};
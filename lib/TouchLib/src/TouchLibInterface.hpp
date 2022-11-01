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
 * @file      TouchLibInterface.hpp
 * @author    Micky (513673326@qq.com)
 * @date      2022-10-24
 *
 */

#pragma once

#include "stdint.h"

class TP_Point {
public:
  TP_Point(void){};
  ~TP_Point(){};
  TP_Point(uint8_t id, uint16_t x, uint16_t y, uint16_t size, uint8_t pressure, uint8_t state)
      : id(id), x(x), y(y), size(size), pressure(pressure), state(state) {}

  bool operator==(TP_Point point) {
    return ((point.x == x) && (point.y == y) && (point.size == size) && (point.pressure == pressure) && (point.state == state));
  }
  bool operator!=(TP_Point point) {
    return ((point.x != x) || (point.y != y) || (point.size != size) || (point.pressure != pressure) || (point.state != state));
  }

  uint8_t id;
  uint16_t x;
  uint16_t y;
  uint8_t size;
  uint8_t pressure;
  uint8_t state;
};

class TouchLibInterface {
public:
  TouchLibInterface() {}
  virtual ~TouchLibInterface() {}

  virtual bool init() = 0;

  virtual void deinit() = 0;

  virtual bool enableSleep() = 0;

  virtual bool read() = 0;

  virtual uint8_t getPointNum() = 0;

  virtual TP_Point getPoint(uint8_t n) = 0;

  // virtual void setRotation(uint8_t r) = 0;

  virtual uint8_t getRotation() = 0;
};

class TouchLibGesture {
public:
  TouchLibGesture() {}
  virtual ~TouchLibGesture() {}

  virtual bool isEnableGesture() = 0;

  virtual bool enableGesture() = 0;

  virtual bool disableGesture() = 0;

  virtual uint8_t getGesture(void) = 0;
};
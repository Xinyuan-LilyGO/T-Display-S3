#pragma once

#include "Wire.h"

#define CST328_ADDRESS   0X1A
#define CST328_RAW_MAX_X 170
#define CST328_RAW_MAX_Y 320

enum {
  ENUM_MODE_DEBUG_INFO = 0xD101,
  ENUM_SYSTEM_RESET = 0XD102,
  ENUM_REDO_CALIBRATION = 0XD104,
  ENUM_DEEP_SLEEP = 0XD105,
  ENUM_MODE_DEBUG_POINTS = 0xD108,
  ENUM_MODE_NORMAL = 0xD109,
  ENUM_MODE_DEBUG_RAWDATA = 0xD10A,
  ENUM_MODE_DEBUG_WRITE = 0xD10B,
  ENUM_MODE_DEBUG_CALIBRATION = 0xD10C,
  ENUM_MODE_DEBUG_DIFF = 0xD10D,
  ENUM_MODE_FACTORY = 0xD119,
} cst3xx_mode_t;

typedef enum {
  TOUCH_RELEASED = 0X00,
  TOUCH_PRESS = 0X06,
} point_state_t;

typedef struct {
  uint32_t x;          // x coordinate
  uint32_t y;          // y coordinate
  uint8_t pressure;    // Pressure value
  point_state_t state; //
  uint8_t finger_id;
} finger_point_t;

typedef struct {
  finger_point_t point[5];
  uint8_t finger_num;
  bool is_updata;
} touch_info_t;

class CSTXXX {
public:
  CSTXXX(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y) {
    reso_x = max_x - min_x;
    reso_y = max_y - min_y;
  }
  ~CSTXXX() {}

  bool init(TwoWire &wire, uint8_t res_pin = -1, uint8_t int_pin = -1);
  bool get_touch_point(touch_info_t *t);
  bool is_update();

private:
  void _wirte_reg(uint8_t *data, uint8_t len);
  void _wirte_reg(uint16_t reg, uint8_t *data, uint8_t len);
  void _read_reg(uint16_t reg, uint8_t *data, uint8_t len);
  TwoWire *_wire = nullptr;
  uint8_t _addr;
  uint8_t _res_pin;
  uint8_t _int_pin;
  uint16_t reso_x;
  uint16_t reso_y;
  float prop_x;
  float prop_y;
};
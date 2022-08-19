#include "cstxx.h"
#include "Arduino.h"

void CSTXXX::_wirte_reg(uint8_t *data, uint8_t len) {
  _wire->beginTransmission(_addr);
  _wire->write(data, len);
  _wire->endTransmission();
}
void CSTXXX::_wirte_reg(uint16_t reg, uint8_t *data, uint8_t len) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(data, len);
  _wire->endTransmission();
}
void CSTXXX::_read_reg(uint16_t reg, uint8_t *data, uint8_t len) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(_addr, len);
  uint8_t index = 0;
  while (_wire->available())
    // while (index < len)
    data[index++] = _wire->read();
}

bool CSTXXX::init(TwoWire &wire, uint8_t res_pin, uint8_t int_pin) {
  _wire = &wire;
  _res_pin = res_pin;
  _int_pin = int_pin;
  _addr = CST328_ADDRESS;

  pinMode(_int_pin, INPUT_PULLUP);

  if (_res_pin >= 0) {
    pinMode(_res_pin, OUTPUT);
    digitalWrite(_res_pin, HIGH);
    delay(50);
    digitalWrite(_res_pin, LOW);
    delay(200);
    digitalWrite(_res_pin, HIGH);
    delay(200);
  } else {
    uint8_t buf[4];
    buf[0] = 0xD1;
    buf[1] = 0x0E;
    _wirte_reg(buf, 2);
    delay(200);
  }

  prop_x = CST328_RAW_MAX_X / reso_x;
  prop_y = CST328_RAW_MAX_Y / reso_y;
  Serial.printf("prop_x:%f  prop_y:%f\r\n", prop_x, prop_y);

  return true;
}

bool CSTXXX::get_touch_point(touch_info_t *t) {
  //   touch_info_t _t;
  uint8_t raw_data[27];
  //   cst3xx_data_t data;
  //   memset(&t, 0, sizeof(touch_info_t));
  _read_reg(0xD000, raw_data, 27);

  // for (uint8_t i = 0; i < 27; i++) {
  //   Serial.printf("0x%X :", 0xD000 + i);
  //   Serial.println(data.raw[i]);
  // }

  t->finger_num = raw_data[5] & 0x0f;
  //   _t.finger_num = data.param.finger_num;
  //   _t.finger_num = Serial.println(data.param.f1_id);
  if (t->finger_num != 0) {
    t->point[0].pressure = raw_data[4];

    t->point[0].x = (raw_data[1] << 4 | raw_data[3] >> 4) / prop_x;
    t->point[0].y = (raw_data[2] << 4 | (raw_data[3] & 0x0f)) / prop_y;
    t->point[0].state = (point_state_t)(raw_data[0] & 0x0f);
    t->point[0].finger_id = raw_data[0] >> 4;

    for (uint8_t i = 1; i < t->finger_num; i++) {
      t->point[i].state = (point_state_t)(raw_data[(i * 5) + 2] & 0x0f);
      t->point[i].finger_id = raw_data[(i * 5) + 2] >> 4;
      t->point[i].x = (raw_data[(i * 5) + 3] << 4 | raw_data[(i * 5) + 5] >> 4) / prop_x;
      t->point[i].y = (raw_data[(i * 5) + 4] << 4 | (raw_data[(i * 5) + 5] & 0x0f)) / prop_x;
      t->point[i].pressure = raw_data[(i * 5) + 6];
    }

    // _t.point[0].pressure = data.param.f1_pressure;
    // _t.point[0].x = data.param.f1_x_h << 4 | data.param.f1_x_l;
    // _t.point[0].y = data.param.f1_y_h << 4 | data.param.f1_y_l;
    // _t.point[0].state = (point_state_t)data.param.f1_state;

    // _t.point[1].pressure = data.param.f1_pressure;
    // _t.point[1].x = data.param.f2_x_h << 4 | data.param.f2_x_l;
    // _t.point[1].y = data.param.f2_y_h << 4 | data.param.f2_y_l;
    // _t.point[1].state = (point_state_t)data.param.f2_state;

    // _t.point[2].pressure = data.param.f1_pressure;
    // _t.point[2].x = data.param.f3_x_h << 4 | data.param.f3_x_l;
    // _t.point[2].y = data.param.f3_y_h << 4 | data.param.f3_y_l;
    // _t.point[2].state = (point_state_t)data.param.f3_state;

    // _t.point[3].pressure = data.param.f1_pressure;
    // _t.point[3].x = data.param.f4_x_h << 4 | data.param.f4_x_l;
    // _t.point[3].y = data.param.f4_y_h << 4 | data.param.f4_y_l;
    // _t.point[3].state = (point_state_t)data.param.f4_state;

    // _t.point[4].pressure = data.param.f1_pressure;
    // _t.point[4].x = data.param.f5_x_h << 4 | data.param.f5_x_l;
    // _t.point[4].y = data.param.f5_y_h << 4 | data.param.f5_y_l;
    // _t.point[4].state = (point_state_t)data.param.f5_state;

    // memcpy(t, &_t, sizeof(touch_info_t));
    return true;
  }

  return false;
}